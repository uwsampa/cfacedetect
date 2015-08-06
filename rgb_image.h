/*
 * rgb_image.h
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#ifndef RGB_IMAGE_H_
#define RGB_IMAGE_H_

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

typedef struct {
   float r;
   float g;
   float b;
} RgbPixel;

typedef struct {
   int w;
   int h;
   RgbPixel** pixels;
   char* meta;
} RgbImage;

void initRgbImage(RgbImage* image);

int loadRgbImage(const char* fileName, RgbImage* image);

int saveRgbImage(RgbImage* image, const char* fileName, float grayscale);

void freeRgbImage(RgbImage* image);

void makeGrayscale(RgbImage* rgbImage);

void grayscale(RgbImage* image);

#endif /* RGB_IMAGE_H_ */
