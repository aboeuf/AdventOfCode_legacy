#!/usr/bin/python3

import os

INCLUDE_EXTENSIONS = ["h", "hpp"]
ALL_EXTENSIONS = [*INCLUDE_EXTENSIONS, "c", "cpp", "txt", "py", "gitignore", "ui", "sh"]

FILTERED_DIRECTORIES = ["build", ".git"]
QT_INCLUDE = "/usr/include/x86_64-linux-gnu/qt5"


def get_parent_dirpath(path):
    return os.path.abspath(os.path.join(path, os.pardir))


SCRIPTS_DIR = get_parent_dirpath(__file__)
PYTHON_DIR = get_parent_dirpath(SCRIPTS_DIR)
ROOT_DIR = get_parent_dirpath(PYTHON_DIR)
PROJECT_NAME = ROOT_DIR.split("/")[-1]


def get_all_children_directories(path):
    directories = []

    def recursion(current):
        nonlocal directories
        if os.path.isdir(current):
            if current not in directories:
                directories.append(current)
            for name in os.listdir(current):
                recursion(os.path.join(current, name))

    recursion(path)
    directories.sort()
    return directories


def get_paths():
    all_paths = [f"{PROJECT_NAME}.pro"]
    include_paths = get_all_children_directories(QT_INCLUDE)
    include_paths.append(ROOT_DIR)
    filtered = [os.path.join(ROOT_DIR, path) for path in FILTERED_DIRECTORIES]

    def add_filepath(filepath):
        nonlocal all_paths, include_paths
        extension = os.path.splitext(filepath)[1][1:]
        if extension not in ALL_EXTENSIONS:
            return
        if filepath not in all_paths:
            all_paths.append(filepath)
        if extension in INCLUDE_EXTENSIONS:
            current = get_parent_dirpath(filepath)
            while current != ROOT_DIR:
                if current not in include_paths:
                    include_paths.append(current)
                current = get_parent_dirpath(current)

    def get_paths_recursive(path):
        if os.path.isfile(path):
            add_filepath(path)
            return
        if not os.path.isdir(path) or path in filtered:
            return
        for name in os.listdir(path):
            if path != ROOT_DIR or not name.startswith(PROJECT_NAME):
                get_paths_recursive(os.path.join(path, name))

    get_paths_recursive(ROOT_DIR)
    all_paths.sort()
    include_paths.sort()
    return (all_paths, include_paths)


def main():
    all_paths, include_paths = get_paths()
    with open(os.path.join(ROOT_DIR, f"{PROJECT_NAME}.files"), "w") as file:
        for path in all_paths:
            file.write(path)
            file.write("\n")
        file.close()
    with open(os.path.join(ROOT_DIR, f"{PROJECT_NAME}.includes"), "w") as file:
        for path in include_paths:
            file.write(path)
            file.write("\n")
        file.close()


if __name__ == "__main__":
    main()
