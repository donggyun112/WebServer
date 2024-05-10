import os
import http.server
import socketserver
from cgi import FieldStorage
from urllib.parse import urlparse, parse_qs
import subprocess

PORT = 8000
UPLOAD_DIR = './uploaded'

class ServerHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        # `/upload.py` 경로 처리
        if self.path == '/upload.py':
            self.handle_upload()
        else:
            self.send_error(404, "Path not found")

    def handle_upload(self):
        form = FieldStorage(
            fp=self.rfile,
            headers=self.headers,
            environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': self.headers['Content-Type']}
        )
        fileitem = form['file']

        if fileitem.filename:
            # 클라이언트에서 전송된 파일명 추출
            fn = os.path.basename(fileitem.filename.replace("\\", "/"))
            file_path = os.path.join(UPLOAD_DIR, fn)
            
            # 파일 저장
            with open(file_path, 'wb') as f:
                f.write(fileitem.file.read())

            message = '파일 "{}"가 업로드 되었습니다.'.format(fn)
        else:
            message = '파일이 업로드되지 않았습니다.'

        result = subprocess.run(['python', 'upload.py'], stdout=subprocess.PIPE)
        self.send_response(200)
        self.end_headers()
        self.wfile.write(result.stdout)

        print(result.stdout.decode())
        # `multipart/form-data` 파싱
        

        # # `file` 필드에서 파일 데이터 가져오기
        # file_item = form['file']
        # if file_item.filename:
        #     # 파일 경로 설정
            # file_path = os.path.join(DIRECTORY, file_item.filename)
        #     os.makedirs(DIRECTORY, exist_ok=True)

        #     # 파일 쓰기
        #     with open(file_path, 'wb') as file_output:
        #         file_output.write(file_item.file.read())

        #     # 클라이언트에 응답
        #     self.send_response(200)
        #     self.end_headers()
        #     self.wfile.write(f"File '{file_item.filename}' uploaded successfully.".encode())
        # else:
        #     self.send_response(400)
        #     self.end_headers()
        #     self.wfile.write(b"No file was uploaded.")

    def do_GET(self):
        # `/delete.py` 경로 처리
        path_parts = urlparse(self.path)
        if path_parts.path == '/delete.py':
            query = parse_qs(path_parts.query)
            file_name = query.get('file', [''])[0]
            self.handle_delete(file_name)
        else:
            super().do_GET()

    def handle_delete(self, file_name):
        file_path = os.path.join(DIRECTORY, file_name)
        if os.path.exists(file_path):
            os.remove(file_path)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(f"File '{file_name}' deleted successfully.".encode())
        else:
            self.send_error(404, "File not found")

def run(server_class=http.server.HTTPServer, handler_class=ServerHandler):
    server_address = ('', PORT)
    httpd = server_class(server_address, handler_class)
    print(f"Serving at port {PORT}")
    httpd.serve_forever()

if __name__ == "__main__":
    run()