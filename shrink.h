
#ifndef SHRINK_H_
#define SHRINK_H_

RgbImage* allocate(int width, int height);

RgbImage* shrink(RgbImage* pxls, int x, int y, int size, int scale, int downsp);

#endif