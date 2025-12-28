#!/usr/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/venv/bin/activate
export PYTHONPATH=$SCRIPT_DIR
python -m aoc_gui.main
deactivate
