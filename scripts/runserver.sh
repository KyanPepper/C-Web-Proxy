#!/bin/bash 

# Change to the working directory
cd server

# Create a Python virtual environment
python3 -m venv venv

# Activate the virtual environment
source venv/bin/activate

# Install the required packages
pip install -r requirements.txt

# Run the Python server
python3 server.py $0
