#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "face.h"
#include "floatfann.h"
#include "parse.h"
#include "rgb_image.h"
#include "shrink.h"

/** Version selection
  * 0: original viola jones algorithm
  * 1: new thierry's way of implementation
  */
#define VERSION 0

/** Turns on adaptive step for the algorithm
  * false: Adaptive step disabled (ver. 0 only)
  * true: Adaptive step enabled (ver. 0 only)
  */
#define ADAPTIVE_STEP false

/// Turns on merging when collecting results
#define MERGE false

///Turns on debugging information
#define DEBUG false

/// Turns on data collection
/// 0: no data collection
/// 1: window collection, all
/// 2: window collection, pos/neg
/// 3: face collection
#define DATA_COLLECT_MODE 3

// Inverse probability of recording a
// negative sample (balances-out the
// negative vs. positive count)
#define DRAW_PROB 1

/// Default data output path
#define DATA_FN "train"

/// Defined ANN window size
#define DEFSIZE 20

/// Classifier Window
#define CLASSDIM 20

/// Window scaling factor
#define SCALE_FACTOR 1.25

/// Window step size along Y
#define Y_STEP_SIZE 1

/// Window step size along X
#define X_STEP_SIZE 1 /// window step size along X step

/// head of list of faces found
Face* head = NULL;

/// number of faces in list
int count = 0;

/// name of the data file name
char * dataFileName = DATA_FN;

/// global invocation count
int invocCount = 0;

/** This function dumps a window of pixels
  * to a specified file for data collection
  * purposes.
  * @param[in][in] result window of pixels
  * @param[in][in] num whether it is a face or not
  */
void printPix(RgbImage* result, FILE* fp, int label) {

	int i, j;

	for (i = 0; i < DEFSIZE; i++) {
		for (j = 0; j < DEFSIZE; j++) {
			fprintf(fp, "%f ", result->pixels[i][j].r / 255.0);
		}
	}
	fprintf(fp, "\n");
	if (label!=-1) {
		fprintf(fp, "%d\n", label);
	}
}


/** This computes an integral image from pxls
  * @param[in][in] pxls pixel values
  * @param[in][in] isSquared flag to determine
  *            if integral image needs to be squared
  * @return pointer to computed integral RgbImage
  */
RgbImage* integralImage(RgbImage* pxls, int isSquared) {

	//Initialize and allocate for RgbImage
	RgbImage* result = allocRgbImage(pxls->w, pxls->h);

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

/** Compute the mean from integral image starting from x, y with window size
  * @param[in] integral integral image
  * @param[in] x x coordinate
  * @param[in] y y coordinate
  * @param[in] window window size
  * @param[in] area total area
  * @return the mean
  */
float getMean(RgbImage* integral, int x, int y, int window, int area) {
	// Works for floats, but if pixels are ints, should be cast to float before division
	return (integral->pixels[y][x].r - integral->pixels[y + window][x].r- integral->pixels[y][x + window].r
		+ integral->pixels[y + window][x + window].r) / area;
}

/** Compute the feature value with integral image. Feature is scaled by scale.
  * @param[in] integral integral image
  * @param[in] node that got computed
  * @param[in] scale scale factore
  * @param[in] x x coordinate
  * @param[in] y y coordinate
  * @return the computed feature value
  */
float getFeatureVal(RgbImage* integral, Node nodeWithRects, float scale, int x, int y) {
	float totalFeatureVal = 0.0;
	int i;
	for (i = 0; i < nodeWithRects.rectNum; i++) {
		int rectx = (int)(nodeWithRects.rectList[i].width * scale + 0.5);
		int recty = (int)(nodeWithRects.rectList[i].height * scale + 0.5);
		int rectw = (int)(nodeWithRects.rectList[i].x * scale + 0.5);
		int recth = (int)(nodeWithRects.rectList[i].y * scale + 0.5);

		int w1x = min(x + rectx, integral->w - 1);
		int w1y = min(y + recty, integral->h - 1);
		int w2x = min(x + rectx + rectw, integral->w - 1);
		int w2y = min(y + recty, integral->h - 1);
		int w3x = min(x + rectx , integral->w - 1);
		int w3y = min(y + recty + recth, integral->h - 1);
		int w4x = min(x + rectx + rectw, integral->w - 1);
		int w4y = min(y + recty + recth, integral->h - 1);

		totalFeatureVal += nodeWithRects.rectList[i].weight * ( integral->pixels[w1y][w1x].r
			- integral->pixels[w2y][w2x].r - integral->pixels[w3y][w3x].r + integral->pixels[w4y][w4x].r);
	}
	return totalFeatureVal;
}

/** Merge faces that overlaps by more than 40%
  * @return number of difference in list of faces
  */
int mergeRectangles() {
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


/** Detect faces from pxls with fixed window size
  * @param[in] pxls input RgbImage
  * @param[in] integral integral RgbImage
  * @param[in] integralsq squared integral RgbImage
  * @param[in] classifier the haar classifier
  * @param[in] window fixed window size
  * @param[in] scale fixed scale
  */
void detectSingleScale(RgbImage* pxls, RgbImage* integral, RgbImage* integralsq, Cascade* classifier, int window, float scale) {
	int width = integral->w;
	int height = integral->h;
	int area = window * window;
	int y, x;

	printf("window=%d width=%d height=%d invocs=%d\n", window, width, height, invocCount);

#if DATA_COLLECT_MODE==1 || DATA_COLLECT_MODE==3
	FILE* fp_all = fopen(dataFileName, "a");
	assert(fp_all && "Could not open data file!");
#elif DATA_COLLECT_MODE==2
	char filePath_pos[256];
	char filePath_neg[256];
	strcpy(filePath_pos, dataFileName);
	strcpy(filePath_neg, dataFileName);
	strcat(filePath_pos, ".pos.data");
	strcat(filePath_neg, ".neg.data");
	FILE* fp_pos = fopen(filePath_pos, "a");
	FILE* fp_neg = fopen(filePath_neg, "a");
	assert(fp_neg && fp_pos && "Could not open data files!");
#endif //DATA_COLLECT_MODE


	int y_step_size, x_step_size;
	#if VERSION == 0
		#if ADAPTIVE_STEP
			y_step_size = (int) (window*(float)Y_STEP_SIZE/DEFSIZE);
			x_step_size = (int) (window*(float)X_STEP_SIZE/DEFSIZE);
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

			#if DATA_COLLECT_MODE==1 || DATA_COLLECT_MODE==2
				RgbImage* result = shrink(pxls, x, y, window, window, DEFSIZE, DEFSIZE);
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
					Node thisNode = classifier->stages[i].nodeList[j];

					//sum in rectangle is D - B - C + A
					float totalFeatureVal;
					#if VERSION == 0
						totalFeatureVal = getFeatureVal(integral, thisNode, scale, x, y);
					#elif VERSION == 1
						totalFeatureVal = getFeatureVal(integral, thisNode, 1.0, x, y);
					#endif

					if (totalFeatureVal / area < classifier->stages[i].nodeList[j].threshold * stdev) {
						stagePassThresh += classifier->stages[i].nodeList[j].weights[0];
					} else {
						stagePassThresh += classifier->stages[i].nodeList[j].weights[1];
					}
				}

				if (stagePassThresh < classifier->stages[i].threshold) {
					#if DATA_COLLECT_MODE==1
						int r = rand() % DRAW_PROB;
						if (DRAW_PROB==1 || r == DRAW_PROB-1) {
							printPix(result, fp_all, 0);
						}
					#elif DATA_COLLECT_MODE==2
						int r = rand() % DRAW_PROB;
						if (DRAW_PROB==1 || r == DRAW_PROB-1) {
							printPix(result, fp_neg, -1);
						}
					#endif //DATA_COLLECT_MODE
					break;
				}

				if ( i + 1 == classifier->stgNum) {
					#if DATA_COLLECT_MODE==1
						printPix(result, fp_all, 1);
					#elif DATA_COLLECT_MODE==2
						printPix(result, fp_pos, -1);
					#endif //DATA_COLLECT_MODE
					#if VERSION == 0
						head = push(head, window, x, y);
					#elif VERSION == 1
						head = push(head, (int)(window * scale), (int)(x * scale), (int)(y * scale));
					#endif
					count++;
				}
			}

			#if DATA_COLLECT_MODE==1 || DATA_COLLECT_MODE==2
				freeRgbImage(result);
			#endif //DATA_COLLECT_MODE

			invocCount ++;
		}
	}

	#if DATA_COLLECT_MODE==1 || DATA_COLLECT_MODE==3
		fclose(fp_all);
	#elif DATA_COLLECT_MODE==2
		fclose(fp_pos);
		fclose(fp_neg);
	#endif //DATA_COLLECT_MODE
}

/** Using FANN approximate detectSingleScale
  * @param[in] ann setted up FANN
  * @param[in] pxls input RgbImage
  * @param[in] window window size scanning the image
  */
void detectSingleScaleApprox(struct fann *ann, RgbImage* pxls, int window) {
	int width = pxls->w;
	int height = pxls->h;

	printf("window=%d width=%d height=%d invocs=%d\n", window, width, height, invocCount);

	fann_type input[DEFSIZE * DEFSIZE];
	float *calc_out;
	int i, j;
	int s;
	int y, x;
	int y_step_size, x_step_size;

#if DATA_COLLECT_MODE==1 || DATA_COLLECT_MODE==3
	FILE* fp_all = fopen(dataFileName, "a");
	assert(fp_all && "Could not open data file!");
#elif DATA_COLLECT_MODE==2
	char filePath_pos[256];
	char filePath_neg[256];
	strcpy(filePath_pos, dataFileName);
	strcpy(filePath_neg, dataFileName);
	strcat(filePath_pos, ".pos.data");
	strcat(filePath_neg, ".neg.data");
	FILE* fp_pos = fopen(filePath_pos, "a");
	FILE* fp_neg = fopen(filePath_neg, "a");
	assert(fp_neg && fp_pos && "Could not open data files!");
#endif //DATA_COLLECT_MODE

#if ADAPTIVE_STEP
	y_step_size = (int) (window*(float)Y_STEP_SIZE/DEFSIZE);
	x_step_size = (int) (window*(float)X_STEP_SIZE/DEFSIZE);
#else
	y_step_size = Y_STEP_SIZE;
	x_step_size = X_STEP_SIZE;
#endif //ADAPTIVE_STEP

	//slide the window along the y axis by Y_STEP_SIZE
	for (y = 0; y < height - window; y += y_step_size) {
		//slide the window along the x axis by X_STEP_SIZE
		for (x = 0; x < width - window; x += x_step_size) {
			RgbImage* result = shrink(pxls, x, y, window, window, DEFSIZE, DEFSIZE);

			if(result == NULL) {
				assert(NULL);
			}

			for (i = 0; i < DEFSIZE; i++) {
				for (j = 0; j < DEFSIZE; j++) {
					input[i*DEFSIZE+j] = result->pixels[i][j].r / 255.0;
				}
			}

			calc_out = fann_run(ann, input);

			s = calc_out[0] > 0.5 ? 1 : 0;

			#if DATA_COLLECT_MODE==1
				printPix(result, fp_all, s);
			#elif DATA_COLLECT_MODE==2
				if (s==1)
					printPix(result, fp_pos, -1);
				else
					printPix(result, fp_neg, -1);
			#endif //DATA_COLLECT_MODE

			if(s == 1) {
				head = push(head, window, x, y);
				count++;
			}

			#if DATA_COLLECT_MODE==1 || DATA_COLLECT_MODE==2
				freeRgbImage(result);
			#endif
			invocCount ++;
		}
	}
}

/** Initiating detectSingleScale with
  * increased window size and scaled features scanning through
  * the image
  * @param[in] pxls input RgbImage
  * @param[in] classifier the haar classifier
  */
void detectMultiScale(RgbImage* pxls, Cascade* classifier) {
	int max_window = min(pxls->w, pxls->h);
	float window = classifier->dim > CLASSDIM ? classifier->dim : CLASSDIM;

	// assert(window==CLASSDIM && "Classifier Window Mismatch!");

	#if VERSION == 0
		RgbImage* integral = integralImage(pxls, 0);
		RgbImage* integralsq = integralImage(pxls, 1);
		if(integral == NULL || integralsq == NULL) {
			return;
		}
		float scale = 1.0;
		while (window < max_window) {
			detectSingleScale(pxls, integral, integralsq, classifier, (int)window, scale);
			window = window * SCALE_FACTOR;
			scale = scale * SCALE_FACTOR;
		}
		freeRgbImage(integral);
		freeRgbImage(integralsq);

	#elif VERSION == 1
		int shrinkWidth = pxls->w;
		int shrinkHeight = pxls->h;
		int shrinkSize = max_window;
		float scale = 1.0;
		while (shrinkSize > window) {
			shrinkWidth /= SCALE_FACTOR;
			shrinkHeight /= SCALE_FACTOR;
			shrinkSize /= SCALE_FACTOR;
			scale *= SCALE_FACTOR;
			RgbImage* shrinked = shrink(pxls, 0, 0, pxls->w, pxls->h, shrinkWidth, shrinkHeight);
			RgbImage* integral = integralImage(shrinked, 0);
			RgbImage* integralsq = integralImage(shrinked, 1);
			detectSingleScale(pxls, integral, integralsq, classifier, (int)window, scale);
			freeRgbImage(shrinked);
			freeRgbImage(integral);
			freeRgbImage(integralsq);
		}
	#endif //VERSION

	printf("Detected = %d!\n", count);

	#if MERGE
		int diff = 1;
		while (diff > 0) {
			diff = mergeRectangles();
		}
	#endif

	#if DATA_COLLECT_MODE==3
		FILE* fp_all = fopen(dataFileName, "a");
		printfree(head, fp_all);
	#else
		printfree(head, NULL);
	#endif
	printf("Total faces = %d!\n", count);
}

/** Initiating detectSingleScaleApprox with
  * increased window size and scaled features scanning through
  * the image
  * @param[in] pxls input RgbImage
  * @param[in] ann the fann neural network
  */
void detectMultiScaleApprox(RgbImage* pxls, struct fann* ann) {
	int max_window = min(pxls->w, pxls->h);
	float window = CLASSDIM;

	RgbImage* integral = integralImage(pxls, 0);
	RgbImage* integralsq = integralImage(pxls, 1);
	if(integral == NULL || integralsq == NULL) {
		return;
	}
	float scale = 1.0;
	while (window < max_window) {
		detectSingleScaleApprox(ann, pxls, (int)window);
		window = window * SCALE_FACTOR;
		scale = scale * SCALE_FACTOR;

	}
	freeRgbImage(integral);
	freeRgbImage(integralsq);

	printf("Detected = %d!\n", count);

	#if MERGE
		int diff = 1;
		while (diff > 0) {
			diff = mergeRectangles();
		}
	#endif

	#if DATA_COLLECT_MODE==3
		FILE* fp_all = fopen(dataFileName, "a");
		printfree(head, fp_all);
	#else
		printfree(head, NULL);
	#endif
	printf("Total faces = %d!\n", count);

}

/** Detects and prints faces coordinates from filename image using cascade
  * @param[in][in] classifier the haar classifier
  * @param[in][in] filename the input file
  */
void detect(Cascade* classifier, char* filename) {
	RgbImage srcImage;

	initRgbImage(&srcImage);

	loadRgbImage(filename, &srcImage);

	grayscale(&srcImage);

	detectMultiScale(&srcImage, classifier);

	freeRgbImage(&srcImage);
}

/** Detects and prints faces coordinates from filename image using ANN
  * @param[in][in] ann fann neural network classifier
  * @param[in][in] filename the input file
  */
void detectApprox(struct fann* ann, char* filename) {
	RgbImage srcImage;

	initRgbImage(&srcImage);

	loadRgbImage(filename, &srcImage);

	grayscale(&srcImage);

	detectMultiScaleApprox(&srcImage, ann);

	freeRgbImage(&srcImage);
}

int main(int argc, char **argv) {
	srand(1);

	#if DATA_COLLECT_MODE
		if (argc != 4) {
			printf("Usage: %s FILENAME CLASSIFIER OUTPUT_FILE\n", argv[0]);
			return -1;
		}
	#else
		if (argc != 3) {
			printf("Usage: %s FILENAME CASCADE\n", argv[0]);
			return -1;
		}
	#endif

	#if DATA_COLLECT_MODE
		dataFileName = argv[3];
	#endif
	int ann_mode, vj_mode;
	char *p = strrchr(argv[2], '.');
	ann_mode = p ? (strcmp(p, ".nn") == 0) : 0;
	vj_mode = p ? (strcmp(p, ".xml") == 0) : 0;
	if (ann_mode) {
		printf("ANN Mode.\n");
		struct fann *ann = fann_create_from_file(argv[2]);
		if (ann) {
			detectApprox(ann, argv[1]);
		}
		fann_destroy(ann);
	} else if (vj_mode) {
		printf("VJ Mode.\n");
		Cascade* classifier = loadCascade(argv[2]);
		if (classifier) {
			detect(classifier, argv[1]);
		}
		freeCascade(classifier);
	} else {
		printf("Classifier not recognized.\n");
	}

	printf("Incoc. Count: %d\n", invocCount);

	return 0;

}