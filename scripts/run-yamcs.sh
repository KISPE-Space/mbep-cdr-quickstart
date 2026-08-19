#!/bin/bash

set -o errexit

REPO_DIR="${HOME}/egse" # Temporary Setup will adapt to taste
BUNDLE_NAME="mbep-cdr-yamcs-0.0.1-CDR"

cd "${REPO_DIR}/target/${BUNDLE_NAME}" && ./bin/yamcsd




