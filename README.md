
1. Clone the repository to your Unix system
 * `git clone git@github.com:uwsampa/cfacedetect.git`
 
2. Type `make` to compile it. Make sure libxml2 and fann is installed.

3. Run `./detect <rgb filename>` for face detection. 

4. Run `python data.py <directory> <output>` for data collection.
 * make sure turns on DATA flag in detect.c.

Note: detect only works for rgb picture files

Use png2rgb.py or jpg2rgb.py for image conversion.
 * To convert from RGB to PNG: `$ python png2rgb.py png <rgb filename> <png filename>`
 * To convert from PNG to RGB: `$ python png2rgb.py rgb <png filename> <rgb filename>`
 * To convert from JPG to RGB: `$ python jpg2rgb.py <jpg filename>`


