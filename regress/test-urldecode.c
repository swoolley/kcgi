/*	$Id$ */
/*
 * Copyright (c) 2018 Kristaps Dzonsons <kristaps@bsd.lv>
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
#include "../config.h"

#if HAVE_ERR
# include <err.h>
#endif

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../kcgi.h"

struct	test {
	enum kcgi_err	 errorcode;
	const char	*input;
	const char	*output;
};

static	const struct test tests[] = {
	{ KCGI_OK, "", "" },
	{ KCGI_OK, "foobar", "foobar" },
	{ KCGI_OK, "foo+bar", "foo bar" },
	{ KCGI_OK, "foo~bar", "foo~bar" },
	{ KCGI_OK, "foo-bar", "foo-bar" },
	{ KCGI_OK, "foo_bar", "foo_bar" },
	{ KCGI_OK, "foo.bar", "foo.bar" },
	{ KCGI_OK, "foo.bar.", "foo.bar." },
	{ KCGI_OK, "foo.bar.-", "foo.bar.-" },
	{ KCGI_OK, "-_foo.bar.-", "-_foo.bar.-" },
	{ KCGI_OK, "-_foo%2bbar.-", "-_foo+bar.-" },
	{ KCGI_OK, "-_foo%09bar.-", "-_foo\tbar.-" },
	{ KCGI_OK, "%09-_foo%09bar.-", "\t-_foo\tbar.-" },
	{ KCGI_OK, "%09-_foo%09bar.-%09", "\t-_foo\tbar.-\t" },
	{ KCGI_OK, "%09-_foo%25%09bar.-%09", "\t-_foo%\tbar.-\t" },
	{ KCGI_OK, "-_foo%2509%7dbar.-", "-_foo%09}bar.-" },
	{ KCGI_OK, "%09%09%09%09", "\t\t\t\t" },
	{ KCGI_FORM, "foo%zubar", "foo%zubar" },
	{ KCGI_FORM, "foo%", "foo%" },
	{ KCGI_OK, NULL, NULL }
};

int
main(int argc, char *argv[])
{
	const struct test *t;
	char	*url;
	enum kcgi_err code;

	url = NULL;
	for (t = tests; NULL != t->input; t++) {
		code = kutil_urldecode(t->input, &url);
		if (KCGI_ENOMEM == code)
			return(EXIT_FAILURE);
		if (t->errorcode != code)
			errx(EXIT_FAILURE, "%s: fail (returned %i, "
				"wanted %i)", t->input, code, t->errorcode);
		if (strcmp(url, t->output))
			errx(EXIT_FAILURE, "%s: fail (have %s, "
				"want %s)", t->input, url, t->output);
		free(url);
	}

	return(EXIT_SUCCESS);
}
