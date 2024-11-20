import http.server
import socketserver

PORT = 8081 #Port to listen on

backend_url = "http://localhost:8081/test1.txt" # URL of my python server file

class Server(http.server.SimpleHTTPRequestHandler):
    protocol_version = 'HTTP/1.0' 

    def log_message(self, format, *args):
        print(f"Received request: {format % args}")


# Create an object of the above class
with socketserver.TCPServer(("", PORT),Server ) as httpd:
    print(f"Serving on port {PORT}")
    httpd.serve_forever()
