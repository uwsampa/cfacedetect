
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "floatfann.h"
#include "rgb_image.h"
#include "parse.h"
#include "shrink.h"
#include "face.h"

#define APPROX false //turns on fann

#define DOWNSP false //downsample or downscale

// FIXME: Have different modes
// #define MODE
// 0: no downscaling/downsampling
// 1: downscaling
// 2: downsampling
// 3: maxpooling
// 4: minpooling

// File paths
#define INPIC "Images/single.rgb" //picture file
#define NN "vision_3L_36N.net" //fann file

// Viola Jones Algorithm parameters
#define DEFSIZE 20 //defined window size
#define SCALE_FACTOR 1.25 //window scaling
#define Y_STEP_SIZE 1 //window step size along Y step
#define X_STEP_SIZE 1 //window step size along X step


Face* head = NULL;
int count = 0;


//Return an integral image from pxls, isSquared determines if it's squared
RgbImage* integralImage(RgbImage* pxls, int isSquared) {

    //Initialize and allocate for RgbImage
    RgbImage* result = allocate(pxls->w, pxls->h);

    if(result == NULL) {
        return NULL;
    }

    //Integral Image
    int x, y;

    for (y = 0; y < pxls->h; y++) {
        for (x = 0; x < pxls->w; x++) {
            float l = pxls->pixels[y][x].r;
            if (isSquared) {
                l = l * l;
            }
            if (x == 0 && y == 0) {
                result->pixels[y][x].r = l;
                result->pixels[y][x].g = l;
                result->pixels[y][x].b = l;
            } else if (y == 0) {
                result->pixels[y][x].r = l + result->pixels[y][x-1].r;
                result->pixels[y][x].g = l + result->pixels[y][x-1].g;
                result->pixels[y][x].b = l + result->pixels[y][x-1].b;
            } else if (x == 0) {
                result->pixels[y][x].r = l + result->pixels[y-1][x].r;
                result->pixels[y][x].g = l + result->pixels[y-1][x].g;
                result->pixels[y][x].b = l + result->pixels[y-1][x].b;
            } else {
                result->pixels[y][x].r = l + result->pixels[y-1][x].r + result->pixels[y][x-1].r - result->pixels[y-1][x-1].r;
                result->pixels[y][x].g = l + result->pixels[y-1][x].g + result->pixels[y][x-1].g - result->pixels[y-1][x-1].g;
                result->pixels[y][x].b = l + result->pixels[y-1][x].b + result->pixels[y][x-1].b - result->pixels[y-1][x-1].b;
            }

        }
    }

    return result;

}

//Return the mean of integral image area
float getMean(RgbImage* integral, int x, int y, int window, int area) {
    // Works for floats, but if pixels are ints, should be cast to float before division
    return (integral->pixels[y][x].r - integral->pixels[y + window][x].r- integral->pixels[y][x + window].r
        + integral->pixels[y + window][x + window].r) / area;
}

//Return the feature value computed with the picture
float getFeatureVal(RgbImage* integral, Feature feat, float scale, int x, int y) {
    float totalFeatureVal = 0.0;
    int i;
    for (i = 0; i < feat.rectNum; i++) {
        int rectx = (int)(feat.rectList[i].width * scale + 0.5);
        int recty = (int)(feat.rectList[i].height * scale + 0.5);
        int rectw = (int)(feat.rectList[i].x * scale + 0.5);
        int recth = (int)(feat.rectList[i].y * scale + 0.5);

        int w1x = min(x + rectx, integral->w - 1);
        int w1y = min(y + recty, integral->h - 1);
        int w2x = min(x + rectx + rectw, integral->w - 1);
        int w2y = min(y + recty, integral->h - 1);
        int w3x = min(x + rectx , integral->w - 1);
        int w3y = min(y + recty + recth, integral->h - 1);
        int w4x = min(x + rectx + rectw, integral->w - 1);
        int w4y = min(y + recty + recth, integral->h - 1);

        totalFeatureVal += feat.rectList[i].weight * ( integral->pixels[w1y][w1x].r
            - integral->pixels[w2y][w2x].r - integral->pixels[w3y][w3x].r + integral->pixels[w4y][w4x].r);
    }
    return totalFeatureVal;
}

//Merges all the faces, delete them if they overlap by 40%
//Look into this may find out the memory leak
int mergeRectangles() {
    int diff = 0;
    Face* rect1;
    Face* rect2;
    for(rect1 = head; rect1 != NULL; rect1 = rect1->next) {
        rect2 = rect1->next;
        while(rect2 != NULL) {
            printf("(%d, %d, %d) | (%d, %d, %d)\n", rect1->window, rect1->x, rect1->y, rect2->window, rect2->x, rect2->y);
            if (overlap(rect1, rect2, head)) {
                printf("overlap by 40 percent and deleted\n");
                rect2 = delete(rect2, head);
                diff++;
                count--;
            } else {
                rect2 = rect2->next;
            }
        }
    }
    return diff;
}


//Sliding the fixed cascade through the picture to figure out if it's a face.
//If a face is detected, store it at the end of linked list
void detectSingleScale(RgbImage* integral, RgbImage* integralsq, Cascade* classifier, int window, float scale, char* filePath) {
    int width = integral->w;
    int height = integral->h;
    int area = window * window;
    int y, x;
    //slide the window along the y axis by Y_STEP_SIZE
    for (y = 0; y < height - window; y+=Y_STEP_SIZE) {
        //slide the window along the x axis by X_STEP_SIZE
        for (x = 0; x < width - window; x+=X_STEP_SIZE) {

            float mean = getMean(integral, x, y, window, area);
            float variance = getMean(integralsq, x, y, window, area) - (mean * mean);

            float stdev = 1.0;

            if (variance > 0) {
                stdev = sqrt(variance);
            }

            int i, j;
            //for each stage in the classifier
            for (i = 0; i < classifier->stgNum; i++) {
                float stagePassThresh = 0.0;
                //for each classifier in the stage
                for (j = 0; j < classifier->stages[i].nodeNum; j++) {
                    Feature feat = classifier->features[classifier->stages[i].nodeList[j].featind];

                    //sum in rectangle is D - B - C + A
                    float totalFeatureVal = getFeatureVal(integral, feat, scale, x, y);

                    if (totalFeatureVal / area < classifier->stages[i].nodeList[j].threshold * stdev) {
                        stagePassThresh += classifier->stages[i].nodeList[j].weights[0];
                    } else {
                        stagePassThresh += classifier->stages[i].nodeList[j].weights[1];
                    }
                }

                if (stagePassThresh < classifier->stages[i].threshold) {
                    break;
                }

                if ( i + 1 == classifier->stgNum) {
                    head = push(head, window, x, y);
                    count++;
                }
            }
        }
    }
}

//The FANN version of detectSingleScale
//First shrink the image to 20x20, then run it through fann to get the output
void approxDetectSingleScale(struct fann *ann, RgbImage* pxls, int window, char* filePath) {
    int width = pxls->w;
    int height = pxls->h;

    fann_type input[DEFSIZE * DEFSIZE];
    float *calc_out;
    int s;

    int y, x;
    for (y = 0; y < height - window; y++) {
        for (x = 0; x < width - window; x++) {
            RgbImage* result = shrink(pxls, x, y, window, DEFSIZE, DOWNSP);

            if(result == NULL) {
                return;
            }

            int i;

            for(i = 0; i < DEFSIZE * DEFSIZE; i++) {
                input[i] = result->pixels[i / DEFSIZE][i % DEFSIZE].r / 255.0;
            }

            calc_out = fann_run(ann, input);

            s = calc_out[0] > 0.5 ? 1 : 0;

            if(s == 1) {
                head = push(head, window, x, y);
            } else {
                break;
            }

            freeRgbImage(result);
        }
    }
}

//Initiating detectSingleScale with different window sizes
void detectMultiScale(RgbImage* pxls, Cascade* classifier, char* filePath) {
    printf("In detectMultiScale.\n");
    int max_window = min(pxls->w, pxls->h);

    RgbImage* integral;
    RgbImage* integralsq;

    struct fann *ann;


    #if APPROX
        ann = fann_create_from_file(NN);
    #else
        integral = integralImage(pxls, 0);
        integralsq = integralImage(pxls, 1);

        if(integral == NULL || integralsq == NULL) {
            return;
        }

    #endif

    float window = classifier->dim;
    float scale = 1.0;

    while (window < max_window) {
        window = window * SCALE_FACTOR;
        scale = scale * SCALE_FACTOR;
        // TODO: support for downscaling - downscale the whole
        // image and recompute the integral to keep the window
        // constant
        // e.g:
        // 1: shrink down dimensions and image by scaling factor (window stays the same)
        // 2: recompute itegrals
        // 3: detection (pass scale 1, window classifier->dim)
        #if APPROX
            approxDetectSingleScale(ann, pxls, (int)window, filePath);
        #else
            detectSingleScale(integral, integralsq, classifier, (int)window, scale, filePath);
        #endif
    }

    /*

    while (max_window > window) {
        max_window = max_window / SCALE_FACTOR;
        RgbImage* shrinked = shrink(pxls, 0, 0, window, max_window, DOWNSP);
        RgbImage* integral = integralImage(shrinked, 0);
        RgbImage* integralsq = integralImage(shrinked, 1);
        detectSingleScale(integral, integralsq, classifier, (int)window, filePath);

    }
    */

    printf("Finished detecting.\n");

    printf("Detected faces = %d!\n", count);

    printf("Merging.\n");
    int diff = 1;
    
    while (diff > 0) {
        diff = mergeRectangles();
    }

    printf("Start printing.\n");
    printfree(head);
    printf("Total faces = %d!\n", count);

    #if APPROX
        fann_destroy(ann);
        printf("Destroyed ann.\n");
    #else
        freeRgbImage(integral);
        freeRgbImage(integralsq);
        printf("Integral freed.\n");
    #endif
}


int main(int argc, char* argv[]) {
    RgbImage srcImage;

    initRgbImage(&srcImage);

    if(argc > 1) {
        loadRgbImage(argv[1], &srcImage);
    } else {
        loadRgbImage(INPIC, &srcImage);
    }


    grayscale(&srcImage);

    Cascade* cas = loadCascade("xml/ocv_clsfr.xml");

    if(cas != NULL) {
        #if APPROX
            printf("Approximating.\n");
        #endif
        detectMultiScale(&srcImage, cas, "script.txt");
        freeCascade(cas);
    }

    freeRgbImage(&srcImage);
    printf("Image freed.\n");

    return 0;
}