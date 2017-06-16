#!/bin/env python
import os
import glob
import sys
import argparse
from subprocess import PIPE, Popen
from threading import Thread
from Queue import Queue


parser = argparse.ArgumentParser()
parser.add_argument('cmd', type=str,nargs='+',
                    help='command files to process',
                    )
args = parser.parse_args()    #store mutilple cmd in list

queue = Queue()
for cmd in args.cmd:
    queue.put(cmd)

while not queue.empty():
    cmdline = queue.get()
    print cmdline

'''

while(True):
    pipe = Popen("ps -U ${USER} | awk '{print $4}'  | grep -c precut", shell=True, stdout=PIPE).stdout
    output = pipe.read()

    if int(output) is 0 :
        break

    os.system('sleep 5')

    sys.stdout.write('\r'+"[Status] Remaining files : "+output.strip())
    sys.stdout.flush()
'''
