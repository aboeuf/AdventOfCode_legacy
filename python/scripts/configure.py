#!/usr/bin/python3

import os
import shutil
import subprocess

def get_parent_dirpath(path):
    return os.path.abspath(os.path.join(path, os.pardir))

SCRIPTS_DIR = get_parent_dirpath(__file__)
PYTHON_DIR = get_parent_dirpath(SCRIPTS_DIR)
ROOT_DIR = get_parent_dirpath(PYTHON_DIR)
BUILD_DIR = os.path.join(ROOT_DIR, "build")

def main():
    if os.path.isdir(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
    os.mkdir(BUILD_DIR)
    subprocess.run(["cmake", ".."], cwd=BUILD_DIR)
    subprocess.run(os.path.join(SCRIPTS_DIR, "generate_qt_creator_files.py"))

if __name__ == "__main__":
    main()
