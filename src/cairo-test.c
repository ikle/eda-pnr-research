/*
 * PCB Cairo Test
 *
 * Copyright (c) 2009-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/* EDA Form */

#include <stdlib.h>

struct eda_form {
	int X, Y;
	unsigned W, H;
	signed char *map;
};

static inline int
eda_form_init (struct eda_form *o, int x, int y, unsigned w, unsigned h)
{
	o->X = x;
	o->Y = y;
	o->W = w;
	o->H = h;

	return (o->map = malloc (w * h)) != NULL;
}

static inline void eda_form_fini (struct eda_form *o)
{
	free (o->map);
}

static inline void
eda_form_point (struct eda_form *o, int x, int y, int color)
{
	o->map[x + y * o->W] = 0;
}

static inline void
eda_form_hline (struct eda_form *o, int xs, int xe, int y, int color)
{
	signed char *pt = o->map + o->W * y + xs;

	for (; xs < xe; ++xs, ++pt)  *pt = color;
}

static inline void
eda_form_vline (struct eda_form *o, int x, int ys, int ye, int color)
{
	signed char *pt = o->map + o->W * ys + x;

	for (; ys < ye; ++ys, pt += o->W)  *pt = color;
}

/* EDA Form Utils */

#include <cairo.h>

int eda_form_push_surface (struct eda_form *o, cairo_surface_t *s)
{
	const int S = cairo_image_surface_get_stride (s);
	unsigned char *src;
	signed char *dst = o->map;
	int i, j;

	if ((src = cairo_image_surface_get_data (s)) == NULL)
		goto no_src;

	for (j = 0; j < o->H; ++j, src += S, dst += o->W)
		for (i = 0; i < o->W; ++i)
			dst[i] = src[i] == 0 ? 1 : -1;

	cairo_surface_destroy (s);
	return 1;
no_src:
	cairo_surface_destroy (s);
	return 0;
}

int eda_form_mask (struct eda_form *o, size_t w, size_t h, size_t sp,
		   void (*draw) (cairo_t *c, size_t w, size_t h, size_t sp))
{
	cairo_surface_t *s;
	cairo_t *c;

	s = cairo_image_surface_create (CAIRO_FORMAT_A8, o->W, o->H);
	c = cairo_create (s);

	cairo_set_antialias  (c, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_cap   (c, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join  (c, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_width (c, sp << 1);

	draw (c, w, h, sp);
	cairo_fill_preserve (c);
	cairo_stroke (c);

	cairo_destroy (c);
	cairo_surface_flush (s);
	return eda_form_push_surface (o, s);
}

/* EDA Form Pad */

#include <cairo.h>

static void eda_form_pad_draw (cairo_t *c, size_t w, size_t h, size_t sp)
{
	cairo_rectangle (c, sp, sp, w, h);
}

int eda_form_pad (struct eda_form *o, size_t w, size_t h, size_t space)
{
	const size_t W = w + (space << 1), H = h + (space << 1);

	if (!eda_form_init (o, W >> 1, H >> 1, W, H))
		return 0;

	if (!eda_form_mask (o, w, h, space, eda_form_pad_draw))
		goto no_mask;

	eda_form_hline (o, 0, W, o->Y, 1);	/* channel	*/
	eda_form_point (o, o->X, o->Y, 0);	/* target	*/
	return 1;
no_mask:
	eda_form_fini (o);
	return 0;
}

/* Experiments */

#include <stdio.h>
#include <stdlib.h>

#include <cairo.h>

static void eda_form_show (struct eda_form *o)
{
	signed char *line = o->map;
	size_t i, j;

	for (j = 0; j < o->H; ++j, line += o->W, putchar ('\n'))
		for (i = 0; i < o->W; ++i)
			putchar (line[i] < 0 ? '+' : line[i] == 0 ? 'o': ' ');
}

struct pcb_pad {
	int rx, ry, sp;
};

void pcb_pad_show (const struct pcb_pad *o, cairo_t *c, int x, int y)
{
	const int X = (x - o->rx), Y = (y - o->ry);
	const int W = (o->rx << 1) | 1, H = (o->ry << 1) | 1;

	cairo_rectangle (c, X, Y, W, H);
	cairo_fill (c);
}

void pcb_pad_mask (const struct pcb_pad *o, cairo_t *c, int x, int y)
{
	const int X = (x - o->rx), Y = (y - o->ry), dx = (o->rx << 1);
	const int W = dx | 1, H = (o->ry << 1) | 1;
	const int D = (o->sp << 1);

	cairo_save (c);
//	cairo_set_antialias (c, CAIRO_ANTIALIAS_NONE);

	cairo_set_fill_rule (c, CAIRO_FILL_RULE_EVEN_ODD);
	cairo_rectangle (c, X - o->sp, Y - o->sp, W + D, H + D);
	cairo_rectangle (c, X - o->sp, y, o->sp + 1, 1);
	cairo_rectangle (c, X + dx,    y, o->sp + 1, 1);
	cairo_clip (c);

	cairo_rectangle (c, X, Y, W, H);
	cairo_fill_preserve (c);

	cairo_set_line_cap (c, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join (c, CAIRO_LINE_JOIN_ROUND);
	cairo_set_line_width (c, D);
	cairo_stroke (c);

	cairo_restore (c);
}

static void check (const char *domain, int status)
{
	if (status == CAIRO_STATUS_SUCCESS)
		return;

	fprintf (stderr, "E: %s: %s\n", domain, cairo_status_to_string (status));
	exit (1);
}

int main (int argc, char *argv[])
{
	const int W = 402, H = 302;
	cairo_surface_t *s;
	cairo_t *c;
	struct pcb_pad pad = {20, 7, 7};
	int i;

	s = cairo_image_surface_create (CAIRO_FORMAT_A8, W, H);
	check ("Surface", cairo_surface_status (s));

	c = cairo_create (s);
	check ("Context", cairo_status (c));

	cairo_set_source_rgba (c, 0, 0, 0, 1);
	cairo_set_operator (c, CAIRO_OPERATOR_SOURCE);
	cairo_paint (c);

	cairo_set_source_rgba (c, 0, 0, 0, 0);
	cairo_rectangle (c, 1, 1, W - 2, H - 2);
	cairo_fill (c);

	cairo_set_source_rgba (c, 0, 0, 0, 1);

	for (i = 0; i < 8; ++i)  pcb_pad_mask (&pad, c,  62, 52 + 30 * i);
	for (i = 0; i < 8; ++i)  pcb_pad_mask (&pad, c, 201, 52 + 30 * i);
	for (i = 0; i < 8; ++i)  pcb_pad_mask (&pad, c, 338, 52 + 30 * i);

	cairo_destroy (c);

	check ("Write", cairo_surface_write_to_png (s, "cairo-test.png"));
	cairo_surface_destroy (s);

	struct eda_form form;

	if (!eda_form_pad (&form, 41, 15, 7)) {
		fprintf (stderr, "E: Cannot form pad mask\n");
		return 1;
	}

	eda_form_show (&form);
	eda_form_fini (&form);
	return 0;
}
