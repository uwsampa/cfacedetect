#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "rgb_image.h"
#include "shrink.h"

/** This shrinks a image of size starting from x, y with width and height to scale size image.
  * @param[in] pxls the image to be shrinked
  * @param[in] x the x coordinate
  * @param[in] y the y coordinate
  * @param[in] width the selected area's width
  * @param[in] height the selected area's height
  * @param[in] scaleWidth width after shrinking
  * @param[in] scaleHeight height after shrinking
  * @return the shrinked image.
  */
RgbImage* shrink(RgbImage* pxls, int x, int y, int width, int height, int scaleWidth, int scaleHeight) {
    RgbImage* result = allocRgbImage(scaleWidth, scaleHeight);

    assert(result && "Allocation of image buffer seems to have failed...");

    float scale_factor = (float) width / scaleWidth;
    int i, j;

    #if SHRINK_MODE == 1 // downsampling
        for (i = 0; i < scaleHeight; i++) {
            for (j = 0; j < scaleWidth; j++) {
                int fixedy = (int) ((float) i * scale_factor) + y;
                int fixedx = (int) ((float) j * scale_factor) + x;

                result->pixels[i][j] = pxls->pixels[fixedy][fixedx];;
            }
        }
    #elif (SHRINK_MODE == 2 || SHRINK_MODE == 3 || SHRINK_MODE == 4)
        int k, l;
        for (i = 0; i < scaleHeight; i++) {
            for (j = 0; j < scaleWidth; j++) {
                #if SHRINK_MODE == 4 //min
                    RgbPixel temp = {255.0, 255.0, 255.0};
                #else
                    RgbPixel temp = {0.0, 0.0, 0.0};
                #endif //SHRINK_MODE

                int fixedy = (int) ((float) i * scale_factor) + y;
                int fixedx = (int) ((float) j * scale_factor) + x;
                int nxt_fixedy = (int) ((float) (i+1) * scale_factor) + y;
                int nxt_fixedx = (int) ((float) (j+1) * scale_factor) + x;

                for (k = fixedy; k < nxt_fixedy; k++) {
                    for (l = fixedx; l < nxt_fixedx; l++) {
                        #if SHRINK_MODE == 2 // average
                            temp.r += pxls->pixels[k][l].r;
                            temp.g += pxls->pixels[k][l].g;
                            temp.b += pxls->pixels[k][l].b;
                        #elif SHRINK_MODE == 3 // max
                            temp.r = max(temp.r, pxls->pixels[k][l].r);
                            temp.g = max(temp.g, pxls->pixels[k][l].g);
                            temp.b = max(temp.b, pxls->pixels[k][l].b);
                        #elif SHRINK_MODE == 4 // min
                            temp.r = min(temp.r, pxls->pixels[k][l].r);
                            temp.g = min(temp.g, pxls->pixels[k][l].g);
                            temp.b = min(temp.b, pxls->pixels[k][l].b);
                        #endif //SHRINK_MODE
                    }
                }

                #if SHRINK_MODE == 2 // average
                    result->pixels[i][j].r = temp.r / (scale_factor * scale_factor);
                    result->pixels[i][j].g = temp.g / (scale_factor * scale_factor);
                    result->pixels[i][j].b = temp.b / (scale_factor * scale_factor);
                #else
                    result->pixels[i][j].r = temp.r;
                    result->pixels[i][j].g = temp.g;
                    result->pixels[i][j].b = temp.b;
                #endif
            }
        }
    #endif //SHRINK_MODE
    return result;
}

