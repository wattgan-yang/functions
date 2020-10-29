#include <stdio.h>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct
{
	unsigned short int type;
	unsigned int size;
	unsigned short int reserved1, reserved2;
	unsigned int offset;
} FILEHEADER;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
	unsigned int size;			   /* Info Header size in bytes */
	int width, height;			   /* Width and height of image */
	unsigned short int planes;	   /* Number of colour planes */
	unsigned short int bits;	   /* Bits per pixel */
	unsigned int compression;	   /* Compression type */
	unsigned int imagesize;		   /* Image size in bytes */
	int xresolution, yresolution;  /* Pixels per meter */
	unsigned int ncolours;		   /* Number of colours */
	unsigned int importantcolours; /* Important colours */
} INFOHEADER;
#pragma pack(pop)

unsigned char *bmp_to_rgb565_auto_malloc(const char *file_path, int *width, int *height)
{
	FILEHEADER header1;
	INFOHEADER header2;

	unsigned short *rgb_buf = NULL;

	FILE *bmpf = fopen(file_path, "rb");

	if (!bmpf)
	{
		printf("file read failed\n");
		return NULL;
	}

	fread(&header1, 1, sizeof(header1), bmpf);
	fread(&header2, 1, sizeof(header2), bmpf);

	*width = header2.width;
	*height = header2.height;

	int bmp_buf_size = header2.width * header2.height * (header2.bits / 8);
	unsigned char *bmp_buf = (unsigned char *)malloc(bmp_buf_size);

	int read_size = fread(bmp_buf, 1, bmp_buf_size, bmpf);

	fclose(bmpf);

	if (header2.bits == 24)
	{
		rgb_buf = (unsigned short *)malloc(header2.width * header2.height * 2);
		int rgb565_pos = 0;

		for (int row = header2.height - 1; row >= 0; row--)
		{
			int bmp_pos = header2.width * row * 3;
			for (int col = 0; col < header2.width; col++)
			{
				unsigned char blue = bmp_buf[bmp_pos];
				unsigned char green = bmp_buf[bmp_pos + 1];
				unsigned char red = bmp_buf[bmp_pos + 2];

				rgb_buf[rgb565_pos] = ((red & 0b11111000) << 8) | ((green & 0b11111100) << 3) | (blue >> 3);

				bmp_pos += 3;
				rgb565_pos++;
			}
		}
	}
	else
	{
		printf("support only 24 bit bmp\n");
	}

	free(bmp_buf);

	return (unsigned char *)rgb_buf;
}

int main(int argc, char *argv[])
{

	int width, height;

	unsigned char *rgbBuf = bmp_to_rgb565_auto_malloc("x.bmp", &width, &height);

	if(!rgbBuf)
		return -1;

	printf("image res: %d x %d\n", width, height);

	FILE *rgbf = fopen("xxx.rgb", "wb");

	fwrite(rgbBuf, 1, width * height * 2, rgbf);
	fclose(rgbf);
	free(rgbBuf);

	return 0;
}