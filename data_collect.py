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
OCV_CASCADE="xml/ocv_clsfr.xml"

def shell(command, cwd=None, shell=False):
    return subprocess.check_output(
        command,
        cwd=cwd,
        stderr=subprocess.STDOUT,
        shell=shell,
    )

# def jpgToRgb(inPath, outPath):
#     jpg = Image.open(inPath)
#     pix = jpg.load()
#     rgb = open(outPath, "w")

#     rgb.write(str(jpg.size[0]) + "," + str(jpg.size[1]) + "\n")

#     for y in range(0, jpg.size[1]):
#         rgb.write(str(pix[0, y][0]) + "," + str(pix[0, y][1]) + "," + str(pix[0, y][2]))
#         for x in range(1, jpg.size[0]):
#             rgb.write("," + str(pix[x, y][0]) + "," + str(pix[x, y][1]) + "," + str(pix[x, y][2]))
#         rgb.write("\n")

def jpgToRgb(inPath, outPath):
    jpg = Image.open(inPath)
    pix = jpg.load()
    rgb = open(outPath, "w")
    rgb.write(str(jpg.size[0]) + "," + str(jpg.size[1]) + "\n")

    if jpg.mode == 'L':
        for y in range(0, jpg.size[1]):
            rgb.write(str(pix[0, y]) + "," + str(pix[0, y]) + "," + str(pix[0, y]))
            for x in range(1, jpg.size[0]):
                rgb.write("," + str(pix[x, y]) + "," + str(pix[x, y]) + "," + str(pix[x, y]))
            rgb.write("\n")
    else:
        for y in range(0, jpg.size[1]):
            rgb.write(str(pix[0, y][0]) + "," + str(pix[0, y][1]) + "," + str(pix[0, y][2]))
            for x in range(1, jpg.size[0]):
                rgb.write("," + str(pix[x, y][0]) + "," + str(pix[x, y][1]) + "," + str(pix[x, y][2]))
            rgb.write("\n")

def compileExecutable(window):
    defineStr = "DEFINES=\"-DDEFSIZE="+str(window)
    defineStr += " -DCOMMON_DUMP=1"
    defineStr += " -DDRAW_PROB=1"
    defineStr += "\""
    print defineStr
    shell(["make", "clean"])
    shell(["make", defineStr])


def test(path, outfile, window, testClassifier):

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

    # Stats
    stats = {
        "true_pos": 0,
        "true_neg": 0,
        "false_pos": 0,
        "false_neg": 0,
    }

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

            # Now perform data collection on both cascades:
            origFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]+'_orig'
            testFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]+'_test'
            # Collect original labels
            try:
                shell(["make", "clean"])
                compileExecutable(window)
                logging.debug('Running face detection and data collection on {} with original cascade'.format(origFile))
                shell(["./detect", rgb, OCV_CASCADE, origFile])
            except:
                logging.error('Face detection on {} failed'.format(rgb))
                exit()
            logging.debug('Face detection successful!')
            origData = process(origFile)
            # Collect test labels
            try:
                shell(["make", "clean"])
                compileExecutable(window)
                logging.debug('Running face detection and data collection on {} with test cascade {}'.format(testFile, testClassifier))
                shell(["./detect", rgb, testClassifier, testFile])
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
            for idx, dat in enumerate(testData):
                tst_data = dat[0]
                ref_data = origData[idx][0]
                tst_label = dat[1]
                ref_label = origData[idx][1]
                if (tst_data != ref_data):
                    logging.error('Test and Original data mismatch!')
                    logging.error('Test Data: {}'.dat[0])
                    logging.error('Orig Data: {}'.origData[idx][0])
                    exit()
                # Update Stats
                if (tst_label==str(1) and ref_label==str(1)):
                    stats["true_pos"] += 1
                if (tst_label==str(0) and ref_label==str(0)):
                    stats["true_neg"] += 1
                if (tst_label==str(1) and ref_label==str(0)):
                    stats["false_pos"] += 1
                if (tst_label==str(0) and ref_label==str(1)):
                    stats["false_neg"] += 1

                # Check label
                if (tst_label == str(1)):
                    filterData.append([tst_data, ref_label])

            trainingSamples+= filterData

            # Cleanup files
            os.remove(origFile)
            os.remove(testFile)

        # Print stats
        if ((stats["true_pos"]+stats["false_pos"])>0):
            precision = float(stats["true_pos"])/(stats["true_pos"]+stats["false_pos"])
        else:
            precision = -1
        if (stats["true_pos"]+stats["false_neg"]):
            recall = float(stats["true_pos"])/(stats["true_pos"]+stats["false_neg"])
        else:
            recall = -1
        logging.info("Raw stats = {}".format(stats))
        logging.info("Precision = {}".format(precision))
        logging.info("Recall = {}".format(recall))

        with open(outfile, 'w') as f:
            f.write("{} {} {}\n".format(len(trainingSamples), (window*window), 1))
            for dat in trainingSamples:
                f.write("{}\n{}\n".format(dat[0], dat[1]))

    finally:
        shutil.rmtree(rgbDir)


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
                shell(["./detect", rgb, OCV_CASCADE,dataFile])
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
        '-testClassifier', dest='testClassifier', action='store', required=False,
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

    if (args.testClassifier!=None):
        test(args.imdir, args.outfile, args.window, args.testClassifier)
    else:
        collect(args.imdir, args.outfile, args.window, args.size, args.pRatio, args.extensive)


if __name__ == '__main__':
    cli()
