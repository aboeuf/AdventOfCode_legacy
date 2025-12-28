import os


def get_parent_dirpath(path):
    return os.path.abspath(os.path.join(path, os.pardir))


PACKAGE_DIR = get_parent_dirpath(__file__)
ICONS_DIR = os.path.join(PACKAGE_DIR, "icons")
GUI_DIR = get_parent_dirpath(PACKAGE_DIR)
PYTHON_DIR = get_parent_dirpath(GUI_DIR)
PROJECT_DIR = get_parent_dirpath(PYTHON_DIR)
SOLVERS_DIR = os.path.join(PROJECT_DIR, "cpp_solvers/src")
BUILD_DIR = os.path.join(PROJECT_DIR, "build")
PY_BUILD_DIR = os.path.join(BUILD_DIR, "python")
BINDINGS_DIR = os.path.join(PY_BUILD_DIR, "bindings")


def get_absolute_path(filename):
    return os.path.join(PACKAGE_DIR, filename)
