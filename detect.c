
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "fann.h"
#include "rgb_image.h"
#include "parse.h"

#define INPIC "Images/gee.rgb"
//#define SIZEOF(array) sizeof(array) / sizeof(array[0])
#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

typedef struct Face {
	int window;
	int x;
	int y;
	struct Face* next;
} Face;

Face* head;
Face* tail;
int count = 0;
//faces = [] //what array? array of window sizes? guess as [x,y,?]

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
    printf("Grayscaling done.\n");
}

RgbImage* summed(RgbImage* pxls, int isSquared) {
	int i, c = 0;

	//Initialize and allocate for RgbImage
	RgbImage* result = (RgbImage *)malloc(sizeof(RgbImage));
	result->w = pxls->w;
	result->h = pxls->h;
	result->meta = NULL;

	result->pixels = (RgbPixel**)malloc(pxls->h * sizeof(RgbPixel*));
	if (result->pixels == NULL) {
        printf("Warning: Oops! Cannot allocate memory for the pixels!\n");
        return NULL;
    }
	for(i = 0; i < pxls->h; i++) {
        result->pixels[i] = (RgbPixel*)malloc(pxls->w * sizeof(RgbPixel));
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
	printf("Image allocated.\n");

	//Integral Image
	int x, y;

	for (y = 0; y < pxls->h; y++) {
		for (x = 0; x < pxls->w; x++) {
			float l = floor(pxls->pixels[x][y].r);
			if (isSquared) {
				l = l * l;			
			}
			//for leftmost pixel, copy from orig
			//TODO: is dividing by 255 correct? seems to give correct vals...
			if (x == 0 && y == 0) {
				//integral[x][y] = l;
				result->pixels[x][y].r = l;
				result->pixels[x][y].g = l;
				result->pixels[x][y].b = l;
			} else if(x == 0) {
				//integral[x][y] = l + integral[x][y-1];
				result->pixels[x][y].r = l + result->pixels[x][y-1].r;
				result->pixels[x][y].g = l + result->pixels[x][y-1].g;
				result->pixels[x][y].b = l + result->pixels[x][y-1].b;
			} else if (y == 0) {
				//integral[x][y] = l + integral[x-1][y];
				result->pixels[x][y].r = l + result->pixels[x-1][y].r;
				result->pixels[x][y].g = l + result->pixels[x-1][y].g;
				result->pixels[x][y].b = l + result->pixels[x-1][y].b;
			} else {
				//integral[x][y] = l + integral[x][y-1] + integral[x-1][y] - integral[x-1][y-1];
				result->pixels[x][y].r = l + result->pixels[x][y-1].r + result->pixels[x-1][y].r - result->pixels[x-1][y-1].r;
				result->pixels[x][y].g = l + result->pixels[x][y-1].g + result->pixels[x-1][y].g - result->pixels[x-1][y-1].g;
				result->pixels[x][y].b = l + result->pixels[x][y-1].b + result->pixels[x-1][y].b - result->pixels[x-1][y-1].b;
			}
			//printf("%d %d: %f, %f, %f\n", x, y, result->pixels[x][y].r, result->pixels[x][y].g, result->pixels[x][y].b);
		}
	}

	return result;

}


float getMean(RgbImage* integral, int x, int y, int window, int area) {
	return (integral->pixels[x][y].r - integral->pixels[x + window][y].r- integral->pixels[x][y + window].r 
		+ integral->pixels[x + window][y + window].r) / area;
}



float getFeatureVal(RgbImage* integral, Feature feat, float scale, int x, int y) {
	float totalFeatureVal = 0.0;
	int i;
	for (i = 0; i < feat.rectNum; i++) {
		int rectx = (int)(feat.rectList[i].x * scale + 0.5);
		int recty = (int)(feat.rectList[i].y * scale + 0.5);
		int rectw = (int)(feat.rectList[i].width * scale + 0.5);
		int recth = (int)(feat.rectList[i].height * scale + 0.5);

		int w1x = min(x + rectx, integral->w - 1);
		int w1y = min(y + recty, integral->h - 1);
		int w2x = min(x + rectx + rectw, integral->w - 1);
		int w2y = min(y + recty, integral->h - 1);
		int w3x = min(x + rectx , integral->w - 1);
		int w3y = min(y + recty + recth, integral->h - 1);
		int w4x = min(x + rectx + rectw, integral->w - 1);
		int w4y = min(y + recty + recth, integral->h - 1);

		totalFeatureVal += feat.rectList[i].weight * ( integral->pixels[w1x][w1y].r 
			- integral->pixels[w2x][w2y].r - integral->pixels[w3x][w3y].r + integral->pixels[w4x][w4y].r);
	}
	return totalFeatureVal;
}

/*
int mergeRectangles() {
	int diff = 0;
	int i;
	int j;
	for (i = 0; i < faces.length; i++) {
		for (j = 0; j < faces.length; j++) {

			if (!faces.contains(faces[i])||!faces.contains(faces[j])) {
				continue;
			}
			if (i == j) {
				continue;
			}

			int r1x1 = faces[i][1];
			int r1x2 = faces[i][1] + faces[i][0];
			int r1y1 = faces[i][2];
			int r1y2 = faces[i][2] + faces[i][0];

			int r2x1 = faces[j][1];
			int r2x2 = faces[j][1] + faces[j][0];
			int r2y1 = faces[j][2];
			int r2y2 = faces[j][2] + faces[j][0];

			//if they don't overlap, skip it
			if (r1x1 < r2x2 && r1x2 > r2x1 && r1y1 < r2y2 && r1y2 > r2y1) {
				int a1 = faces[i][0] * faces[i][0];
				int a2 = faces[j][0] * faces[j][0];

				float aIntersect = max(0,min(r1x2, r2x2) - max(r1x1, r2x1)) * max(0,min(r1y2, r2y2)-max(r1y1, r2y1));
				float aUnion = a1 + a2 - aIntersect;
				if (aIntersect/aUnion > 0.4) {
					faces[i][1] = min(r1x1, r2x1);
					faces[i][2] = min(r1y1, r2y1);
					faces[i][0] = max(max(r1x2, r2x2) - rect1[1], max(r1y2, r2y2) - rect1[2]);
					if (faces.contains(faces[j]) {
						faces.remove(rect2);
						diff++;
					]
				}
			}

	return diff;
}*/

void detectSingleScale(RgbImage* pxls, RgbImage* integral, RgbImage* integralsq, Cascade* classy, int window, float scale, char* filePath) {
	int width = integral->w;
	int height = integral->h;
	int area = window * window;
	int y, x;
	for (y = 0; y < height - window; y++) {
		for (x = 0; x < width - window; x++) {

			//fit = []
			//pix = np.array(img)
			//fit = shrink(pix, x, y, window, defined_size, downsp)
		
			float mean = getMean(integral, x, y, window, area);
			float variance = getMean(integral, x, y, window, area) - (mean * mean);

			printf("%d, %d: %f, %f\n", y, x, mean, variance);

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

					printf("%d, %f, %f, %f\n", classy->stages[i].nodeList[j].featind, classy->stages[i].nodeList[j].threshold, stagePassThresh, totalFeatureVal);
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
					if(tail->next == NULL) {
						tail->next = (Face *)malloc(sizeof(Face));
						tail = tail->next;
					}
					tail->window = window;
					tail->x = x;
					tail->y = y;
					tail->next = NULL;
					count++;
					printf("Potentially a face.\n");
				}
			}
		}
	}
}

void detectMultiScale(RgbImage* pxls, Cascade* classifier, char* filePath) {
	printf("In detectMultiScale.\n");
	int width = pxls->w; //maybe avoid img? only use pxls is enough
	int height = pxls->h;
	int max_window = min(width, height);

	//test summed
	RgbImage* integral = summed(pxls, 0);
	RgbImage* integralsq = summed(pxls, 1);

	//test getMean
	//float mean = getMean(integral, 1, 2, 25, 625);
	//float var = getMean(integral, 1, 2, 25, 625) - (mean * mean);
	//printf("%f, %f\n", mean, var);

	//test getFeatureVal and detectSingleScale
	float bounds = classifier->dim;
	float scale = 1.0;
	while (bounds < max_window) {
		bounds = bounds * 1.25;
		scale = scale * 1.25;
		head = (Face *)malloc(sizeof(Face));

		head->window = 0;
		head->x = 0;
		head->y = 0;
		head->next = NULL;

		tail = head;
		//printf("bounds: %d, scale: %f\n", (int)bounds, scale);
		detectSingleScale(pxls, integral, integralsq, classifier, (int)bounds, scale, filePath);
	}

	Face* cur = head->next;
	while(cur != NULL) {
		printf("[%d, %d, %d] | ", cur->window, cur->x, cur->y);
		cur = cur->next;
	}
	printf("total faces = %d!\n", count);

	//int diff = 1;
	//while (diff > 0) {
	//	diff = mergeRectangles();
	//}

	//printf("%d, %d, %d, %d\n", integral->w, integral->h, integralsq->w, integralsq->h);
	freeRgbImage(integral);
	freeRgbImage(integralsq);
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

	return 0;
}