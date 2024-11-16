import pytest
import requests

# Server must be running before running the tests on port 8080

url = "http://localhost:8080"


def test_health_check():
    # Send a GET request to the server
    print(f"Sending GET request to {url}")
    response = requests.get(f"{url}/test1.txt")

    # Assert the server responds correctly
    assert response.status_code == 200
    assert response.content == b"abc123"
