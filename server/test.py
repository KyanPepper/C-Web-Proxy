import pytest
import requests

# Server must be running before running the tests on port 8081

serverURL = "http://localhost:8081"
proxies = {
    "http": "http://localhost:8080",  # Proxy server is on port 8080
    "https": "http://localhost:8080",
}


def test_health_check():
    # Send a GET request to the server
    print(f"Sending GET request to {serverURL}")
    response = requests.get(f"{serverURL}/test1.txt")

    # Assert the server responds correctly
    assert response.status_code == 200
    assert response.content == b"abc123"


def testGetOnLocalServerWithProxy():
    response = requests.get(f"{serverURL}/test1.txt", proxies=proxies)

    # Print the response status and content
    print(f"Status Code: {response.status_code}")
    print(f"Response Content: {response.content}")

    assert response.status_code == 200
    assert response.content == b"abc123"


def test_health_check_not_found():
    response = requests.get(f"{serverURL}/DNE.txt", proxies=proxies)
    assert response.status_code == 404


   
