import os
import sys
sys.path.append("../facedetect/")
import subprocess
import glob
import time
from facedetect import grayscale
from facedetect import detectMultiScale
import parse_cascade
from torgb import torgb
from PIL import Image

## Calling the C version face detection
# @param[in] imagePath path of the input rgb image
# @return detected faces
def cfunc(imagePath):
	outstring = subprocess.check_output(["./detect", imagePath])
	result = outstring.split("\n")[4]
	result = result.split(" | ")
	result.pop()
	faces = []
	for x in xrange(len(result)):
		window, x, y = result[x].split(", ")
		window = int(window.split("[")[1])
		x = int(x)
		y = int(y.split("]")[0])
		faces.append([window, x, y])
	faces.sort()
	return faces

## Cally Python version face detection
# @param[in] imagePath image file path
# @param[in] classifier the haar classifier
# @param[in] features features to be computed
# @param[in] bound window size
# @return detected faces
def pfunc(imagePath, classifier, features, bound):
	img = Image.open(imagePath)

	img_pxls = img.load()

	gray = grayscale(img, img_pxls)

	faces = detectMultiScale(img, gray, classifier, features, bound, "debug.txt")

	faces.sort()
	return faces

## Compare the output from two versions of face detection algorithm(Python vs. C)
# and their running time
# @arg the input image directory
if __name__ == '__main__':

	if len(sys.argv) is not 2: 
		print('Error: Oops! wrong arguments!')
		print('Usage: ' + sys.argv[0] + ' IMAGE_DIRECTORY')

	## Haar classifier
	classifier = []
	## features
	features = []
	classifier, features, bound = parse_cascade.load_cascade("xml/ocv_clsfr.xml")

	subprocess.call(["make"])

	directory = str(sys.argv[1])
	jpgs = glob.glob(directory + "*.jpg")

	## python total running time
	pt = 0
	## c total running time
	ct = 0
	## number of differences
	error = 0.0
	## number of images
	count = len(jpgs)

	for jpg in jpgs:
		torgb(jpg)
		## name of the jpg file
		name = jpg.split(".")[0]
		## time python starts running
		ps = time.time()
		## faces returned from python
		pf = pfunc(jpg, classifier, features, bound)
		## python running time
		pe = time.time() - ps
		## time c starts running
		cs = time.time()
		## faces returned from c
		cf = cfunc(name + ".rgb")
		## c running time
		ce = time.time() - cs
		os.remove(name + ".rgb")
		print "Python vs. C (execution time):", pe, "vs.", ce 
		pt += pe
		ct += ce
		print "Python:", pf
		print "C:", cf
		if cmp(pf, cf) is not 0:
			error += 1

	print "Python vs. C (total execution time):", pt, "vs.", ct
	print "Error percent:", error/count

	subprocess.call(["make", "clean"])
