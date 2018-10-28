#include <err.h>

#include <gtest/gtest.h>

extern "C" {
#include "xcore.h"
};

/* TODO either fix googletest's [ASSERT|EXPECT]_EXIT* on openbsd or switch */

/* xfont tests */

TEST(xfont, EmptyString_ShouldExit)
{
   xfont_settings_t s = {
      .desc = (char*)"",
      .fgcolor = (char*)""
   };
   ASSERT_EXIT(xfont_init(&s), ::testing::ExitedWithCode(1), "");
}

TEST(xfont, FixedUsingPt_ShouldExit)
{
   xfont_settings_t s = {
      .desc = (char*)"fixed 12pt",
      .fgcolor = (char*)""
   };
   ASSERT_EXIT(xfont_init(&s), ::testing::ExitedWithCode(1), "");
}

TEST(xfont, FixedUsingPx_ShouldSucced)
{
   xfont_settings_t s = {
      .desc = (char*)"fixed 12px",
      .fgcolor = (char*)""
   };
   xfont_t *f = xfont_init(&s);
   ASSERT_EQ(f->height, 12);
   xfont_free(f);
}

TEST(xfont, FixedUsingNoSuffix_ShouldSucced)
{
   xfont_settings_t s = {
      .desc = (char*)"fixed 12",
      .fgcolor = (char*)""
   };
   xfont_t *f = xfont_init(&s);
   ASSERT_EQ(f->height, 12);
   xfont_free(f);
}

TEST(xfont, TimesBold48_ShouldSucceed)
{
   xfont_settings_t s = {
      .desc = (char*)"Times Bold 48",
      .fgcolor = (char*)""
   };
   xfont_t *f = xfont_init(&s);
   ASSERT_EQ(f->height, 48);
   xfont_free(f);
}

TEST(xfont, TimesOrHelveticaBold8px_ShouldSucceed)
{
   xfont_settings_t s = {
      .desc = (char*)"Times,Helvetica Bold 8px",
      .fgcolor = (char*)""
   };
   xfont_t *f = xfont_init(&s);
   ASSERT_EQ(f->height, 8);
   xfont_free(f);
}

TEST(xfont, XFontFree_ShouldSucceed)
{
   xfont_settings_t s = {
      .desc = (char*)"Times,Helvetica Bold 8px",
      .fgcolor = (char*)""
   };
   xfont_t *f = xfont_init(&s);
   ASSERT_EQ(f->height, 8);
   xfont_free(f);
}

/* xdisp tests */

TEST(xdisp, Init_IsntNull)
{
   xdisp_t *x = xdisp_init();
   ASSERT_NE(x, NULL);
}

TEST(xdisp, Free_ShouldSucced)
{
   xdisp_t *x = xdisp_init();
   xdisp_free(x);
}

TEST(xdisp, Dimensions_AreReasonable)
{
   xdisp_t *x = xdisp_init();
   ASSERT_GT(x->display_width,  0);
   ASSERT_GT(x->display_height, 0);

   ASSERT_LT(x->display_width,  10000);
   ASSERT_LT(x->display_height, 10000);
   xdisp_free(x);
}

/* xwin tests */

TEST(xwin, Init_ShouldSucceed)
{
   xwin_settings_t s = {
      .bgcolor = (char*) "ff0000",
      .wname   = (char*) "test",
      .x = 1, .y = 2,
      .w = 3, .h = 4
   };

   xdisp_t *x = xdisp_init();
   xwin_t *w = xwin_init(x, &s);
   xwin_free(w);
}

TEST(xwin, Init_OutputSettingsShouldMatchInput)
{
   xwin_settings_t s = {
      .bgcolor = (char*) "ff0000",
      .wname   = (char*) "test",
      .x = 1, .y = 2,
      .w = 3, .h = 4
   };

   xdisp_t *x = xdisp_init();
   xwin_t *w = xwin_init(x, &s);
   ASSERT_STREQ(s.bgcolor, w->settings->bgcolor);
   ASSERT_STREQ(s.wname, w->settings->wname);
   ASSERT_EQ(s.x, w->settings->x);
   ASSERT_EQ(s.y, w->settings->y);
   ASSERT_EQ(s.w, w->settings->w);
   ASSERT_EQ(s.h, w->settings->h);
   xwin_free(w);
}

/* hex2rgba tests */

TEST(hex2rgba, EmptyString_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, PoundEmptyString_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("#",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, FooString_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("foo",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, PoundFooString_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("#foo",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, OneTwoAndFiveNumbers_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("f",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("ff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("fffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, PoundOneTwoAndFiveNumbers_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("#f",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("#ff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("#fffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, SevenNineTenNumbers_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("fffffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("fffffffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("ffffffffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, PoundSevenNineTenNumbers_ShouldExit)
{
   double r, g, b, a;
   ASSERT_EXIT(hex2rgba("#fffffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("#fffffffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
   ASSERT_EXIT(hex2rgba("#ffffffffff",&r,&g,&b,&a), ::testing::ExitedWithCode(1), "");
}

TEST(hex2rgba, Red3_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("f00", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 1.0);
}

TEST(hex2rgba, PoundRed3_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("#f00", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 1.0);
}

TEST(hex2rgba, Red4_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("f003", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 0.2);
}

TEST(hex2rgba, PoundRed4_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("#f003", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 0.2);
}

TEST(hex2rgba, Red6_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("ff0000", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 1.0);
}

TEST(hex2rgba, PoundRed6_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("#ff0000", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 1.0);
}

TEST(hex2rgba, Red8_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("ff000033", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 0.2);
}

TEST(hex2rgba, PoundRed8_ShouldSucceed)
{
   double r, g, b, a;
   hex2rgba("ff000033", &r, &g, &b, &a);
   ASSERT_EQ(r, 1.0);
   ASSERT_EQ(g, 0.0);
   ASSERT_EQ(b, 0.0);
   ASSERT_EQ(a, 0.2);
}
