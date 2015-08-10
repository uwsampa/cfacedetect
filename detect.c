#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "face.h"
#include "fann/include/floatfann.h"
#include "parse.h"
#include "rgb_image.h"
#include "shrink.h"

#define VERSION 0
//0: original viola jones algorithm
//1: new thierry's way of implementation

#define ADAPTIVE_STEP false
//false: Adaptive step disabled (ver. 0 only)
//true: Adaptive step enabled (ver. 0 only)

#define APPROX false //turns on fann

#define MERGE true //turns on merging

#define MODE 2
// 1: downscaling
// 2: downsampling
// 3: maxpooling
// 4: minpooling

#define DEBUG false //turns on debugging information

#define DATA false //turns on data collection

// File paths
#define CASCADE "xml/ocv_clsfr.xml" //cascade file
#define INPIC "Images/single.rgb" //picture file
#define NN "vision_3L_36N.net" //fann file

// Viola Jones Algorithm parameters
#define DEFSIZE 20 //defined window size
#define SCALE_FACTOR 1.25 //window scaling
#define Y_STEP_SIZE 1 //window step size along Y step
#define X_STEP_SIZE 1 //window step size along X step


Face* head = NULL;
int count = 0;

#if DATA
	char* filePath;

	void printPix(RgbImage* result, int num) {
		FILE* fp;
		int i, j;

		fp = fopen(filePath, "a");

		for (i = 0; i < DEFSIZE; i++) {
			for (j = 0; j < DEFSIZE; j++) {
				fprintf(fp, "%f ", result->pixels[i][j].r / 255.0);
			}
		}
		fprintf(fp, "\n");
		fprintf(fp, "%d\n", num);
		fclose(fp);
	}
#endif


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
	// Works for floats, but if pixels are ints, should be classifiert to float before division
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
int mergeRectangles(int filter) {
	int diff = 0;
	Face* rect1;
	Face* rect2;
	Face* rect2prev;
	for(rect1 = head; rect1 != NULL; rect1 = rect1->next) {
		rect2 = rect1->next;
		rect2prev = rect1;
		while(rect2 != NULL) {
			#if DEBUG
				printf("(%d, %d, %d) | (%d, %d, %d)\n", rect1->window, rect1->x, rect1->y, rect2->window, rect2->x, rect2->y);
			#endif

			if (overlap(rect1, rect2)) {
				#if DEBUG
					printf("overlap by 40 percent and deleted\n");
				#endif
				rect2 = deleteNext(rect2prev);
				diff++;
				count--;
			} else {
				rect2prev = rect2;
				rect2 = rect2->next;
			}
		}
	}
	return diff;
}


//Sliding the fixed classifiercade through the picture to figure out if it's a face.
//If a face is detected, store it at the end of linked list
void detectSingleScale(RgbImage* pxls, RgbImage* integral, RgbImage* integralsq, Cascade* classifier, int window, float scale) {
	int width = integral->w;
	int height = integral->h;
	int area = window * window;
	int y, x;

	int y_step_size, x_step_size;
	#if VERSION == 0
		#if ADAPTIVE_STEP
			y_step_size = (int) (window*Y_STEP_SIZE*0.05);
			x_step_size = (int) (window*X_STEP_SIZE*0.05);
		#else
			y_step_size = Y_STEP_SIZE;
			x_step_size = X_STEP_SIZE;
		#endif //ADAPTIVE_STEP
	#elif VERSION == 1
		y_step_size = Y_STEP_SIZE;
		x_step_size = X_STEP_SIZE;
	#endif //VERSION

	//slide the window along the y axis by Y_STEP_SIZE
	for (y = 0; y < height - window; y += y_step_size) {
		//slide the window along the x axis by X_STEP_SIZE
		for (x = 0; x < width - window; x += x_step_size) {

			#if DATA
				RgbImage* result = shrink(pxls, x, y, window, window, DEFSIZE, DEFSIZE, MODE);
			#endif

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
					// Feature feat = classifier->features[classifier->stages[i].nodeList[j].featind];
					Feature feat = *(classifier->stages[i].nodeList[j].feat);

					// printf("Get to stage: %d, node: %d\n", i, j);

					//sum in rectangle is D - B - C + A
					float totalFeatureVal;
					#if VERSION == 0
						totalFeatureVal = getFeatureVal(integral, feat, scale, x, y);
					#elif VERSION == 1
						totalFeatureVal = getFeatureVal(integral, feat, 1.0, x, y);
					#endif

					if (totalFeatureVal / area < classifier->stages[i].nodeList[j].threshold * stdev) {
						stagePassThresh += classifier->stages[i].nodeList[j].weights[0];
					} else {
						stagePassThresh += classifier->stages[i].nodeList[j].weights[1];
					}
				}

				if (stagePassThresh < classifier->stages[i].threshold) {
					#if DATA
						int r = rand() % 1000;
						if (r == 999) {
							printPix(result, 0);
						}
					#endif
					break;
				}

				if ( i + 1 == classifier->stgNum) {
					#if DATA
						printPix(result, 1);
					#endif
					#if VERSION == 0
						head = push(head, window, x, y);
					#elif VERSION == 1
						head = push(head, (int)(window * scale), (int)(x * scale), (int)(y * scale));
					#endif
					count++;
				}
			}

			#if DATA
				freeRgbImage(result);
			#endif
		}
	}
}

//The FANN version of detectSingleScale
//First shrink the image to 20x20, then run it through fann to get the output
void approxDetectSingleScale(struct fann *ann, RgbImage* pxls, int window) {
	int width = pxls->w;
	int height = pxls->h;

	fann_type input[DEFSIZE * DEFSIZE];
	float *calc_out;
	int s;

	int y, x;
	for (y = 0; y < height - window; y++) {
		for (x = 0; x < width - window; x++) {
			RgbImage* result = shrink(pxls, x, y, window, window, DEFSIZE, DEFSIZE, MODE);

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
void detectMultiScale(RgbImage* pxls, Cascade* classifier) {
	int max_window = min(pxls->w, pxls->h);
	float window = classifier->dim;

	#if VERSION == 0

		#if APPROX
			struct fann *ann;
			printf("Approximating.\n");
			ann = fann_create_from_file(NN);
		#else
			RgbImage* integral = integralImage(pxls, 0);
			RgbImage* integralsq = integralImage(pxls, 1);
			if(integral == NULL || integralsq == NULL) {
				return;
			}
		#endif

		float scale = 1.0;

		while (window < max_window) {
			window = window * SCALE_FACTOR;
			scale = scale * SCALE_FACTOR;

			#if APPROX
				approxDetectSingleScale(ann, pxls, (int)window);
			#else
				detectSingleScale(pxls, integral, integralsq, classifier, (int)window, scale);
			#endif
		}

		#if APPROX
			fann_destroy(ann);
		#else
			freeRgbImage(integral);
			freeRgbImage(integralsq);
		#endif

	#elif VERSION == 1
		// TODO: support for downscaling - downscale the whole
			// image and recompute the integral to keep the window
			// constant
			// e.g:
			// 1: shrink down dimensions and image by scaling factor (window stays the same)
			// 2: recompute itegrals
			// 3: detection (pass scale 1, window classifier->dim)

		int shrinkWidth = pxls->w;
		int shrinkHeight = pxls->h;
		int shrinkSize = max_window;
		float scale = 1.0;
		while (shrinkSize > window) {
			shrinkWidth /= SCALE_FACTOR;
			shrinkHeight /= SCALE_FACTOR;
			shrinkSize /= SCALE_FACTOR;
			scale *= SCALE_FACTOR;
			RgbImage* shrinked = shrink(pxls, 0, 0, pxls->w, pxls->h, shrinkWidth, shrinkHeight, MODE);
			RgbImage* integral = integralImage(shrinked, 0);
			RgbImage* integralsq = integralImage(shrinked, 1);
			detectSingleScale(pxls, integral, integralsq, classifier, (int)window, scale);
			freeRgbImage(shrinked);
			freeRgbImage(integral);
			freeRgbImage(integralsq);
		}

	#endif

	printf("Detected = %d!\n", count);

	#if MERGE
		printf("Merging.\n");
		int diff = 1;
		while (diff > 0) {
			diff = mergeRectangles(max_window);
		}
	#endif

	printfree(head);
	printf("Total faces = %d!\n", count);
}

void detect(Cascade* classifier, char* filename) {
	RgbImage srcImage;

	initRgbImage(&srcImage);

	loadRgbImage(filename, &srcImage);

	grayscale(&srcImage);

	printf("Detecting.\n");

	detectMultiScale(&srcImage, classifier);

	freeRgbImage(&srcImage);
}

int main(int argc, char **argv) {
	Cascade* classifier = loadCascade(CASCADE);

	if (classifier != NULL) {

		#if DATA
			if (argc != 3) {
				printf("Error: wrong arguments for collecting data.\n");
				printf("Usage: %s FILENAME OUTPUT_FILE\n", argv[0]);
			} else {
				srand(time(NULL));
				filePath = argv[2];
				detect(classifier, argv[1]);
			}
		#else
			if (argc != 2) {
				detect(classifier, INPIC);
			} else {
				detect(classifier, argv[1]);
			}
		#endif
		freeCascade(classifier);
	}
	return 0;
	
}