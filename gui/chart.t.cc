#include <err.h>

#include <gtest/gtest.h>

extern "C" {
#include "chart.h"
};


TEST(chart, InitZeroSeries_ShouldFail)
{
	ASSERT_EXIT(chart_init(0, 20, true, "", (const char*[]){ (const char*)"" }),
		::testing::ExitedWithCode(1), "");
}

TEST(chart, InitZeroSamples_ShouldFail)
{
	ASSERT_EXIT(chart_init(20, 0, true, "", (const char*[]){ (const char*)"" }),
		::testing::ExitedWithCode(1), "");
}

TEST(chart, InitWithNullBgcolor_ShouldFail)
{
	ASSERT_EXIT(chart_init(1, 10, true, NULL, (const char*[]){ (const char*)"" }),
		::testing::ExitedWithCode(1), "");
}

TEST(chart, InitWithNullColorArray_ShouldFail)
{
	ASSERT_EXIT(chart_init(1, 10, true, "", NULL),
		::testing::ExitedWithCode(1), "");
}

TEST(chart, InitFree_ShouldSucceed)
{
	struct chart *c = chart_init(10, 1, true, "#444",
		(const char *[]){ (const char*)"#f00" });
	chart_free(c);
}
