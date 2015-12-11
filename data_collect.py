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

ERROR_MODE=3

LOG_FILE = "data_collect.log"
DATA_FILE = "out.data"
OCV_CASCADE="xml/ocv_clsfr.xml"
TEST_FILE = "test_faces.out"
REF_FILE = "ref_faces.out"

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

def compileExecutable(window, mode=ERROR_MODE):
    defineStr = "DEFINES=\"-DDEFSIZE="+str(window)
    defineStr += " -DDATA_COLLECT_MODE="+str(mode)
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

    refFaceData = ""
    testFaceData = ""

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
                compileExecutable(window)
                logging.debug('Running face detection and data collection on {} with original cascade'.format(origFile))
                shell(["./detect", rgb, OCV_CASCADE, origFile])
            except:
                logging.error('Face detection on {} failed'.format(rgb))
                exit()
            logging.debug('Face detection successful!')
            try:
                compileExecutable(window)
                logging.debug('Running face detection and data collection on {} with test cascade {}'.format(testFile, testClassifier))
                shell(["./detect", rgb, testClassifier, testFile])
            except:
                logging.error('Face detection on {} failed'.format(rgb))
                exit()
            logging.debug('Face detection successful!')

            if ERROR_MODE==3:
                origFaces = processFaces(origFile)
                testFaces = processFaces(testFile)
                statistics = measureOverlap(origFaces, testFaces)
                stats["true_pos"] += statistics["true_pos"]
                stats["false_pos"] += statistics["false_pos"]
                stats["false_neg"] += statistics["false_neg"]
                refFaceData += "=="+os.path.basename(jpeg)+"==\n"
                for idx, face in enumerate(origFaces):
                    refFaceData +="{}: {}, {}, {}, {}\n".format(idx, face[0], face[1], face[2], face[3])
                refFaceData += "\n"
                testFaceData += "=="+os.path.basename(jpeg)+"==\n"
                for idx, face in enumerate(testFaces):
                    testFaceData +="{}: {}, {}, {}, {}\n".format(idx, face[0], face[1], face[2], face[3])
                testFaceData += "\n"


            if ERROR_MODE==1:
                # Read in data files
                origData = process(origFile)
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
                        logging.error('Test and Original data mismatch at index {}!'.format(idx))
                        logging.error('Test Data: {}'.format(tst_data))
                        logging.error('Orig Data: {}'.format(ref_data))
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

        with open(REF_FILE, 'w') as f:
            f.write(refFaceData)
        with open(TEST_FILE, 'w') as f:
            f.write(testFaceData)

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

            # Now run Viola Jones for data collection
            dataFile = rgbDir+os.path.splitext(os.path.basename(rgb))[0]
            try:
                logging.debug('Running face detection and data collection on {}'.format(dataFile))
                shell(["./detect", rgb, OCV_CASCADE, dataFile])
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

def processFaces(dataFile):
    faces = []
    with open(dataFile) as f:
        for line in f:
            faces.append([int(x) for x in line.strip().split(" ")])
    return faces

def overlap(face1, face2):
    r1x1 = face1[0]
    r1y1 = face1[1]
    r1x2 = face1[2] + face1[0]
    r1y2 = face1[3] + face1[1]
    r2x1 = face2[0]
    r2y1 = face2[1]
    r2x2 = face2[2] + face2[0]
    r2y2 = face2[3] + face2[1]

    intersection = max(0, min(r1x2, r2x2) - max(r1x1, r2x1)) * max(0, min(r1y2, r2y2) - max(r1y1, r1y1))
    union = face1[2]*face1[3] + face2[2]*face2[3] - intersection
    result = float(intersection)/float(union)

    return result

def measureOverlap(origFaces, testFaces):
    logging.info("Found {} faces with reference classifier!".format(len(origFaces)))
    logging.info("Found {} faces with test classifier!".format(len(testFaces)))
    logging.debug("Raw reference face data: {}!".format(origFaces))
    logging.debug("Raw test face data: {}!".format(testFaces))

    faces_to_find = 0
    truePos = 0

    remove_face = False
    for origFace in origFaces:
        faces_to_find = faces_to_find + 1
        for testFace in testFaces:
            score = overlap(origFace,testFace)
            if score > .5:
                truePos = truePos + 1
                testFaces.remove(testFace)
                logging.debug("Found face!")
                logging.debug("Orig: {}".format(origFace))
                logging.debug("Test: {}".format(testFace))
                logging.debug("Overlap score: {}".format(score))
                break

    # Collect Statistics
    falsePos = len(testFaces)
    falseNeg = len(origFaces)-truePos

    # Compute Precision and Recall
    if len(origFaces)>0:
        recall = (float) (truePos) / len(origFaces)
        precision = (float) (truePos) / (truePos + falsePos)
        logging.debug("Precision = {}".format(precision))
        logging.debug("Recall = {}".format(recall))

    return {"true_pos": truePos, "false_pos": falsePos, "false_neg": falseNeg}

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
