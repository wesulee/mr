#!/usr/bin/python

"""
Build release assets from development assets.
"""

import json
import os
import shutil


# if json==True, minify all json
def copyFolder(src, dst, json=False):
    """copy files in src into dst"""
    # make sure dst directory exists
    if not os.path.exists(dst):
        os.makedirs(dst)
    for name in os.listdir(src):
        srcPath = os.path.join(src, name)
        if not os.path.isfile(srcPath):
            continue
        dstPath = os.path.join(dst, name)
        if os.path.exists(dstPath):
            print("Overwriting " + dstPath)
        if json and isJSON(srcPath):
            minifyJSON(srcPath, dstPath)
        else:
            shutil.copy(srcPath, dstPath)


# src is dev data path, dst is release data path
def buildAssets(src, dst):
    for d in ("", "fonts", "rooms"):
        copyFolder(os.path.join(src, d), os.path.join(dst, d), True)


def isJSON(path):
    return os.path.splitext(path)[1] == ".json"


def minifyJSON(src, dst):
    with open(src) as fsrc:
        with open(dst, "w") as fdst:
            json.dump(json.load(fsrc), fdst, separators=(',', ':'))


if __name__ == '__main__':
    buildAssets("../data/", "data/")
