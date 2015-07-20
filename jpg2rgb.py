
from PIL import Image
import png2rgb as pg

imagePath = "Images/c.jpg"

if __name__ == '__main__':
	im = Image.open(imagePath)
	newPath = str(imagePath.split(".")[0] + ".png")
	im.save(newPath)
	img = pg.png2rgb(newPath)
	pg.rgbsave(img, str(newPath.split(".")[0] + ".rgb"))