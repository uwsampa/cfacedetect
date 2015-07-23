
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "rgb_image.h"


RgbImage* allocate(int width, int height) {
	int i, c = 0;

	//Initialize and allocate for RgbImage
	RgbImage* result = (RgbImage *)malloc(sizeof(RgbImage));
	result->w = width;
	result->h = height;
	result->meta = NULL;

	result->pixels = (RgbPixel**)malloc(height * sizeof(RgbPixel*));
	if (result->pixels == NULL) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");
        return NULL;
    }
	for(i = 0; i < height; i++) {
        result->pixels[i] = (RgbPixel*)malloc(width * sizeof(RgbPixel));
        if (result->pixels[i] == NULL) {
            c = 1;
            break;
        }
    }
	if (c == 1) {
		printf("Warning: Oops! Cannot allocate memory for the pixels!\n");
		for (i--; i >= 0; i--) {
	    	free(result->pixels[i]);
	    }
		free(result->pixels);
		free(result);
		return NULL;
	}
	return result;
}

RgbImage* shrink(RgbImage* pxls, int x, int y, int size, int scale, bool downsp) {
	RgbImage* result = allocate(scale, scale);

	if(result == NULL) {
		return NULL;
	}

	int scale_factor = size / scale;
	int i, j;
	if (downsp) {
		for (i = 0; i < scale; i++) {
			for (j = 0; j < scale; j++) {
				int fixedx = i * scale_factor + x;
				int fixedy = j * scale_factor + y;

				if (fixedx >= pxls->w) {
					fixedx = pxls->w - 1;
				}
				if (fixedy >= pxls->h) {
					fixedy = pxls->h - 1;
				}

				result->pixels[i][j] = pxls->pixels[fixedx][fixedy];
			}
		}
	} else {
		int k, l;
		for (i = 0; i < scale; i++) {
			for (j = 0; j < scale; j++) {
				RgbPixel temp = {0.0, 0.0, 0.0};
				for (k = 0; k < scale_factor; k++) {
					for (l = 0; l < scale_factor; l++) {
						int fixedx = i * scale_factor + x + k;
						int fixedy = j * scale_factor + y + l;

						if (fixedx >= pxls->w) {
							fixedx = pxls->w - 1;
						}
						if (fixedy >= pxls->h) {
							fixedy = pxls->h - 1;
						}

						temp.r += pxls->pixels[fixedx][fixedy].r;
						temp.g += pxls->pixels[fixedx][fixedy].g;
						temp.b += pxls->pixels[fixedx][fixedy].b;
					}
				}

				result->pixels[i][j].r = temp.r / (scale_factor * scale_factor);
				result->pixels[i][j].g = temp.g / (scale_factor * scale_factor);
				result->pixels[i][j].b = temp.b / (scale_factor * scale_factor);
			}
		}

	}

	return result;
}

