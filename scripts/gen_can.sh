#!/usr/bin/env bash

echo "Generating can messages"

CAN_LIB_LOC="./dash/drivers/can"
OUTPUT_FILE="./dash/can/can_dbc.hpp"
CAN_GEN_PY="$CAN_LIB_LOC/scripts/can_gen.py"
CAN_DBC="nfr26_dbc.csv"

# make the virtual enviornment, if needed
if [ ! -d "$CAN_LIB_LOC/scripts/venv" ]; then
    python3 -m venv "$CAN_LIB_LOC/scripts/venv"
fi

# activate the virtual environment
source "$CAN_LIB_LOC/scripts/venv/Scripts/Activate"

# install requirements
pip install -r "$CAN_LIB_LOC/scripts/requirements.txt"

# make the output directory
mkdir -p "$(dirname "$OUTPUT_FILE")"

# run the script
python "$CAN_GEN_PY" -c camelCase -o "$OUTPUT_FILE" "$CAN_DBC"

# deactivate the virtual environment
deactivate

