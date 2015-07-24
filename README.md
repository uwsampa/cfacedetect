
1. Clone the repository to your Unix system
 * `git clone git@github.com:uwsampa/cfacedetect.git`
 
2. Type `make` to compile it. Make sure libxml2 and fann is installed.

3. Run `./detect` for face detection. 

Note: This only works rgb file converted from legitimate png.

Use png2rgb.py for conversion.
 * To convert from RGB to PNG: `$ python png2rgb.py png Images/example.rgb Images/example.png`
 * To convert from PNG to RGB: `$ python png2rgb.py rgb Images/example.png Images/example.rgb`
