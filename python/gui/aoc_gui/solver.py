from .paths import BINDINGS_DIR
import requests
import sys

sys.path.append(BINDINGS_DIR)

import aoc


def solve(year, day, input):
    return aoc.solve(year, day, input)
