#!/bin/bash

set -o errexit

REPO_DIR="${HOME}/egse" # Temporary Setup will adapt to taste
BUNDLE_NAME="mbep-cdr-yamcs-0.0.1-CDR"

# Stopping any running instances of YAMCS
# YAMCS Can only be ran as once instanve at a time due to shared resources

echo "[-] Stopping any existing yamcsd instances"
pkill -f org.yamcs.YamcsServer || true
sleep 3
if pgrep -f org.yamcs.YamcsServer > /dev/null; then
    echo "[-] yamcsd still running, force killing"
    pkill -9 -f org.yamcs.YamcsServer
fi

echo "[-] Stopping any existing satll-sim instances"
pkill -f satll-sim || true

# Give processes a moment to actually shut down before starting new ones
sleep 1

echo "[-] Starting yamcsd"
cd "${REPO_DIR}/target/${BUNDLE_NAME}" && ./bin/yamcsd > /tmp/yamcsd.log 2>&1 &


# Give time for the socket to be initialised by YAMCS
sleep 3
echo "[-] Starting satll-sim"
#cd "${REPO_DIR}/scripts/satll-simulator" && ./satll-sim > /tmp/simulator.log 2>&1 &
cd "${REPO_DIR}/scripts/satll-simulator" && stdbuf -o0 -e0 ./satll-sim > /tmp/simulator.log 2>&1 & # Needed to run with buffer changes

echo "[-] Both processes started in background"
echo "    yamcsd log:      /tmp/yamcsd.log"
echo "    satll-sim log:   /tmp/simulator.log"

gnome-terminal -- tail -f /tmp/yamcsd.log
gnome-terminal -- tail -f /tmp/simulator.log

wait

