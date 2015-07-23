
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "floatfann.h"
#include "rgb_image.h"
#include "parse.h"
#include "shrink.h"

#define INPIC "Images/gee.rgb"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define APPROX true

#define DOWNSP false

#define NN "vision_3L_36N.net"

#define DEFSIZE 20


typedef struct Face {
	int index;
	int window;
	int x;
	int y;
	struct Face* next;
} Face;

Face* head = NULL;
int count = 0;

void push(int window, int x, int y, int index) {
    Face* temp = (Face *)malloc(sizeof(Face));
    temp->window = window;
    temp->x = x;
    temp->y = y;
    temp->index = index;
    temp->next = NULL;
    Face* cur = head;
    if (head == NULL) {
    	head = temp;
    } else {
	    while (cur->next != NULL) {
	        cur = cur->next;
	    }
    	cur->next = temp;
    } 
}

bool contains(Face* face) {
	Face* temp = head;
	while(temp != NULL) {
		if(face->window == temp->window && face->x == temp->x && face->y == temp->y) {
			return true;
		}
		temp = temp->next;
	}
	return false;
}

void delete(Face* face) {
    Face *temp, *prev;
    prev = head;
    temp = head;
    while(temp != NULL) {
    	if(face->window == temp->window && face->x == temp->x && face->y == temp->y) {
    		Face* cur;
        	if(temp == head) {
        		head = temp->next;
        		free(temp);
        		cur = head;
        	} else {
		        prev->next = temp->next;
		        free(temp);
		        cur = prev->next;
		    }
		    while(cur != NULL) {
		    	cur->index -= 1;
		    	cur = cur->next;
		    }
		    break;
    	} else {
	        prev = temp;
	        temp = temp->next;
    	}
    }
}

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
           // printf("%d %d : %.1f\n", i, j, floor(luminance));
        }
    }
    printf("Grayscaling done.\n");
}

RgbImage* summed(RgbImage* pxls, int isSquared) {

	//Initialize and allocate for RgbImage
	RgbImage* result = allocate(pxls->w, pxls->h);

	if(result == NULL) {
		return NULL;
	}

	//Integral Image
	int x, y;

	for (y = 0; y < pxls->h; y++) {
		for (x = 0; x < pxls->w; x++) {
			//printf("%d, %d: %f\n", y, x, floor(pxls->pixels[y][x].r));
			float l = pxls->pixels[y][x].r;
			//printf("%d %d : %.1f\n", y, x, l);
			if (isSquared) {
				l = l * l;			
			}
			//for leftmost pixel, copy from orig
			//TODO: is dividing by 255 correct? seems to give correct vals...
			if (x == 0 && y == 0) {
				//integral[y][x] = l;
				result->pixels[y][x].r = l;
				result->pixels[y][x].g = l;
				result->pixels[y][x].b = l;
			} else if (y == 0) {
				//integral[y][x] = l + integral[y-1][x];
				result->pixels[y][x].r = l + result->pixels[y][x-1].r;
				result->pixels[y][x].g = l + result->pixels[y][x-1].g;
				result->pixels[y][x].b = l + result->pixels[y][x-1].b;
				//printf("y == 0 : %.1f\n", result->pixels[y][x-1].r);
			} else if (x == 0) {
				//integral[y][x] = l + integral[y][x-1];
				result->pixels[y][x].r = l + result->pixels[y-1][x].r;
				result->pixels[y][x].g = l + result->pixels[y-1][x].g;
				result->pixels[y][x].b = l + result->pixels[y-1][x].b;
				//printf("x == 0 : %.1f\n", result->pixels[y-1][x].r);
			} else {
				//integral[y][x] = l + integral[y-1][x] + integral[y][x-1] - integral[y-1][x-1];
				result->pixels[y][x].r = l + result->pixels[y-1][x].r + result->pixels[y][x-1].r - result->pixels[y-1][x-1].r;
				result->pixels[y][x].g = l + result->pixels[y-1][x].g + result->pixels[y][x-1].g - result->pixels[y-1][x-1].g;
				result->pixels[y][x].b = l + result->pixels[y-1][x].b + result->pixels[y][x-1].b - result->pixels[y-1][x-1].b;
				//printf("%.1f %.1f %.1f\n", result->pixels[y-1][x].r, result->pixels[y][x-1].r, result->pixels[y-1][x-1].r);
			}
			
		}
	}

	return result;

}


float getMean(RgbImage* integral, int x, int y, int window, int area) {
	return (integral->pixels[y][x].r - integral->pixels[y + window][x].r- integral->pixels[y][x + window].r 
		+ integral->pixels[y + window][x + window].r) / area;
}



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

		//printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", integral->w, integral->h, rectx, recty, rectw, recth, w1x, w1y, w2x, w2y, w3x, w3y, w4x, w4y);

		totalFeatureVal += feat.rectList[i].weight * ( integral->pixels[w1y][w1x].r 
			- integral->pixels[w2y][w2x].r - integral->pixels[w3y][w3x].r + integral->pixels[w4y][w4x].r);
	}
	return totalFeatureVal;
}


int mergeRectangles() {
	int diff = 0;
	int i, j;
	bool del = false;
	Face* rect1 = head;
	Face* rect2 = head;

	int size = count;

	for (i = 0; i < count; i++) {
		for (j = 0; j < size; j++) {
			if (i == j) {
				continue;
			}
			if (!contains(rect1) || !contains(rect2)) {
				continue;
			}

			//printf("%d %d %d %d\n", size, count, i, j);
			int r1x1 = rect1->x;
			int r1x2 = r1x1 + rect1->window;
			int r1y1 = rect1->y;
			int r1y2 = r1y1 + rect1->window;

			int r2x1 = rect2->x;
			int r2x2 = r2x1 + rect2->window;
			int r2y1 = rect2->y;
			int r2y2 = r2y1 + rect2->window;

			if (r1x1 < r2x2 && r1x2 > r2x1 && r1y1 < r2y2 && r1y2 > r2y1) {
				int a1 = rect1->window * rect1->window;
				int a2 = rect2->window * rect2->window;
				int aIntersect = max(0, min(r1x2, r2x2) - max(r1x1, r2x1)) * max(0, min(r1y2, r2y2) - max(r1y1, r2y1));
				int aUnion = a1 + a2 - aIntersect;
				//printf("%d %d %d %d ", min(r1x2, r2x2), max(r1x1, r2x1), min(r1y2, r2y2), max(r1y1, r2y1));
				//printf("%d %d %f\n", aIntersect, aUnion, (float)(aIntersect / aUnion));
				if ((float)(aIntersect / aUnion) > 0.4) {
					rect1->x = min(r1x1, r2x1);
					rect1->y = min(r1y1, r2y1);
					rect1->window = max(max(r1x2, r2x2) - rect1->x, max(r1y2, r2y2) - rect1->y);
					if (contains(rect2)) {
						Face* temp = rect2->next;
						delete(rect2);
						del = true;
						rect2 = temp;
						diff++;
						count--;
						//printf("Deleted.\n");
					}
				}
			}
			if (!del) {
				rect2 = rect2->next;
			} else {
				del = false;
			}
		}
		size = count;
		rect1 = rect1->next;
	}
	
	return diff;
}

void detectSingleScale(RgbImage* integral, RgbImage* integralsq, Cascade* classy, int window, float scale, char* filePath) {
	int width = integral->w;
	int height = integral->h;
	int area = window * window;
	int y, x;
	for (y = 0; y < height - window; y++) {
		for (x = 0; x < width - window; x++) {

			//RgbImage* fit = shrink(pix, x, y, window, defined_size, true);
		
			float mean = getMean(integral, x, y, window, area);
			float variance = getMean(integralsq, x, y, window, area) - (mean * mean);

			//printf("%d %d %f %f\n", y, x, mean, variance);

			float stdev = 1.0;

			if (variance > 0) {
				stdev = sqrt(variance);
			}

			int i, j;
			for (i = 0; i < classy->stgNum; i++) {
				float stagePassThresh = 0.0;

				for (j = 0; j < classy->stages[i].nodeNum; j++) { 
					Feature feat = classy->features[classy->stages[i].nodeList[j].featind];

					//sum in rectangle is D - B - C + A
					float totalFeatureVal = getFeatureVal(integral, feat, scale, x, y);
					
					if (totalFeatureVal / area < classy->stages[i].nodeList[j].threshold * stdev) {
						stagePassThresh += classy->stages[i].nodeList[j].weights[0];
					} else { 
						stagePassThresh += classy->stages[i].nodeList[j].weights[1];
					}

					//printf("%d %f %f %f\n", classy->stages[i].nodeList[j].featind, classy->stages[i].nodeList[j].threshold, stagePassThresh, totalFeatureVal);
				}

				if (stagePassThresh < classy->stages[i].threshold) {
					//if (zerocount < onecount)
					//	printPix(fit, 0, filePath)
					//	zerocount = zerocount + 1
					break;
				}
				
				if ( i + 1 == classy->stgNum) {
					//printPix(fit, 1, filePath)
					//onecount = onecount + 1
					//faces.append([window, x, y])
					push(window, x, y, count);
					count++;
				}
			}
		}
	}
}

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
				push(window, x, y, count);
				count++;
			} else {
				break;
			}
		}
	}
}

void detectMultiScale(RgbImage* pxls, Cascade* classifier, char* filePath) {
	printf("In detectMultiScale.\n");
	//int width = pxls->w; //maybe avoid img? only use pxls is enough
	//int height = pxls->h;
	int max_window = min(pxls->w, pxls->h);

	RgbImage* integral, integralsq;

	struct fann *ann;


	#if APPROX
		ann = fann_create_from_file(NN);
	#else
		integral = summed(pxls, 0);
		integralsq = summed(pxls, 1);

		if(integral == NULL || integralsq == NULL) {
			return;
		}

	#endif
	

	//test getFeatureVal and detectSingleScale
	float bounds = classifier->dim;
	float scale = 1.0;
	
	while (bounds < max_window) {
		bounds = bounds * 1.25;
		scale = scale * 1.25;
		#if APPROX
			approxDetectSingleScale(ann, pxls, (int)bounds, filePath);
		#else
			detectSingleScale(integral, integralsq, classifier, (int)bounds, scale, filePath);
		#endif
	}

	printf("Finished detecting.\n");

	printf("Detected faces = %d!\n", count);

	
	int diff = 1;
	while (diff > 0) {
		printf("Merging.\n");
		diff = mergeRectangles();
	}
	
	printf("Start printing.\n");
	while(head != NULL) {
		printf("[%d, %d, %d] | ", head->window, head->x, head->y);
		Face* temp = head;
		head = head->next;
		free(temp);
	}
	printf("Total faces = %d!\n", count);

	#if APPROX
		fann_destroy(ann);
	#else
		freeRgbImage(integral);
		freeRgbImage(integralsq);
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
		detectMultiScale(&srcImage, cas, "script.txt");
		freeCascade(cas);
	}

	freeRgbImage(&srcImage);

	//for testing shrink
	/*
	RgbImage srcImage;
	initRgbImage(&srcImage);
	loadRgbImage("Images/gee.rgb", &srcImage);

	RgbImage* dstImage = shrink(&srcImage, 0, 0, 100, 30, false);

	if (dstImage != NULL) {
		saveRgbImage(dstImage, "Images/shrink.rgb", 1);
		freeRgbImage(dstImage);
	}
	
	freeRgbImage(&srcImage);
	*/

	return 0;
}