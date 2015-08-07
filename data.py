import os
import sys
import glob
import subprocess
from jpg2rgb import jpg2rgb

if __name__ == '__main__':
	if len(sys.argv) is not 3:
		print('Error: Oops! wrong arguments!')
		print('Usage: ' + sys.argv[0] + ' IMAGE_DIRECTORY OUTPUT_FILE')
		exit(-1)

	subprocess.call(["make"])

	directory = str(sys.argv[1])
	output = str(sys.argv[2])

	images = glob.glob(directory + "*.jpg")

	for image in images:
		jpg2rgb(image)

	images = glob.glob(directory + "*.rgb")

	pic = 0
	for image in images:
		pic = pic + 1
		print pic, image
		subprocess.call(["./detect", image, output])

	subprocess.call(["make", "clean"])
