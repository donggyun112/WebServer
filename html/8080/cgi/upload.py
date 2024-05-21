#!/usr/bin/env python3

import cgi
import cgitb
import os

# Enable CGI traceback for debugging
cgitb.enable()

# Directory to store uploaded images
UPLOAD_DIR = "/Users/parkjunseo/42/webserv/html/8080/uploads"  # 여기를 적절한 디렉토리 경로로 변경하세요

# Ensure the upload directory exists
if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

class FileControl:
    def __init__(self, form):
        self.form = form

    def handleUpload(self):
        # Check if the file was uploaded
        if 'file' in self.form:
            file_item = self.form['file']
            # Check if the file is not empty
            if file_item.filename:
                # Securely create a full file path
                filename = os.path.basename(file_item.filename)
                filepath = os.path.join(UPLOAD_DIR, filename)
                
                # Write the file to the specified directory
                with open(filepath, 'wb') as f:
                    f.write(file_item.file.read())
                
                self.send_response(200, f"File '{filename}' uploaded successfully.")
            else:
                self.send_response(400, "No file was uploaded.")
        else:
            self.send_response(400, "No file field in form.")

    def send_response(self, status_code, message):
        print("HTTP/1.1 200 OK")
        print("Content-Type: text/html\n")
        print("<html><body>")
        print(f"<h2>{message}</h2>")
        print("</body></html>")

    def get_status_message(self, status_code):
        return {
            400: "Bad Request",
            200: "OK"
        }.get(status_code, "Unknown Status")

def handlePost():
    contentType = os.environ.get('CONTENT_TYPE')
    form = cgi.FieldStorage()
    if contentType and contentType.startswith("multipart/form-data"):
        listing = FileControl(form)
        listing.handleUpload()
    else:
        send_response(400, "Bad Request")

def send_response(status_code, message):
    print(f"Status: {status_code} {get_status_message(status_code)}")
    print("Content-Type: text/html\n")
    print("<html><body>")
    print(f"<h2>{message}</h2>")
    print("</body></html>")

def get_status_message(status_code):
    return {
        400: "Bad Request",
        200: "OK"
    }.get(status_code, "Unknown Status")

if __name__ == "__main__":
    handlePost()
