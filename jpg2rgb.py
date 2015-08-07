
from PIL import Image
import png2rgb as pg
import sys
import os

def jpg2rgb(imagePath):
	im = Image.open(imagePath)
	newPath = str(imagePath.split(".")[0] + ".png")
	im.save(newPath)
	img = pg.png2rgb(newPath)
	pg.rgbsave(img, str(newPath.split(".")[0] + ".rgb"))
	os.remove(newPath)


if __name__ == '__main__':
	if len(sys.argv) is not 2:
		print('Error: Oops! wrong arguments!')
		print('Usage: ' + sys.argv[0] + ' INPUT_FILE OUTPUT_FILE')
		exit(-1)
	jpg2rgb(sys.argv[1])