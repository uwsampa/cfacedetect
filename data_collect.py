import os
import subprocess
import tempfile
import shutil
import random
import argparse
import logging
import fnmatch
from PIL import Image
import sys


LOG_FILE = "data_collect.log"
DATA_FILE = "out.data"

def shell(command, cwd=None, shell=False):
    return subprocess.check_output(
        command,
        cwd=cwd,
        stderr=subprocess.STDOUT,
        shell=shell,
    )

def jpgToRgb(inPath, outPath):
    jpg = Image.open(inPath)
    pix = jpg.load()
    rgb = open(outPath, "w")

    rgb.write(str(jpg.size[0]) + "," + str(jpg.size[1]) + "\n")

    for y in range(0, jpg.size[1]):
        rgb.write(str(pix[0, y][0]) + "," + str(pix[0, y][1]) + "," + str(pix[0, y][2]))
        for x in range(1, jpg.size[0]):
            rgb.write("," + str(pix[x, y][0]) + "," + str(pix[x, y][1]) + "," + str(pix[x, y][2]))
        rgb.write("\n")

def crawl(imdir, outfile):

    jpegs = []
    for root, dirnames, filenames in os.walk(imdir):
      for filename in fnmatch.filter(filenames, '*.jpg'):
        jpegs.append(os.path.join(root, filename))
    logging.info('Found {} images in {}'.format(len(jpegs), imdir))

    rgbDir = tempfile.mkdtemp()+'/'
    logging.debug('New directory created: {}'.format(rgbDir))

    rgbs = []
    for jpeg in jpegs:
        fn = os.path.basename(jpeg)
        rgb = rgbDir+os.path.splitext(fn)[0]+".rgb"
        logging.debug('Converting {} to rgb format'.format(fn))
        jpgToRgb(jpeg, rgb)
        rgbs.append(rgb)

    logging.info('Converted {} jpegs to rgb format'.format(len(rgbs)))

    try:
        shell("make")
    except:
        logging.error('Compiling detect.c failed')
        exit()

    logging.info('Compiled face detection program successfully')

    trainingSet = []

    for rgb in rgbs:
        dataFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]
        try:
            logging.debug('Running face detection and data collection on {}'.format(dataFile))
            shell(["./detect", rgb, dataFile])
        except:
            logging.error('Face detection on {} failed'.format(rgb))
            exit()
        trainingSet = process(dataFile)

        with open(outfile, 'a') as f:
            for dat in trainingSet:
                f.write("{}\n{}\n".format(dat[0], dat[1]))

    shutil.rmtree(rgbDir)

def merge(large, small):
    tempData = []
    sampleIndices = random.sample(xrange(len(large)), len(small))
    tempData = [large[i] for i in sampleIndices]
    tempData += small
    random.shuffle(tempData)
    return tempData

def process(dataFile):
    pDataFile = dataFile+'.pos.data'
    nDataFile = dataFile+'.neg.data'

    pData = []
    nData = []

    for dataSet in [[pDataFile, pData, 1], [nDataFile, nData, 0]]:
        with open(dataSet[0]) as f:
            for line in f:
                dataSet[1].append([line.strip(), dataSet[2]])

    logging.debug("Obtained {} positive samples, and {} negative samples".format(len(pData), len(nData)))

    mergedData = []
    if len(nData) > len(pData):
        mergedData = merge(nData, pData)
    else:
        mergedData = merge(pData, nData)

    return mergedData

def cli():
    parser = argparse.ArgumentParser(
        description='Face detection training data collection script'
    )
    parser.add_argument(
        '-dir', dest='imdir', action='store', type=str, required=False,
        default=".", help='image dataset directory'
    )
    parser.add_argument(
        '-d', dest='debug', action='store_true', required=False,
        default=False, help='print out debug messages'
    )
    parser.add_argument(
        '-log', dest='logpath', action='store', type=str, required=False,
        default=LOG_FILE, help='path to log file'
    )
    parser.add_argument(
        '-out', dest='outfile', action='store', type=str, required=False,
        default=DATA_FILE, help='path to output data file'
    )
    args = parser.parse_args()

    # Take care of log formatting
    logFormatter = logging.Formatter("%(asctime)s [%(threadName)-12.12s] [%(levelname)-5.5s]  %(message)s", datefmt='%m/%d/%Y %I:%M:%S %p')
    rootLogger = logging.getLogger()

    open(args.logpath, 'a').close()
    fileHandler = logging.FileHandler(args.logpath)
    fileHandler.setFormatter(logFormatter)
    rootLogger.addHandler(fileHandler)

    consoleHandler = logging.StreamHandler()
    consoleHandler.setFormatter(logFormatter)
    rootLogger.addHandler(consoleHandler)

    if(args.debug):
        rootLogger.setLevel(logging.DEBUG)
    else:
        rootLogger.setLevel(logging.INFO)

    if (os.path.isdir(args.imdir)):
        crawl(args.imdir, args.outfile)
    else:
        print ("Error: Directory {} does not exist".format(args.imdir))

if __name__ == '__main__':
    cli()
