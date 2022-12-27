/*
* show.c
* show text using libschrift
* https://github.com/lhf/libschrift-show
* This code is hereby placed in the public domain and also under the MIT license.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schrift.h"

#define max(a,b) ((a) > (b) ? a : b)

static const char *progname;

static void fatal(const char *message)
{
	fprintf(stderr, "%s: %s\n", progname, message);
	exit(1);
}

static void loadfont(SFT *sft, const char *filename, double size)
{
	SFT_Font *font = sft_loadfile(filename);
	if (font == NULL) fatal("sft_loadfile failed");
	sft->font = font;
	sft->xScale = size;
	sft->yScale = size;
	sft->xOffset = 0.0;
	sft->yOffset = 0.0;
	sft->flags = SFT_DOWNWARD_Y;
}

static void loadglyph(const SFT *sft, SFT_UChar codepoint, SFT_Glyph *glyph, SFT_GMetrics *metrics)
{
	if (sft_lookup(sft, codepoint, glyph) < 0) fatal("sft_lookup failed");
	if (sft_gmetrics(sft, *glyph, metrics)< 0) fatal("sft_gmetrics failed");
}

static void newimage(SFT_Image *image, int width, int height, int color)
{
	size_t size = (size_t) (width*height);
	void *pixels  = malloc(size);
	image->pixels = pixels;
	image->width  = width;
	image->height = height;
	memset(pixels,color,size);
}

static void saveimage(SFT_Image *image, FILE *f)
{
	size_t size = (size_t) (image->width*image->height);
	printf("P5\n%d %d\n255\n",image->width,image->height);
	fwrite(image->pixels,size,1,f);
}

static void copyimage(SFT_Image *dest, const SFT_Image *source, int x0, int y0, int color)
{
	unsigned char *d=dest->pixels;
	unsigned char *s=source->pixels;
	int y;
	//for (y=0; y<dest->height; y++) d[x0+y*dest->width]=150;
	d+=x0+y0*dest->width;
	for (y=0; y<source->height; y++)
	{
		int x;
		for (x=0; x<source->width; x++)
		{
			double t=s[x]/255.0;
			d[x]=(1.0-t)*d[x]+t*color;
		//	d[x]=s[x];
		//	if (s[x]!=0) d[x]=s[x];
		}
		d+=dest->width;
		s+=source->width;
	}
}

int main(int argc, char *argv[])
{
	const char *filename;
	double size;
	const char *message;
	SFT sft;
	SFT_Image canvas;

	progname = argv[0];
	if (argc!=4) fatal("fontfile size_in_px string");
	filename = argv[1];
	size = atof(argv[2]);
	message = argv[3];

	loadfont(&sft, filename, size);

	size_t k;
	int margin = 100;
	int width  = 0;
	int height = 0;
	int aheight = 0;
	int bheight = 0;
	for (k=0; k<strlen(message); k++)
	{
		SFT_UChar cp = (SFT_UChar) message[k];
		SFT_Glyph gid;
		SFT_GMetrics mtx;
		loadglyph(&sft, cp, &gid, &mtx);
		width  += max(mtx.advanceWidth,mtx.minWidth);
		aheight = max(aheight,-mtx.yOffset);
		bheight = max(bheight, mtx.yOffset+mtx.minHeight);
		if (k==0 && mtx.leftSideBearing<0 && margin<-mtx.leftSideBearing) margin=mtx.leftSideBearing;
	}
	height = aheight+bheight;
	width += 2*margin;
	height+= 2*margin;
	newimage(&canvas,width,height,255);

	double x = margin;
	int y = margin;
	int baseline = aheight;
	y += baseline;
	SFT_Glyph ogid=0;
	for (k=0; k<strlen(message); k++)
	{
		SFT_Image image;
		SFT_UChar cp = (SFT_UChar) message[k];
		SFT_Glyph gid;
		SFT_GMetrics mtx;
		SFT_Kerning kerning;
		loadglyph(&sft, cp, &gid, &mtx);
		newimage(&image, mtx.minWidth, mtx.minHeight,0);
		sft_render(&sft, gid, image);
		sft_kerning (&sft, ogid, gid, &kerning);
		x += kerning.xShift;
		copyimage(&canvas,&image,x+mtx.leftSideBearing,y+mtx.yOffset,0);
		x += mtx.advanceWidth;
		free(image.pixels);
		ogid=gid;
	}
	//memset((char*)canvas.pixels+y*canvas.width, 128, canvas.width);
	saveimage(&canvas,stdout);

	free(canvas.pixels);
	sft_freefont(sft.font);
	return 0;
}
