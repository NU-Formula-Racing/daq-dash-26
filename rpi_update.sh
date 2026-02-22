# run okay where to get engine root
ENGINE_ROOT="$(okay where | tr -d '\r' | sed -e 's/[[:space:]]*$//')"
echo "$ENGINE_ROOT"
python scripts/rpi_update.py --engine $ENGINE_ROOT --release