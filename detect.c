
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "floatfann.h"
#include "rgb_image.h"
#include "parse.h"
#include "shrink.h"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define APPROX false //turns on fann

#define DOWNSP false //downsample or downscale

#define INPIC "Images/gee.rgb" //picture file

#define NN "vision_3L_36N.net" //fann file

#define DEFSIZE 20 //defined window size


//Structure for storing a face with x, y and window size 
typedef struct Face {
	int window;
	int x;
	int y;
	struct Face* next;
} Face;

//the head of linked list storing faces
Face* head = NULL;
int count = 0;

//Append face to the end of linked list
void push(int window, int x, int y) {
    Face* temp = (Face *)malloc(sizeof(Face));
    temp->window = window;
    temp->x = x;
    temp->y = y;
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

//Returns true if face is within the linked list
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

//Delete the face from the linked list
Face* delete(Face* face) {
    Face *temp, *prev;
    prev = head;
    temp = head;
    while(temp != NULL) {
    	if(face->window == temp->window && face->x == temp->x && face->y == temp->y) {
        	if(temp == head) {
        		head = temp->next;
        		free(temp);
        		return head;
        	} else {
		        prev->next = temp->next;
		        free(temp);
		        return prev->next;
		    }
		    break;
    	} else {
	        prev = temp;
	        temp = temp->next;
    	}
    }
    return NULL;
}

void printfree() {
	while(head != NULL) {
		printf("[%d, %d, %d] | ", head->window, head->x, head->y);
		Face* temp = head;
		head = head->next;
		free(temp);
	}
}

//Return a grayscaled RgbImage
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

//Return an integral image from pxls, isSquared determines if it's squared
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
				result->pixels[y][x].r = l;
				result->pixels[y][x].g = l;
				result->pixels[y][x].b = l;
			} else if (y == 0) {
				result->pixels[y][x].r = l + result->pixels[y][x-1].r;
				result->pixels[y][x].g = l + result->pixels[y][x-1].g;
				result->pixels[y][x].b = l + result->pixels[y][x-1].b;
				//printf("y == 0 : %.1f\n", result->pixels[y][x-1].r);
			} else if (x == 0) {
				result->pixels[y][x].r = l + result->pixels[y-1][x].r;
				result->pixels[y][x].g = l + result->pixels[y-1][x].g;
				result->pixels[y][x].b = l + result->pixels[y-1][x].b;
				//printf("x == 0 : %.1f\n", result->pixels[y-1][x].r);
			} else {
				result->pixels[y][x].r = l + result->pixels[y-1][x].r + result->pixels[y][x-1].r - result->pixels[y-1][x-1].r;
				result->pixels[y][x].g = l + result->pixels[y-1][x].g + result->pixels[y][x-1].g - result->pixels[y-1][x-1].g;
				result->pixels[y][x].b = l + result->pixels[y-1][x].b + result->pixels[y][x-1].b - result->pixels[y-1][x-1].b;
				//printf("%.1f %.1f %.1f\n", result->pixels[y-1][x].r, result->pixels[y][x-1].r, result->pixels[y-1][x-1].r);
			}
			
		}
	}

	return result;

}

//Return the mean of integral image area
float getMean(RgbImage* integral, int x, int y, int window, int area) {
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

		//printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", integral->w, integral->h, rectx, recty, rectw, recth, w1x, w1y, w2x, w2y, w3x, w3y, w4x, w4y);

		totalFeatureVal += feat.rectList[i].weight * ( integral->pixels[w1y][w1x].r 
			- integral->pixels[w2y][w2x].r - integral->pixels[w3y][w3x].r + integral->pixels[w4y][w4x].r);
	}
	return totalFeatureVal;
}

//Merges all the faces, delete them if they overlap by 40%
//Look into this may find out the memory leak
int mergeRectangles() {
	int diff = 0;
	int i, j;
	bool del = false;
	

	int size = count;
	Face* rect1 = head;
	for (i = 0; i < size; i++) {
		
		Face* rect2 = head;
		int r1x1 = rect1->x;
		int r1x2 = r1x1 + rect1->window;
		int r1y1 = rect1->y;
		int r1y2 = r1y1 + rect1->window;
		for (j = 0; j < size; j++) {
			printf("(%d, %d, %d) | (%d, %d, %d)\n", rect1->window, rect1->x, rect1->y, rect2->window, rect2->x, rect2->y);

			if (!contains(rect1) || !contains(rect2)) {
				printf("first if\n");
				continue;
			}

			if (i == j) {
				printf("second if, same rect\n");
				rect2 = rect2->next;
				continue;
			}

			//printf("%d %d %d %d\n", size, count, i, j);

			int r2x1 = rect2->x;
			int r2x2 = r2x1 + rect2->window;
			int r2y1 = rect2->y;
			int r2y2 = r2y1 + rect2->window;

			if (r1x1 < r2x2 && r1x2 > r2x1 && r1y1 < r2y2 && r1y2 > r2y1) {
				printf("overlap\n");
				int a1 = rect1->window * rect1->window;
				int a2 = rect2->window * rect2->window;
				int aIntersect = max(0, min(r1x2, r2x2) - max(r1x1, r2x1)) * max(0, min(r1y2, r2y2) - max(r1y1, r2y1));
				int aUnion = a1 + a2 - aIntersect;
				//printf("%d %d %d %d ", min(r1x2, r2x2), max(r1x1, r2x1), min(r1y2, r2y2), max(r1y1, r2y1));
				//printf("%d %d %f\n", aIntersect, aUnion, (float)(aIntersect / aUnion));
				if ((float)(aIntersect) / aUnion > 0.4) {
					printf("by more than 40 percent\n");
					rect1->x = min(r1x1, r2x1);
					rect1->y = min(r1y1, r2y1);
					rect1->window = max(max(r1x2, r2x2) - rect1->x, max(r1y2, r2y2) - rect1->y);
					if (contains(rect2)) {
						printf("rect2 in faces and removed\n");
						rect2 = delete(rect2);
						count--;
						del = true;
						diff++;
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
		rect1 = rect1->next;
	}
	
	return diff;
}

//Sliding the fixed cascade through the picture to figure out if it's a face.
//If a face store it at the end of linked list
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
					push(window, x, y);
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
				push(window, x, y);
				count++;
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
	//int width = pxls->w; //maybe avoid img? only use pxls is enough
	//int height = pxls->h;
	int max_window = min(pxls->w, pxls->h);

	RgbImage* integral;
	RgbImage* integralsq;

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

	printf("Merging.\n");
	int diff = 1;
	while (diff > 0) {
		diff = mergeRectangles();
	}
	
	printf("Start printing.\n");
	printfree();
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