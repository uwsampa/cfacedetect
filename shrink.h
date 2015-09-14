
#ifndef SHRINK_H_
#define SHRINK_H_

/** Scales image in 4 modes
  * 1: downsampling
  * 2: downscaling
  * 3: maxpooling
  * 4: minpooling
  */
#define SHRINK_MODE 1

RgbImage* shrink(RgbImage* pxls, int x, int y, int OrigWidth, int OrigHeight, int scaleWidth, int scaleHeight);

#endif