#!/usr/bin/python

"""
Completely build the game
"""

import os
import shutil
from build_assets import *


def build(targetDir):
    if os.path.exists(targetDir):
        print("Removing already existing " + targetDir)
        shutil.rmtree(targetDir)
        os.makedirs(targetDir)
    # build assets
    buildAssets("../data/", targetDir + "/data/")
    # if Windows, copy DLLs and mr.exe
    if (os.name == 'nt'):
        copyFolder("dll", targetDir)
        shutil.copy("mr.exe", targetDir + "mr.exe")
    else:
        shutil.copy("mr", targetDir + "mr")
    # make zip file
    shutil.make_archive("mr", "zip", targetDir)

if __name__ == '__main__':
    build("mr-build/")
