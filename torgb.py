from PIL import Image
import sys

## Converts image file to rgb(same filename with rgb extension)
# @param[in] filePath the image file path
def torgb(filePath):
	name = filePath.split(".")[0]
	im = Image.open(filePath)
	pix = im.load()
	rgb = open(name + ".rgb", "w")

	rgb.write(str(im.size[0]) + "," + str(im.size[1]) + "\n")

	for y in range(0, im.size[1]):
		rgb.write(str(pix[0, y][0]) + "," + str(pix[0, y][1]) + "," + str(pix[0, y][2]))
		for x in range(1, im.size[0]):
			rgb.write("," + str(pix[x, y][0]) + "," + str(pix[x, y][1]) + "," + str(pix[x, y][2]))
		rgb.write("\n")


if __name__ == '__main__':
	if len(sys.argv) is not 2:
		print('Error: Oops! wrong arguments!')
		print('Usage: ' + sys.argv[0] + ' INPUT_FILE')
		exit(-1)
	torgb(sys.argv[1])