
#include <stdio.h>
#include <math.h>
//#include "fann.h"
#include "rgb_image.h"
#include "parse.h"

#define INPIC "lena.png"
#define STAGE 22
#define SIZEOF(array) sizeof(array) / sizeof(array[0])
#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

//need to figure out how to load rbg image

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

            image->pixels[i][j].r = luminance;
            image->pixels[i][j].g = luminance;
            image->pixels[i][j].b = luminance;
        }
    }
}
/*
float[][] integral(float[][] pxls, bool isSquared) {
	float integral[width][height]; //use pxls to figure out dimension
	unsigned x, y;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {

			int r = img_pxls[x, y][0];
			int g = img_pxls[x, y][1];
			int b = img_pxls[x, y][2];
			float grays = floor(r * .299 + g * .587 + b * .114)
			if (isSquared) {
				l = grays;
			} else {
				l = (grays)*(grays);
			}
			//for leftmost pixel, copy from orig
			//TODO: is dividing by 255 correct? seems to give correct vals...
			if (x == 0 && y == 0) {
				integral[x][y] = l;

			} else if(x == 0) {
				integral[x][y] = l + integral[x][y-1];
			} else if (y == 0) {
				integral[x][y] = l + integral[x-1][y];
			} else {
				integral[x][y] = l + integral[x][y-1] + integral[x-1][y] - integral[x-1][y-1];
			}

	return integral;

}

float getVariance(float[][] integralsq, x, y, mean, window, area) {
	return (integralsq[x][y] - integralsq[x+window][y] - integralsq[x][y+ window] + integralsq[x + window][y + window]) / area - mean * mean;

	return variance
}

float getMean(float[][] integral, x, y, window, area) {
	return (integral[x][y] - integral[x+window][y]- integral[x][y+ window] + integral[x + window][y + window]) / area;
}

float getFeatureVal() {

}

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
}

void detectSingleScale(img?, float[][] integral, float[][] integralsq, classifier* classy, feature* features, int window, int scale) {
	int width;
	int height;
	int area = window * window
	unsigned y, x;
	for (y = 0; y < height - window; y++) {
		for (x = 0; x < width - window; x++) {

			fit = []
			pix = np.array(img)
			fit = shrink(pix, x, y, window, defined_size, downsp)
		
			float mean = getMean(intimg, x, y,window,area)
			float variance = getVariance(intimgsq, x, y, mean,window,area)

			float stdev;

			if (variance > 0) {
				stdev = sqrt(variance)
			}

			unsigned i, j;
			for (i = 0; i < STAGE; i++) {
				float stagePassThresh = 0.0;

				for (j = 0; j < SIZEOF(classy[i]->list); j++) { 
					feature = features[classy[i]->list[j].feat_ind];

					# sum in rectangle is D - B - C + A
					total_feat_val = getFeatureVal(feature->rect_list, integral, scale, x, y, width, height);
					
					if (total_feat_val/area < node.threshold*stdev) {
						stagePassThresh += node->weights[0];

					} else { 
						stagePassThresh += node->weights[1];
					}

				if (stagePassThresh < stage->threshold) {
					if (zerocount < onecount)
						printPix(fit, 0, filePath)
						zerocount = zerocount + 1
					break;
				}
				
				if ( i + 1 == STAGE) {
					//printPix(fit, 1, filePath)
					//onecount = onecount + 1
					faces.append([window, x, y])
				}

}*/

void detectMultiScale(RgbImage* pxls, Cascade* classifier, char* filePath) {
	int width = pxls->w; //maybe avoid img? only use pxls is enough
	int height = pxls->h;
	int max_window = min(width, height);
	//RgbImage integral = integral(pxls, FALSE);
	//RgbImage integralsq = integral(pxls, TRUE);
	printf("%d, %d, %d\n", width, height, max_window);

	float bounds = classifier->dim;
	float scale = 1.0;
	while (bounds < max_window) {
		bounds = bounds * 1.25;
		scale = scale * 1.25;
		//detectSingleScale(img, int_img, int_img_sq, classifier, features, (int)bounds, scale, filePath)
	}
	//int diff = 1;
	//while (diff > 0) {
	//	diff = mergeRectangles();
	//}
	//printf(faces); maybe a to string method
	//printf("total faces = %d!\n", len(faces));
}


int main(int argc, char* argv[]) {
	RgbImage srcImage;
	initRgbImage(&srcImage);

	if (argc == 1) {
		
		loadRgbImage(argv[0], &srcImage);
	} else {
		loadRgbImage(INPIC, &srcImage);
	}
	grayscale(&srcImage);

	Cascade* cas = loadCascade("5x5.xml");//ocv_clsfr.xml");
	
	if(!cas) {
		detectMultiScale(&srcImage, cas, "script.txt");
		freeCascade(cas);
	}

	freeRgbImage(&srcImage);

	return 0;
}