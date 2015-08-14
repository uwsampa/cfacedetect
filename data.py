import os
import sys
import glob
import subprocess
from torgb import torgb

## Collect data for FANN trainning from detect.c and writes to output files
# @arg the image directory path
# @arg the output file
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
		torgb(image)

	images = glob.glob(directory + "*.rgb")

	pic = 0
	for image in images:
		pic = pic + 1
		print pic, image
		subprocess.call(["./detect", image, output])

	subprocess.call(["make", "clean"])
