/*
 * rgb_image.c
 *
 *  Created on: May 1, 2012
 *      Author: Hadi Esmaeilzadeh <hadianeh@cs.washington.edu>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "rgb_image.h"

/** Initialize rgb image
  * @param image the image to be initialized
  */
void initRgbImage(RgbImage* image) {
    image->w = 0;
    image->h = 0;
    image->pixels = NULL;
    image->meta = NULL;
}

/** Process the rgb file
  * @param fp file reading from
  * @param w char buffer
  * @return the pixel value
  */
int readCell(FILE *fp, char* w) {
    int c;
    int i;

    w[0] = 0;
    for (c = fgetc(fp), i = 0; c != EOF; c = fgetc(fp)) {
        if (c == ' ' || c == '\t') {
            if (w[0] != '\"')
                continue;
        }

        if (c == ',' || c == '\n') {
            if (w[0] != '\"')
                break;
            else if (c == '\"') {
                w[i] = c;
                i++;
                break;
            }
        }

        w[i] = c;
        i++;
    }
    w[i] = 0;

    return c;
}

/** Load rgb image from fileName and store in image
  * @param fileName name of image file
  * @param image place to store the image file
  * @return 1 if load success, 0 otherwise
  */
int loadRgbImage(const char* fileName, RgbImage* image) {
    int c;
    int i;
    int j;
    char w[256];
    RgbPixel** pixels;
    FILE *fp;

    printf("Loading %s ...\n", fileName);

    fp = fopen(fileName, "r");
    if (!fp) {
        printf("Warning: Oops! Cannot open %s!\n", fileName);
        return 0;
    }

    c = readCell(fp, w);
    image->w = atoi(w);
    c = readCell(fp, w);
    image->h = atoi(w);

    printf("%d x %d\n", image->w, image->h);

    pixels = (RgbPixel**)malloc(image->h * sizeof(RgbPixel*));

    if (pixels == NULL) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        fclose(fp);

        return 0;
    }

    c = 0;
    for(i = 0; i < image->h; i++) {
        pixels[i] = (RgbPixel*)malloc(image->w * sizeof(RgbPixel));
        if (pixels[i] == NULL) {
            c = 1;
            break;
        }
    }

    if (c == 1) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        for (i--; i >= 0; i--)
            free(pixels[i]);
        free(pixels);

        fclose(fp);

        return 0;
    }

    for(i = 0; i < image->h; i++) {
        for(j = 0; j < image->w; j++) {
            c = readCell(fp, w);
            pixels[i][j].r = atoi(w);

            c = readCell(fp, w);
            pixels[i][j].g = atoi(w);

            c = readCell(fp, w);
            pixels[i][j].b = atoi(w);
        }
    }
    image->pixels = pixels;

    c = readCell(fp, w);
    image->meta = (char*)malloc(strlen(w) * sizeof(char));
    if(image->meta == NULL) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");

        for (i = 0; i < image->h; i++)
            free(pixels[i]);
        free(pixels);

        fclose(fp);

        return 0;

    }
    strcpy(image->meta, w);

    printf("%s\n", image->meta);

    return 1;
}

/** Allocates memory for a RgbImage.
  * @param[in] width the width of RgbImage
  * @param[in] height the height of RgbImage
  * @return the allocated pointer
  */
RgbImage* allocRgbImage(int width, int height) {
    // int i, c = 0;
    int i, j;

    //Initialize and allocate for RgbImage
    RgbImage* result = (RgbImage *)malloc(sizeof(RgbImage));
    result->w = width;
    result->h = height;
    result->meta = NULL;

    result->pixels = (RgbPixel**)malloc(height * sizeof(RgbPixel*));
    assert (result->pixels && "Cannot allocate memory for the pixels!");

    for(i = 0; i < height; i++) {
        result->pixels[i] = (RgbPixel*)malloc(width * sizeof(RgbPixel));
        assert (result->pixels[i] && "Cannot allocate memory for the pixels!");
        for(j = 0; j < width; j++) {
            result->pixels[i][j].r = 0;
            result->pixels[i][j].g = 0;
            result->pixels[i][j].b = 0;
        }
    }
    return result;
}

/** Save and output the rgb image to fileName
  * @param image rgb image to be saved
  * @param filename filename to be stored in
  * @param scale scale to adjust the picture pixel value
  * @return 1 if success, 0 otherwise
  */
int saveRgbImage(RgbImage* image, const char* fileName, float scale) {
    int i;
    int j;
    FILE *fp;

    printf("Saving %s ...\n", fileName);

    fp = fopen(fileName, "w");
    if (!fp) {
        printf("Warning: Oops! Cannot open %s!\n", fileName);
        return 0;
    }

    fprintf(fp, "%d,%d\n", image->w, image->h);
    printf("%d,%d\n", image->w, image->h);

    for(i = 0; i < image->h; i++) {
        for(j = 0; j < (image->w - 1); j++) {
            fprintf(fp, "%d,%d,%d,", (int)(image->pixels[i][j].r * scale), (int)(image->pixels[i][j].g * scale), (int)(image->pixels[i][j].b * scale));
        }
        fprintf(fp, "%d,%d,%d\n", (int)(image->pixels[i][j].r * scale), (int)(image->pixels[i][j].g * scale), (int)(image->pixels[i][j].b * scale));
    }

    fprintf(fp, "%s", image->meta);
    printf("%s\n", image->meta);

    fclose(fp);

    return 1;
}

/** Free rgb image from memory
  * @param image image to be freed
  */
void freeRgbImage(RgbImage* image) {
    int i;

    if (image->meta != NULL)
        free(image->meta);

    if (image->pixels == NULL)
        return;

    for (i = 0; i < image->h; i++)
        if (image->pixels != NULL)
            free(image->pixels[i]);

    free(image->pixels);
}

void makeGrayscale(RgbImage* image) {
    int i;
    int j;
    float luminance;

    float rC = 0.30 / 256;
    float gC = 0.59 / 256;
    float bC = 0.11 / 256;

    for(i = 0; i < image->h; i++) {
        for(j = 0; j < image->w; j++) {
            luminance =
                rC * image->pixels[i][j].r +
                gC * image->pixels[i][j].g +
                bC * image->pixels[i][j].b;

            image->pixels[i][j].r = luminance;
            image->pixels[i][j].g = luminance;
            image->pixels[i][j].b = luminance;
        }
    }
}

/** Alter the image to grayscale
  * @param image rgb image to be grayscaled
  */
void grayscale(RgbImage* image) {
    int i;
    int j;
    float luminance;

    for(i = 0; i < image->h; i++) {
        for(j = 0; j < image->w; j++) {
            luminance =
                0.299 * image->pixels[i][j].r +
                0.587 * image->pixels[i][j].g +
                0.114 * image->pixels[i][j].b;

            image->pixels[i][j].r = floor(luminance);
            image->pixels[i][j].g = floor(luminance);
            image->pixels[i][j].b = floor(luminance);
        }
    }
}

/** Check the integrity of the image
  * @param image rgb image to verify
  */
void checkIntegrity(RgbImage * image) {
    // Now let's verify the integrity
    int i, j;
    float checksum = 0;
    assert(image);
    assert(image->pixels);
    for (i=0; i<image->h; i++) {
        assert(image->pixels[i]);
        for (j=0; j<image->w; j++) {
            checksum += image->pixels[i][j].r;
            checksum += image->pixels[i][j].g;
            checksum += image->pixels[i][j].b;
        }
    }
    assert (checksum>=0);
}
