#! /usr/bin/env python

import os
import sys
import subprocess
import select
from optparse import OptionParser


# Setup of the command-line arguments parser
text = "Usage: %prog [options] <root-folder>\n\nConvert (in-place) all the BLP files in <root-folder> and its subdirectories"
parser = OptionParser(text, version="%prog 1.0")
parser.add_option("--converter", action="store", default="BLPConverter", type="string",
                  dest="converter", metavar="CONVERTER",
                  help="Path to the BLPConverter executable")
parser.add_option("--remove", action="store_true", default=False,
                  dest="remove", help="Remove the BLP files successfully converted")
parser.add_option("--verbose", action="store_true", default=False,
                  dest="verbose", help="Verbose output")

# Handling of the arguments
(options, args) = parser.parse_args()

# Check the parameters
if len(args) != 1:
    print "No root folder provided"
    sys.exit(-1)

root_folder = args[0]
if root_folder[-1] != os.path.sep:
    root_folder += os.path.sep

try:
    subprocess.Popen('%s --help' % options.converter, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
except:
    print "Can't execute BLPConverter at '%s'" % options.converter
    sys.exit(-1)
    

# Walk the root folder
counter_success_total = 0
failed_total = []
for root, dirs, files in os.walk(root_folder):
    if root == root_folder:
        print "Processing '.'..."
    else:
        print "Processing '%s'..." % root[len(root_folder):]

    blps = filter(lambda x: x.lower().endswith('.blp'), files)

    counter_failed = 0

    if len(blps) > 0:
        current = os.getcwd()
        os.chdir(root)

        to_convert = blps
        while len(to_convert) > 0:
            p = subprocess.Popen('%s %s' % (options.converter, ' '.join([ '"%s"' % image for image in to_convert[0:10] ])), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
            p.wait()
            output = p.stdout.read()
            
            failed = filter(lambda x: not(x.endswith(': OK')) and (len(x) > 0), output.split('\n'))
            counter_failed += len(failed)

            failed_total.extend(failed)

            if options.verbose:
                print '    * ' + output[:-1].replace('\n', '\n    * ')

            if options.remove:
                failed2 = map(lambda x: x[0:x.find(':')], failed)
                done = filter(lambda x: (x not in failed2) and (len(x) > 0), to_convert[0:10])
                p = subprocess.Popen('rm -f %s' % (' '.join([ '"%s"' % image for image in done ])), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
                p.wait()

            to_convert = to_convert[10:]

        os.chdir(current)

    if counter_failed > 0:
        print '%d images converted, %d images not converted' % (len(blps) - counter_failed, counter_failed)
    else:
        print '%d images converted' % (len(blps) - counter_failed)
    print

    counter_success_total += len(blps) - counter_failed

print '----------------------------------------------------------'

if len(failed_total) > 0:
    print 'TOTAL: %d images converted, %d images not converted' % (counter_success_total, len(failed_total))
    print
    print 'Images not converted:'
    for image in failed_total:
        print '    * ' + image
else:
    print 'TOTAL: %d images converted' % counter_success_total
