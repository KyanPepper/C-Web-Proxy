# Change to the working directory
cd server

# Create a Python virtual environment
python3 -m venv

# Activate the virtual environment
source venv/bin/activate

# Run the Python server
pytest test.py
