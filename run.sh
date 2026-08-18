#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

cmake --build build -j"$(nproc)"

"$DYNAMORIO_ROOT/bin64/drrun" -root "$DYNAMORIO_ROOT" -debug \
  -logdir logs -loglevel 5 -logmask 0x20 \
  -c "$PWD/build/libregspill_mre.so" -- "$PWD/build/cbr_stolen"
