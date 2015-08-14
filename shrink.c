#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "rgb_image.h"

/** Allocates memory for a RgbImage.
  * @param[in] width the width of RgbImage
  * @param[in] height the height of RgbImage
  * @return the allocated pointer
  */
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

/** This shrinks a image of size starting from x, y with width and height to scale size image.
  * @param[in] pxls the image to be shrinked
  * @param[in] x the x coordinate
  * @param[in] y the y coordinate
  * @param[in] width the selected area's width
  * @param[in] height the selected area's height
  * @param[in] scaleWidth width after shrinking
  * @param[in] scaleHeight height after shrinking
  * @param[in] mode mode to perform shrinking[1: downscaling 2: downsampling 3: maxpooling 4: minpooling]
  * @return the shrinked image. 
  */
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
	} else if (mode == 3) {
		int k, l;
		for (i = 0; i < scaleHeight; i++) {
			for (j = 0; j < scaleWidth; j++) {
				RgbPixel temp = {0.0, 0.0, 0.0};
				for (k = 0; k < scale_factor; k++) {
					for (l = 0; l < scale_factor; l++) {
						int fixedx = i * scale_factor + x + k;
						int fixedy = j * scale_factor + y + l;

						temp.r = max(temp.r, pxls->pixels[fixedx][fixedy].r);
						temp.g = max(temp.g, pxls->pixels[fixedx][fixedy].g);
						temp.b = max(temp.b, pxls->pixels[fixedx][fixedy].b);
					}
				}
				result->pixels[i][j].r = temp.r;
				result->pixels[i][j].g = temp.g;
				result->pixels[i][j].b = temp.b;
			}
		}
	} else if (mode == 4) {
		int k, l;
		for (i = 0; i < scaleHeight; i++) {
			for (j = 0; j < scaleWidth; j++) {
				RgbPixel temp = {0.0, 0.0, 0.0};
				for (k = 0; k < scale_factor; k++) {
					for (l = 0; l < scale_factor; l++) {
						int fixedx = i * scale_factor + x + k;
						int fixedy = j * scale_factor + y + l;

						temp.r = (temp.r == 0.0) ? pxls->pixels[fixedx][fixedy].r : min(temp.r, pxls->pixels[fixedx][fixedy].r);
						temp.g = (temp.g == 0.0) ? pxls->pixels[fixedx][fixedy].g : min(temp.g, pxls->pixels[fixedx][fixedy].g);
						temp.b = (temp.b == 0.0) ? pxls->pixels[fixedx][fixedy].b : min(temp.b, pxls->pixels[fixedx][fixedy].b);
					}
				}
				result->pixels[i][j].r = temp.r;
				result->pixels[i][j].g = temp.g;
				result->pixels[i][j].b = temp.b;
			}
		}
	}
	return result;
}

