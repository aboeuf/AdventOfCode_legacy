#!/usr/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
UI_DIR="$SCRIPT_DIR/aoc_gui/ui"
rm -f "$UI_DIR"/ui_*.py

source "$SCRIPT_DIR/venv/bin/activate"

for ui_file in "$UI_DIR"/*.ui; do
    [ -e "$ui_file" ] || continue
    filename=$(basename "$ui_file" .ui)
    output_file="$UI_DIR/ui_$filename.py"
    echo "Compiling $filename.ui to ui_$filename.py"
    pyside6-uic "$ui_file" -o "$output_file"
done

deactivate
