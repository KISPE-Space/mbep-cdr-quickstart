#!/bin/bash

set -o errexit

#Variables
REPO_URL="https://github.com/KISPE-Space/mbep-cdr-quickstart.git"
BUNDLE_NAME="mbep-cdr-yamcs-0.0.1-CDR-bundle.tar.gz"

REPO_DIR="${HOME}/egse" # Temporary Setup will adapt to taste


#
#   Adapted YAMCS Install
#
echo "[-] Downloading YAMCS"
echo "[-] Installing at ${REPO_DIR}"
rm -rf "${REPO_DIR}"
git clone "${REPO_URL}" "${REPO_DIR}"

cd "${REPO_DIR}"

# Checks if the .tar bundle exists
if [ ! -f "${BUNDLE_NAME}" ]; then
    echo "[-] ERROR: Could not find ${BUNDLE_NAME} at ${REPO_DIR}"
    echo "    Make sure this script is being run from inside the cloned repo."
    exit 1
fi

# extract the bundle
echo "[-] Extracting ${BUNDLE_NAME}"
mkdir -p "${REPO_DIR}/target"
tar -xzvf "${BUNDLE_NAME}" -C "${REPO_DIR}/target"

echo "[-] Extracting ${BUNDLE_NAME}"

echo "[-] YAMCS Installed"
echo "[-] To Run YAMCS run scripts/.run-yamcs.sh"