#!/bin/bash

set -o errexit

#Variables
REPO_URL="https://github.com/KISPE-Space/mbep-cdr-quickstart.git"
BUNDLE_NAME="mbep-cdr-yamcs-0.0.1-CDR-bundle.tar.gz"

REPO_DIR="${HOME}/egse" # Temporary Setup will adapt to taste

# Must be run as root
if [[ $EUID -ne 0 ]]; then
	echo "Error: must be ran as root"
	echo "Ubuntu: sudo $0"
	echo "Debian: su- $0"
	exit 1
fi

#
#	Java JDK Dependency
#

source /etc/os-release

if ! command -v javac >/dev/null 2>&1; then
	echo "java JDK not found"
	
	apt-get install -y default-jdk
else 
	echo "Java JDK Already Installed"
	javac -version
fi




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