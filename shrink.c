
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "rgb_image.h"

//This allocates memory for a RgbImage.
//Return the allocated pointer
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

//This shrinks a image of size starting from x, y to scale size image, downsp tells
// if use downscaling or downsampling. Return the shrinked image.
// mode
// 1: downscaling
// 2: downsampling
// 3: maxpooling
// 4: minpooling
// 3, 4 wait to be implemented
RgbImage* shrink(RgbImage* pxls, int x, int y, int width, int height, int scaleWidth, int scaleHeight, int mode) {
	RgbImage* result = allocate(scaleWidth, scaleHeight);

	if(result == NULL) {
		return NULL;
	}

	int scale_factor = width / scaleWidth;
	int i, j;

	if (mode == 1) {
		int k, l;
		for (i = 0; i < scaleHeight; i++) {
			for (j = 0; j < scaleWidth; j++) {
				RgbPixel temp = {0.0, 0.0, 0.0};
				for (k = 0; k < scale_factor; k++) {
					for (l = 0; l < scale_factor; l++) {
						int fixedx = i * scale_factor + x + k;
						int fixedy = j * scale_factor + y + l;

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
	} else if (mode == 2) {
		for (i = 0; i < scaleHeight; i++) {
			for (j = 0; j < scaleWidth; j++) {
				int fixedx = i * scale_factor + x;
				int fixedy = j * scale_factor + y;

				result->pixels[i][j] = pxls->pixels[fixedx][fixedy];
			}
		}
	} 


	return result;
}

