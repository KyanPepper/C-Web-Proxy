#!/bin/bash

# Change to the working directory
cd webproxy

make 

# Run the web proxy on port 8080
./proxy 8080
