
1. Clone the repository to your Unix system
 * `git clone git@github.com:uwsampa/cfacedetect.git`
 
2. Type `make` to compile it. Make sure libxml2 and fann is installed.
 * [FANN](http://leenissen.dk/fann/wp/)
 * [libxml2](http://www.xmlsoft.org/)

3. Run `./detect <rgb filename>` for face detection. 
 * make sure turns off DATA flag in detect.c
 * turns on APPROX flag for fann activation
 * turns on DEBUG flag for debug info
 * turns on ADAPTIVE_STEP flag for adaptive steping
 * switch version of face detection with VERSION

4. Run `python data.py <directory> <output>` for data collection.
 * make sure turns on DATA flag in detect.c.

5. Run `python compare.py <directory>` for compare results with python face detection
 * [Python facedetect](https://github.com/uwsampa/facedetect)
 * make sure facedetect is on the same level as cfacedetect, otherwise change the directory path

6. `doxygen config` to generate documentations

Note: detect only works for rgb picture files

Use torgb.py for image conversion.
 * `$ python torgb.py <input>`


