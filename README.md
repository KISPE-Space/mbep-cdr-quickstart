# Yamcs QuickStart

This repository holds the source code to start a basic Yamcs application that monitors a simulated spacecraft in low earth orbit.

You may find it useful as a starting point for your own project.

## Documentation
Documentation for using and running the software tailored to the MBEP CDR are in `/documentation`.


## Prerequisites

* Java 17+
* Linux x64/aarch64, macOS x64/aarch64, or Windows x64

A copy of Maven is also required, however this gets automatically downloaded an installed by using the `./mvnw` shell script as detailed below.


## Bundling

Running through Maven is useful during development, but it is not recommended for production environments. Instead bundle up your Yamcs application in a tar.gz file:

    ./mvnw package
