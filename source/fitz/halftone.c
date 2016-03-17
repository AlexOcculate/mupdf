#include "mupdf/fitz.h"

fz_halftone *
fz_new_halftone(fz_context *ctx, int comps)
{
	fz_halftone *ht;
	int i;

	ht = fz_malloc(ctx, sizeof(fz_halftone) + (comps-1)*sizeof(fz_pixmap *));
	ht->refs = 1;
	ht->n = comps;
	for (i = 0; i < comps; i++)
		ht->comp[i] = NULL;

	return ht;
}

fz_halftone *
fz_keep_halftone(fz_context *ctx, fz_halftone *ht)
{
	if (ht)
		ht->refs++;
	return ht;
}

void
fz_drop_halftone(fz_context *ctx, fz_halftone *ht)
{
	int i;

	if (!ht || --ht->refs != 0)
		return;
	for (i = 0; i < ht->n; i++)
		fz_drop_pixmap(ctx, ht->comp[i]);
	fz_free(ctx, ht);
}

/* Default mono halftone, lifted from Ghostscript. */
/* The 0x00 entry has been changed to 0x01 to avoid problems with white
 * pixels appearing in the output; as we use < 0 should not appear in the
 * array. I think that gs scales this slighly and hence never actually uses
 * the raw values here. */
static unsigned char mono_ht[] =
{
	0x0E, 0x8E, 0x2E, 0xAE, 0x06, 0x86, 0x26, 0xA6, 0x0C, 0x8C, 0x2C, 0xAC, 0x04, 0x84, 0x24, 0xA4,
	0xCE, 0x4E, 0xEE, 0x6E, 0xC6, 0x46, 0xE6, 0x66, 0xCC, 0x4C, 0xEC, 0x6C, 0xC4, 0x44, 0xE4, 0x64,
	0x3E, 0xBE, 0x1E, 0x9E, 0x36, 0xB6, 0x16, 0x96, 0x3C, 0xBC, 0x1C, 0x9C, 0x34, 0xB4, 0x14, 0x94,
	0xFE, 0x7E, 0xDE, 0x5E, 0xF6, 0x76, 0xD6, 0x56, 0xFC, 0x7C, 0xDC, 0x5C, 0xF4, 0x74, 0xD4, 0x54,
	0x01, 0x81, 0x21, 0xA1, 0x09, 0x89, 0x29, 0xA9, 0x03, 0x83, 0x23, 0xA3, 0x0B, 0x8B, 0x2B, 0xAB,
	0xC1, 0x41, 0xE1, 0x61, 0xC9, 0x49, 0xE9, 0x69, 0xC3, 0x43, 0xE3, 0x63, 0xCB, 0x4B, 0xEB, 0x6B,
	0x31, 0xB1, 0x11, 0x91, 0x39, 0xB9, 0x19, 0x99, 0x33, 0xB3, 0x13, 0x93, 0x3B, 0xBB, 0x1B, 0x9B,
	0xF1, 0x71, 0xD1, 0x51, 0xF9, 0x79, 0xD9, 0x59, 0xF3, 0x73, 0xD3, 0x53, 0xFB, 0x7B, 0xDB, 0x5B,
	0x0D, 0x8D, 0x2D, 0xAD, 0x05, 0x85, 0x25, 0xA5, 0x0F, 0x8F, 0x2F, 0xAF, 0x07, 0x87, 0x27, 0xA7,
	0xCD, 0x4D, 0xED, 0x6D, 0xC5, 0x45, 0xE5, 0x65, 0xCF, 0x4F, 0xEF, 0x6F, 0xC7, 0x47, 0xE7, 0x67,
	0x3D, 0xBD, 0x1D, 0x9D, 0x35, 0xB5, 0x15, 0x95, 0x3F, 0xBF, 0x1F, 0x9F, 0x37, 0xB7, 0x17, 0x97,
	0xFD, 0x7D, 0xDD, 0x5D, 0xF5, 0x75, 0xD5, 0x55, 0xFF, 0x7F, 0xDF, 0x5F, 0xF7, 0x77, 0xD7, 0x57,
	0x02, 0x82, 0x22, 0xA2, 0x0A, 0x8A, 0x2A, 0xAA, 0x01 /*0x00*/, 0x80, 0x20, 0xA0, 0x08, 0x88, 0x28, 0xA8,
	0xC2, 0x42, 0xE2, 0x62, 0xCA, 0x4A, 0xEA, 0x6A, 0xC0, 0x40, 0xE0, 0x60, 0xC8, 0x48, 0xE8, 0x68,
	0x32, 0xB2, 0x12, 0x92, 0x3A, 0xBA, 0x1A, 0x9A, 0x30, 0xB0, 0x10, 0x90, 0x38, 0xB8, 0x18, 0x98,
	0xF2, 0x72, 0xD2, 0x52, 0xFA, 0x7A, 0xDA, 0x5A, 0xF0, 0x70, 0xD0, 0x50, 0xF8, 0x78, 0xD8, 0x58
};

fz_halftone *fz_default_halftone(fz_context *ctx, int num_comps)
{
	fz_halftone *ht = fz_new_halftone(ctx, num_comps);

	fz_try(ctx)
	{
		int i;
		for (i = 0; i < num_comps; i++)
			ht->comp[i] = fz_new_pixmap_with_data(ctx, NULL, 16, 16, mono_ht);
	}
	fz_catch(ctx)
	{
		fz_drop_halftone(ctx, ht);
		fz_rethrow(ctx);
	}

	return ht;
}

/* Finally, code to actually perform halftoning. */
static void make_ht_line(unsigned char *buf, fz_halftone *ht, int x, int y, int w)
{
	/* FIXME: There is a potential optimisation here; in the case where
	 * the LCM of the halftone tile widths is smaller than w, we could
	 * form just one 'LCM' run, then copy it repeatedly.
	 */
	int k, n;
	n = ht->n;
	for (k = 0; k < n; k++)
	{
		fz_pixmap *tile = ht->comp[k];
		unsigned char *b = buf++;
		unsigned char *t;
		unsigned char *tbase;
		int px = x + tile->x;
		int py = y + tile->y;
		int tw = tile->w;
		int th = tile->h;
		int w2 = w;
		int len;
		px = px % tw;
		if (px < 0)
			px += tw;
		py = py % th;
		if (py < 0)
			py += th;

		assert(tile->n == 1);

		/* Left hand section; from x to tile width */
		tbase = tile->samples + (unsigned int)(py * tw);
		t = tbase + px;
		len = tw - px;
		if (len > w2)
			len = w2;
		w2 -= len;
		while (len--)
		{
			*b = *t++;
			b += n;
		}

		/* Centre section - complete copies */
		w2 -= tw;
		while (w2 >= 0)
		{
			len = tw;
			t = tbase;
			while (len--)
			{
				*b = *t++;
				b += n;
			}
			w2 -= tw;
		}
		w2 += tw;

		/* Right hand section - stragglers */
		t = tbase;
		while (w2--)
		{
			*b = *t++;
			b += n;
		}
	}
}

/* Inner mono thresholding code */
typedef void (threshold_fn)(unsigned char *ht_line, unsigned char *pixmap, unsigned char *out, int w);

static void do_threshold_1(unsigned char *ht_line, unsigned char *pixmap, unsigned char *out, int w)
{
	int bit = 0x80;
	int h = 0;

	do
	{
		if (*pixmap < *ht_line++)
			h |= bit;
		pixmap += 2; /* Skip the alpha */
		bit >>= 1;
		if (bit == 0)
		{
			*out++ = h;
			h = 0;
			bit = 0x80;
		}

	}
	while (--w);
	if (bit != 0x80)
		*out = h;
}

/*
	Note that the tests in do_threshold_4 are inverted compared to those
	in do_threshold_1. This is to allow for the fact that the CMYK
	contone renderings have white = 0, whereas rgb, and greyscale have
	white = 0xFF. Reversing these tests enables us to maintain that
	BlackIs1 in bitmaps.
*/
static void do_threshold_4(unsigned char *ht_line, unsigned char *pixmap, unsigned char *out, int w)
{
	w--;
	while (w > 0)
	{
		int h = 0;
		if (pixmap[0] >= ht_line[0])
			h |= 0x80;
		if (pixmap[1] >= ht_line[1])
			h |= 0x40;
		if (pixmap[2] >= ht_line[2])
			h |= 0x20;
		if (pixmap[3] >= ht_line[3])
			h |= 0x10;
		if (pixmap[5] >= ht_line[4])
			h |= 0x08;
		if (pixmap[6] >= ht_line[5])
			h |= 0x04;
		if (pixmap[7] >= ht_line[6])
			h |= 0x02;
		if (pixmap[8] >= ht_line[7])
			h |= 0x01;
		*out++ = h;
		pixmap += 10;
		ht_line += 8;
		w -= 2;
	}
	if (w == 0)
	{
		int h = 0;
		if (pixmap[0] >= ht_line[0])
			h |= 0x80;
		if (pixmap[1] >= ht_line[1])
			h |= 0x40;
		if (pixmap[2] >= ht_line[2])
			h |= 0x20;
		if (pixmap[3] >= ht_line[3])
			h |= 0x10;
		*out = h;
	}
}

fz_bitmap *fz_new_bitmap_from_pixmap(fz_context *ctx, fz_pixmap *pix, fz_halftone *ht)
{
	return fz_new_bitmap_from_pixmap_band(ctx, pix, ht, 0, 0);
}

fz_bitmap *fz_new_bitmap_from_pixmap_band(fz_context *ctx, fz_pixmap *pix, fz_halftone *ht, int band, int bandheight)
{
	fz_bitmap *out = NULL;
	unsigned char *ht_line = NULL;
	unsigned char *o, *p;
	int w, h, x, y, n, pstride, ostride;
	fz_halftone *ht_orig = ht;
	threshold_fn *thresh;

	if (!pix)
		return NULL;

	fz_var(ht_line);
	fz_var(out);

	band *= bandheight;
	n = pix->n-1; /* Remove alpha */

	switch(n)
	{
	case 1:
		thresh = &do_threshold_1;
		break;
	case 4:
		thresh = &do_threshold_4;
		break;
	default:
		assert(!"Unsupported number of components");
		return NULL;
	}

	if (ht == NULL)
	{
		ht = fz_default_halftone(ctx, n);
	}
	fz_try(ctx)
	{
		ht_line = fz_malloc(ctx, pix->w * n);
		out = fz_new_bitmap(ctx, pix->w, pix->h, n, pix->xres, pix->yres);
		o = out->samples;
		p = pix->samples;

		h = pix->h;
		x = pix->x;
		y = pix->y + band;
		w = pix->w;
		ostride = out->stride;
		pstride = pix->w * pix->n;
		while (h--)
		{
			make_ht_line(ht_line, ht, x, y++, w);
			thresh(ht_line, p, o, w);
			o += ostride;
			p += pstride;
		}
	}
	fz_always(ctx)
	{
		if (!ht_orig)
			fz_drop_halftone(ctx, ht);
		fz_free(ctx, ht_line);
	}
	fz_catch(ctx)
		fz_rethrow(ctx);

	return out;
}
