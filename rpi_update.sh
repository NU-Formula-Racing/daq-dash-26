# run okay where to get engine root
ENGINE_ROOT=$(okay where | tr -d '\n')
echo "$ENGINE_ROOT"
python scripts/rpi_update.py --engine "$ENGINE_ROOT" --clear --dry-run