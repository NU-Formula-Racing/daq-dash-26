# run okay where to get engine root
ENGINE_ROOT=$(okay where | tr -d '\n')

python scripts/rpi_update.py --engine "$ENGINE_ROOT"