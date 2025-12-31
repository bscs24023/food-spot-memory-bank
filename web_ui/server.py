#!/usr/bin/env python3
"""
Food Spot Memory Bank - Web UI Server
Combines HTTP server + Socket client in one file
"""

import socket
import json
import threading
from http.server import HTTPServer, SimpleHTTPRequestHandler
import os
import sys

class CPPBackend:
    
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
        self.sock = None
    
    def connect(self):
     
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))
        print(f" Connected to C++ backend at {self.host}:{self.port}")
    
    def send_command(self, command):

        try:
            if not self.sock:
                self.connect()
            
            self.sock.sendall(command.encode())
            response = self.sock.recv(4096).decode()
            
            try:
                return json.loads(response)
            except:
                return {"status": "error", "message": response}
                
        except Exception as e:
            print(f"Socket error: {e}")
            self.sock = None
            return {"status": "error", "message": str(e)}
    
    def close(self):
        if self.sock:
            self.sock.close()

class FoodSpotHandler(SimpleHTTPRequestHandler):
    
    cpp_backend = CPPBackend()
    
    def do_GET(self):
 
        if self.path == '/' or self.path == '/index.html':
            self.serve_html('index.html')
        elif self.path == '/style.css':
            self.serve_css('style.css')
        elif self.path.startswith('/api/'):
            self.handle_api_request()
        else:
          
            super().do_GET()
    
    def do_POST(self):
   
        if self.path.startswith('/api/'):
            self.handle_api_request()
        else:
            self.send_error(404)
    
    def serve_html(self, filename):
        try:
            with open(filename, 'rb') as f:
                content = f.read()
            
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.send_header('Content-length', len(content))
            self.end_headers()
            self.wfile.write(content)
            
        except FileNotFoundError:
            self.send_error(404, f"File {filename} not found")
    
    def serve_css(self, filename):
        try:
            with open(filename, 'rb') as f:
                content = f.read()
            
            self.send_response(200)
            self.send_header('Content-type', 'text/css')
            self.send_header('Content-length', len(content))
            self.end_headers()
            self.wfile.write(content)
            
        except FileNotFoundError:
            self.send_error(404)
    
    def handle_api_request(self):
        api_path = self.path[5:]
        
        if self.command == 'GET':
            if '?' in api_path:
                path, query = api_path.split('?', 1)
                params = self.parse_query(query)
            else:
                path = api_path
                params = {}
        else:
            path = api_path
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            params = json.loads(post_data.decode())
        
        response = self.route_request(path, params)

        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(json.dumps(response).encode())
    
    def parse_query(self, query_string):
        params = {}
        if query_string:
            pairs = query_string.split('&')
            for pair in pairs:
                if '=' in pair:
                    key, value = pair.split('=', 1)
                    params[key] = value
        return params
    
    def route_request(self, path, params):
        """Route API requests to appropriate handlers"""
        
        if path == 'login':
            return self.handle_login(params)
        elif path == 'register':
            return self.handle_register(params)
        elif path == 'add_restaurant':
            return self.handle_add_restaurant(params)
        elif path == 'get_restaurants':
            return self.handle_get_restaurants(params)
        elif path == 'get_friends':
            return self.handle_get_friends(params)
        elif path == 'add_friend':
            return self.handle_add_friend(params)
        elif path == 'get_alerts':
            return self.handle_get_alerts(params)
        elif path == 'mark_alerts_read':
            return self.handle_mark_alerts_read(params)
        elif path == 'get_recommendations':
            return self.handle_get_recommendations(params)
        elif path == 'search_cuisine':
            return self.handle_search_cuisine(params)
        elif path == 'search_location':
            return self.handle_search_location(params)
        elif path == 'search_rating':
            return self.handle_search_rating(params)
        elif path == 'search_price':
            return self.handle_search_price(params)
        else:
            return {"status": "error", "message": "Unknown API endpoint"}

    def handle_mark_alerts_read(self, params):
        user_id = params.get('userID', '')
        
        if not user_id:
            return {"status": "error", "message": "User ID required"}
        
        cmd = f"MARK_ALERTS_READ {user_id}"
        return self.cpp_backend.send_command(cmd)
    
    
    def handle_login(self, params):
        username = params.get('username', '')
        password = params.get('password', '')
        
        cmd = f"LOGIN {username} {password}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_register(self, params):
        username = params.get('username', '')
        email = params.get('email', '')
        password = params.get('password', '')
        
        cmd = f"REGISTER {username} {email} {password}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_add_restaurant(self, params):
        user_id = params.get('userID', '')
        name = params.get('name', '').replace(' ', '_')
        location = params.get('location', '').replace(' ', '_')
        cuisine = params.get('cuisine', '').replace(' ', '_')
        rating = params.get('rating', '0')
        price = params.get('price', '0')
        notes = params.get('notes', '').replace(' ', '_')
        
        cmd = f"ADD_RESTAURANT {user_id} {name} {location} {cuisine} {rating} {price} {notes}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_get_restaurants(self, params):
        user_id = params.get('userID', '')
        cmd = f"GET_RESTAURANTS {user_id}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_get_friends(self, params):
        user_id = params.get('userID', '')
        cmd = f"GET_FRIENDS {user_id}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_add_friend(self, params):
        user_id = params.get('userID', '')
        friend_name = params.get('username', '')
        cmd = f"ADD_FRIEND {user_id} {friend_name}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_get_alerts(self, params):
        user_id = params.get('userID', '')
        cmd = f"GET_ALERTS {user_id}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_get_recommendations(self, params):
        user_id = params.get('userID', '')
        cmd = f"GET_RECOMMENDATIONS {user_id}"
        return self.cpp_backend.send_command(cmd)
    
    def handle_search_cuisine(self, params):
        user_id = params.get('userID', '')
        cuisine = params.get('cuisine', '').replace(' ', '_')
        
        if not user_id:
            return {"status": "error", "message": "User ID required"}
        if not cuisine:
            return {"status": "error", "message": "Cuisine required"}
        
        cmd = f"SEARCH_CUISINE {user_id} {cuisine}"
        print(f" Search cuisine command: {cmd}")
        return self.cpp_backend.send_command(cmd)

    def handle_search_location(self, params):
        user_id = params.get('userID', '')
        location = params.get('location', '').replace(' ', '_')
        
        if not user_id:
            return {"status": "error", "message": "User ID required"}
        if not location:
            return {"status": "error", "message": "Location required"}
        
        cmd = f"SEARCH_LOCATION {user_id} {location}"
        print(f" Search location command: {cmd}")
        return self.cpp_backend.send_command(cmd)

    def handle_search_rating(self, params):
        user_id = params.get('userID', '')
        min_rating = params.get('minRating', '0')
        max_rating = params.get('maxRating', '10')
        
        if not user_id:
            return {"status": "error", "message": "User ID required"}
        
        cmd = f"SEARCH_RATING {user_id} {min_rating} {max_rating}"
        print(f" Search rating command: {cmd}")
        return self.cpp_backend.send_command(cmd)

    def handle_search_price(self, params):
        user_id = params.get('userID', '')
        min_price = params.get('minPrice', '0')
        max_price = params.get('maxPrice', '10000')
        
        if not user_id:
            return {"status": "error", "message": "User ID required"}
        
        cmd = f"SEARCH_PRICE {user_id} {min_price} {max_price}"
        print(f" Search price command: {cmd}")
        return self.cpp_backend.send_command(cmd)

def start_server(port=5000):
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    server_address = ('', port)
    httpd = HTTPServer(server_address, FoodSpotHandler)
    
    print("  FOOD SPOT MEMORY BANK - WEB UI")
    print(f"HTTP Server: http://localhost:{port}")
    print(f"C++ Backend: localhost:8080")
    print("\nOpen your browser and go to: http://localhost:5000")
    print("Press Ctrl+C to stop the server\n")
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n\nServer stopped.")
    finally:
        FoodSpotHandler.cpp_backend.close()

if __name__ == '__main__':
    start_server()