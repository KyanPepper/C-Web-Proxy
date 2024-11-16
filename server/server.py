import http.server
import socketserver

PORT = 8080  #Port to listen on

class Server(http.server.SimpleHTTPRequestHandler):
    protocol_version = 'HTTP/1.0'  # Use HTTP/1.0 

    def log_message(self, format, *args):
        print(f"Received request: {format % args}")


# Create an object of the above class
with socketserver.TCPServer(("", PORT),Server ) as httpd:
    print(f"Serving on port {PORT}")
    httpd.serve_forever()
