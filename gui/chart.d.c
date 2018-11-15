/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Test driver (system test) for chart.* component
 * Run by just:
 *  ./chart.d
 */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "chart.h"

int
main()
{
	const char *colors[] = {
		"#ff0000",
		"#00ff00",
		"#0000ff"
	};

	struct chart *c;

	c = chart_init(
		10, 3,      /* 10 samples of 3 different series */
		false,      /* raw values (not percentages) */
		"#555555",
		colors);

	double i = 1;
	while (1) {
		chart_update(c, (double[]){ i, i + 1, i + 2 });
		i++;
		chart_print(c);
		sleep(1);
	}

	chart_free(c);
	return 0;
}
