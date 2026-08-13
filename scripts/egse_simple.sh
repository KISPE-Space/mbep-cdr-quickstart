#!/bin/bash

git clone https://github.com/KISPE-Space/mbep-cdr-quickstart.git ~/egse

cd ~/egse

tar -xzf mbep-cdr-yamcs-0.0.1-CDR-bundle.tar.gz -C ~/egse

cd ~/egse/mbep-cdr-yamcs-0.0.1-CDR && ./bin/yamcsd #> /tmp/yamcs.log 2>&1 &

# cd ~/egse/scripts/satll-simulator

# ./satll-sim
