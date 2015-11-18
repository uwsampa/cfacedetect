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

def compileExecutable(window, cascade="xml/ocv_clsfr.xml"):
    try:
        shell(["make","clean"])
        defineStr = "DEFINES=\"-DDEFSIZE="+str(window)
        defineStr += " -DCOMMON_DUMP"
        defineStr += " -DDRAW_PROB=1"
        defineStr += " -DCASCADE="+cascade
        defineStr += "\""
        shell(["make", defineStr])
    except:
        logging.error('Compiling face detection program failed')
        exit()

def test(path, outfile, window, testCascade):

    # Collect input sources
    jpegs = []
    if (os.path.isfile(path)):
        # If it's a file
        jpegs.append(path)
        logging.info('Added image {}'.format(path))
    elif (os.path.isdir(path)):
        # If it's a directory
        for root, dirnames, filenames in os.walk(path):
          for filename in fnmatch.filter(filenames, '*.jpg'):
            jpegs.append(os.path.join(root, filename))
        logging.info('Found {} images in {}'.format(len(jpegs), path))
    else:
        # Path invalid
        logging.error('Path invalid: {}'.format(path))
        exit()

    # Temporary directory for temporary files
    try:
        # rgbDir = tempfile.mkdtemp()+'/'
        rgbDir = './'
        logging.debug('New directory created: {}'.format(rgbDir))

        trainingSamples = []
        for jpeg in jpegs:
            # Convert jpgs to rgbs
            fn = os.path.basename(jpeg)
            rgb = rgbDir+os.path.splitext(fn)[0]+".rgb"
            logging.debug('Converting {} to rgb format'.format(fn))
            jpgToRgb(jpeg, rgb)
            logging.debug('Conversion successful: {}'.format(rgb))

            # Now perform data collection on both cascades:
            origFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]+'_orig'
            testFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]+'_test'
            # Collect original labels
            try:
                compileExecutable(window)
                logging.debug('Running face detection and data collection on {} with original cascade'.format(origFile))
                shell(["./detect", rgb, origFile])
            except:
                logging.error('Face detection on {} failed'.format(rgb))
                exit()
            logging.debug('Face detection successful!')
            origData = process(origFile)
            # Collect test labels
            try:
                compileExecutable(window, testCascade)
                logging.debug('Running face detection and data collection on {} with test cascade {}'.format(testFile, testCascade))
                shell(["./detect", rgb, testFile])
            except:
                logging.error('Face detection on {} failed'.format(rgb))
                exit()
            logging.debug('Face detection successful!')
            testData = process(testFile)

            # Perform Data merger:
            if (len(testData)!=len(origData)):
                logging.error('Test and Original data length mismatch!')
                exit()
            # Positive Test Data with Orig Labels
            filterData = []

            for idx, elem in enumerate(testData):
                if (elem[0] != origData[idx][0]):
                    logging.error('Test and Original data mismatch!')
                    logging.error('Test Data: {}'.elem[0])
                    logging.error('Orig Data: {}'.origData[idx][0])
                    exit()
                if (elem[1] == str(1)):
                    filterData.append([elem[0], origData[idx][1]])
            trainingSamples+= filterData

        with open(outfile, 'w') as f:
            f.write("{} {} {}\n".format(len(trainingSamples), (window*window), 1))
            for dat in trainingSamples:
                f.write("{}\n{}\n".format(dat[0], dat[1]))

    finally:
        # shutil.rmtree(rgbDir)
        logging.error('Okay')



def collect(path, outfile, window, size, pRatio, extensive=False):

    try:
        shell(["make","clean"])
        defineStr = "DEFINES=\"-DDEFSIZE="+str(window)+"\""
        shell(["make", defineStr])
    except:
        logging.error('Compiling face detection program failed')
        exit()

    logging.info('Compiled face detection program successfully')

    # Collect input sources
    jpegs = []
    if (os.path.isfile(path)):
        # If it's a file
        jpegs.append(path)
        logging.info('Added image {}'.format(path))
    elif (os.path.isdir(path)):
        # If it's a directory
        for root, dirnames, filenames in os.walk(path):
          for filename in fnmatch.filter(filenames, '*.jpg'):
            jpegs.append(os.path.join(root, filename))
        logging.info('Found {} images in {}'.format(len(jpegs), path))
    else:
        # Path invalid
        logging.error('Path invalid: {}'.format(path))
        exit()

    # Temporary directory for temporary files
    try:
        rgbDir = tempfile.mkdtemp()+'/'
        logging.debug('New directory created: {}'.format(rgbDir))

        trainingSamples = []
        for jpeg in jpegs:
            # Convert jpgs to rgbs
            fn = os.path.basename(jpeg)
            rgb = rgbDir+os.path.splitext(fn)[0]+".rgb"
            logging.debug('Converting {} to rgb format'.format(fn))
            jpgToRgb(jpeg, rgb)
            logging.debug('Conversion successful: {}'.format(rgb))

            # Now perform neural network training
            testFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]
            try:
                logging.debug('Running face detection and data collection on {}'.format(dataFile))
                shell(["./detect", rgb, dataFile])
            except:
                logging.error('Face detection on {} failed'.format(rgb))
                exit()
            logging.debug('Face detection successful!')

            trainingSamples += process(dataFile, pRatio)
            if size and not extensive:
                if len(trainingSamples) >= size:
                    break

        random.shuffle(trainingSamples)

        if (size):
            trainingSamples = trainingSamples[0:size]

        with open(outfile, 'w') as f:
            f.write("{} {} {}\n".format(len(trainingSamples), (window*window), 1))
            for dat in trainingSamples:
                f.write("{}\n{}\n".format(dat[0], dat[1]))

    finally:
        shutil.rmtree(rgbDir)

def merge(pos, neg, pRatio):

    mergedData = []

    total = len(neg)+len(pos)
    logging.debug('There are {} pos, {} neg, {} total'.format(len(pos), len(neg), total))
    # Do we have enough positive samples
    if len(pos) >= pRatio*total:
        mergedData += neg
        numPos = int(len(neg)/(1-pRatio)) - len(neg)
        mergedData += pos[:numPos]
        logging.debug('Using {} pos, {} neg samples'.format(numPos, len(neg), total))
    else:
        mergedData += pos
        numNeg = int(len(pos)/pRatio) - len(pos)
        mergedData += neg[:numNeg]
        logging.debug('Using {} pos, {} neg samples'.format(len(pos), numNeg, total))

    random.shuffle(mergedData)
    return mergedData

def process(dataFile, pRatio=None):
    if pRatio==None:
        dataFile = dataFile
        data = []
        with open(dataFile) as f:
            while True:
                inputLine = f.readline()
                outputLine = f.readline()
                if not outputLine: break #EOF
                data.append([inputLine.strip(), outputLine.strip()])
        return data
    else:
        pDataFile = dataFile+'.pos.data'
        nDataFile = dataFile+'.neg.data'

        pData = []
        nData = []

        for dataSet in [[pDataFile, pData, 1], [nDataFile, nData, 0]]:
            with open(dataSet[0]) as f:
                for line in f:
                    dataSet[1].append([line.strip(), dataSet[2]])

        random.shuffle(pData)
        random.shuffle(nData)

        logging.debug("Obtained {} positive samples, and {} negative samples".format(len(pData), len(nData)))

        if pRatio==-1:
            mergedData = pData+nData
        else:
            mergedData = merge(pData, nData, pRatio)

        return mergedData

def cli():
    parser = argparse.ArgumentParser(
        description='Face detection training data collection script'
    )
    parser.add_argument(
        '-path', dest='imdir', action='store', type=str, required=False,
        default=".", help='image dataset directory'
    )
    parser.add_argument(
        '-w', dest='window', action='store', type=int, required=False,
        default=20, help='window size of the training data (default 20)'
    )
    parser.add_argument(
        '-size', dest='size', action='store', type=int, required=False,
        default=None, help='data size'
    )
    parser.add_argument(
        '-pRatio', dest='pRatio', action='store', type=float, required=False,
        default=0.5, help='ratio of positive samples [0.0-1.0]; if set to -1 takes all samples'
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
    parser.add_argument(
        '-extensive', dest='extensive', action='store_true', required=False,
        default=False, help='process all files in dataset and pick random subset (extends runtime)'
    )
    parser.add_argument(
        '-testCascade', dest='testCascade', action='store', required=False,
        default=None, help='path to cascade to test'
    )
    args = parser.parse_args()

    # Take care of log formatting
    logFormatter = logging.Formatter("%(asctime)s [%(threadName)-12.12s] [%(levelname)-5.5s]  %(message)s", datefmt='%m/%d/%Y %I:%M:%S %p')
    rootLogger = logging.getLogger()

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

    if (args.testCascade!=None):
        test(args.imdir, args.outfile, args.window, args.testCascade)
    else:
        collect(args.imdir, args.outfile, args.window, args.size, args.pRatio, args.extensive)


if __name__ == '__main__':
    cli()
