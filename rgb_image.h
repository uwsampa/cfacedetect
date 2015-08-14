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

/// Struct storing rgb pixel value
typedef struct {
	/// r value
	float r;
	/// g value
	float g;
	/// b value
	float b;
} RgbPixel;

/// Struct storing rgb image
typedef struct {
	/// width
	int w;
	/// height
	int h;
	/// array of arrays of rgb pixels
	RgbPixel** pixels;
	/// meta tag
	char* meta;
} RgbImage;

void initRgbImage(RgbImage* image);

int loadRgbImage(const char* fileName, RgbImage* image);

int saveRgbImage(RgbImage* image, const char* fileName, float grayscale);

void freeRgbImage(RgbImage* image);

void makeGrayscale(RgbImage* rgbImage);

void grayscale(RgbImage* image);

#endif /* RGB_IMAGE_H_ */
