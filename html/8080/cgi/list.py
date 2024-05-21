#!/usr/bin/env python3

import os
import cgi
import datetime
import uuid

tmp = os.environ.get("DOCUMENT_ROOT", "/Users/seodong-gyun/42/WebServer/8080")
UPLOAD_DIR = tmp + os.environ.get("UPLOAD_DIR")
MAX_FILE_SIZE = 5 * 1024 * 1024
ALLOWED_EXTENSIONS = [".jpg", ".jpeg", ".png", ".gif", ".pdf", ".txt", ".zip", ".tar.gz", ".tar.bz2", ".tar.xz", ".tar"]
FILE_SIZE_ERROR = 1
FILE_EXTENSION_ERROR = 2
FILE_UPLOAD_ERROR = 3
FILE_NMAE_ERROR = 4
FILE_UPLOAD_SUCCESS = 5


def responseForm(status, content, contentType, contentLength, location=None):
    print(f"HTTP/1.1 {status} {status}")
    print(f"Content-Type: {contentType}; charset=utf-8")
    if location:
        print(f"Location: {location}")
    print(f"Content-Length: {contentLength}")
    print("Connection: keep-alive\r")
    print()
    print(content)

def HTMLForm(message):
    htmlstring = f"""
    <html>
    <head>
    <title>CGI</title>
    </head>
    <body>
    <h1>{message}</h1>
    </body>
    </html>
    """
    return htmlstring

class FileControl:
    
    def __init__(self, form: cgi.FieldStorage):

        self.form = form
        self.fileList = self.listFiles()

    def listFiles(self):
        files = os.listdir(UPLOAD_DIR)
        fileList = ""
        for file in files:
            # UUID 제거하고 원래 파일 이름 가져오기
            displayed_name = file.split("_", 1)[-1]
            # 파일 이름이 너무 길 경우 일부를 생략하고 "..."으로 대체
            if len(displayed_name) > 20:
                displayed_name = displayed_name[:17] + "..."
            fileList += f"""
            <li>
                <div class="file-item">
                    <a href="{UPLOAD_DIR}/{file}" class="file-link" download="{displayed_name}">{displayed_name}</a>
                    <form action="/CGI.py/delete" method="get" class="delete-form">
                        <input type="hidden" name="delete" value="{file}">
                        <p onclick="this.parentNode.submit();" class="delete-btn">삭제</p>
                    </form>
                </div>
            </li>
            """
        return fileList

    def display(self):
        print("HTTP/1.1 200 OK")
        print("Content-type: text/html; charset=utf-8")
        print(f"Date: {datetime.datetime.now().strftime('%a, %d %b %Y %H:%M:%S %Z')}")
        Response = f"""
        <!DOCTYPE html>
        <html lang="ko">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>파일 업로드 및 삭제</title>
            <style>
                body {{
                    font-family: Arial, sans-serif;
                    background-image: url("ground2.png");
                    background-color: #f2f2f2;
                    display: flex;
                    flex-direction: column;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                }}
                .container {{
                    background-color: white;
                    padding: 40px;
                    border-radius: 10px;
                    box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);
                    text-align: center;
                    max-width: 600px;
                }}
                h1 {{
                    color: #333;
                    font-size: 36px;
                    margin-bottom: 20px;
                }}
                form {{
                    margin-top: 20px;
                }}
                input[type="file"] {{
                    margin-bottom: 10px;
                }}
                input[type="submit"] {{
                    padding: 12px 24px;
                    background-color: #4CAF50;
                    color: white;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                    transition: background-color 0.3s ease;
                    display: none;
                }}
                input[type="submit"]:hover {{
                    background-color: #45a049;
                }}
                ul {{
                    list-style-type: none;
                    padding: 0;
                    margin-top: 20px;
                    text-align: left;
                }}
                li {{
                    margin-bottom: 10px;
                }}
                .file-item {{
                    display: flex;
                    align-items: center;
                    justify-content: space-between;
                }}
                .file-link {{
                    color: #333;
                    text-decoration: none;
                    font-size: 18px;
                    font-weight: bold;
                    white-space: nowrap;
                    overflow: hidden;
                    text-overflow: ellipsis;
                    max-width: 70%;
                }}
                .file-link:hover {{
                    color: #4CAF50;
                }}
                .delete-form {{
                    display: inline-block;
                    margin-left: 10px;
                }}
                .delete-btn {{
                    padding: 6px 12px;
                    background-color: #f44336;
                    color: white;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                    transition: background-color 0.3s ease;
                    font-size: 14px;
                    display: inline;
                    margin: 0;
                }}
                .delete-btn:hover {{
                    background-color: #d32f2f;
                }}
            </style>
            <script>
                function enableUpload() {{
                    document.getElementById("uploadBtn").style.display = "inline-block";
                }}
            </script>
        </head>
        <body>
            <div class="container">
                <h1>파일 업로드 및 삭제</h1>
                <form action="/CGI.py" method="post" enctype="multipart/form-data">
                    <input type="file" name="file" onchange="enableUpload()">
                    <input type="submit" value="업로드" id="uploadBtn">
                </form>
                <h2>업로드된 파일 목록</h2>
                <ul>
                    {self.fileList}
                </ul>
            </div>
        </body>
        </html>"""
        print(f"Content-Length: {len(Response.encode('utf-8'))}")
        print("Connection: keep-alive\r")
        print("\r")
        print(Response)
    def save_uploaded_file(self, fileitem, UPLOAD_DIR):
        filename = fileitem.filename

        if filename == "" or filename is None:
            return FILE_NMAE_ERROR, None

        _, ext = os.path.splitext(filename)
        if ext.lower() not in ALLOWED_EXTENSIONS:
            return FILE_EXTENSION_ERROR, None

        filesize = fileitem.limit
        if filesize > MAX_FILE_SIZE:
            return FILE_SIZE_ERROR, None

        basename = os.path.basename(filename)

        unique_filename = str(uuid.uuid4()) + "_" + basename

        filepath = os.path.join(UPLOAD_DIR, unique_filename)

        # 파일 저장
        with open(filepath, "wb") as f:
            f.write(fileitem.file.read())

        return FILE_UPLOAD_SUCCESS, unique_filename

    def handleUpload(self):

        fileitem = self.form["file"]

        filename, fileInfo = self.save_uploaded_file(fileitem, UPLOAD_DIR)
        if filename == FILE_NMAE_ERROR:
            self.fileList = self.listFiles()
            self.display()
        elif filename == FILE_UPLOAD_SUCCESS:
            self.displaySuccess()
        else:
            if filename == FILE_SIZE_ERROR:
                html = """
<!DOCTYPE html>
<html lang="ko">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>파일 크기 초과</title>
<style>
    body {
        font-family: Arial, sans-serif;
        text-align: center;
        padding: 50px;
    }
    h1 {
        font-size: 36px;
        color: #ff6b6b;
        margin-bottom: 20px;
    }
    p {
        font-size: 18px;
        color: #4ecdc4;
    }
    a.button {
        background-color: #4ecdc4;
        border: none;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        cursor: pointer;
    }
</style>
</head>
<body>
<h1>와우! 너무 크고 엄청나...!!!</h1>
<p> 파일이 너무 커서 업로드 할 수 없어요...</p>
<form action="/CGI.py/listing" method="get">
    <button type="submit"> 뒤로 가기 </button>
</form>
</body>
</html>"""
                responseForm(400, html, "text/html", len(html.encode("utf-8")))
            elif filename == FILE_EXTENSION_ERROR:
                html = """
                <!DOCTYPE html>
                <html>
                <head>
                    <title>허용되지 않는 확장자</title>
                    <style>
                        body {
                            font-family: Arial, sans-serif;
                            text-align: center;
                            padding: 50px;
                        }
                        h1 {
                            font-size: 36px;
                            color: #ff6b6b;
                            margin-bottom: 20px;
                        }
                        h2 {
                            font-size: 24px;
                            color: #ff6b6b;
                            margin-bottom: 20px;
                        }
                        p {
                            font-size: 18px;
                            color: #4ecdc4;
                        }
                        form {
                            margin-top: 30px;
                        }
                        button {
                            font-size: 18px;
                            padding: 10px 20px;
                            background-color: #ff6b6b;
                            color: white;
                            border: none;
                            border-radius: 5px;
                            cursor: pointer;
                        }
                    </style>
                </head>
                <body>
                    <h1>앗! 이런 확장자는 허용되지 않아요!</h1>
                    <p>
                        허용되는 확장자는 다음과 같습니다: .jpg, .jpeg, .png, .gif, .pdf, .txt, .zip, .tar.gz, .tar.bz2, .tar.xz, .tar</p>
                    </p>
                        <form action="/CGI.py/listing" method="get">
                            <button type="submit"> 뒤로 가기 </button>
                        </form>
                </body>
                </html>
                """
                responseForm(400, html, "text/html", len(html.encode("utf-8")))
            else:
                html = """
                <!DOCTYPE html>
                <html>
                <head>
                    <title>파일 업로드 실패</title>
                    <style>
                        body {
                            font-family: Arial, sans-serif;
                            text-align: center;
                            padding: 50px;
                        }
                        h1 {
                            font-size: 36px;
                            color: #ff6b6b;
                            margin-bottom: 20px;
                        }
                        p {
                            font-size: 18px;
                            color: #4ecdc4;
                        }
                        button {
                            background-color: #4ecdc4;
                            border: none;
                            color: white;
                            padding: 15px 32px;
                            text-align: center;
                            text-decoration: none;
                            display: inline-block;
                            font-size: 16px;
                            margin: 4px 2px;
                            cursor: pointer;
                        }
                    </style>
                </head>
                <body>
                    <h1>파일 업로드 실패</h1>
                    <p>파일 업로드에 실패했습니다. 다시 시도해주세요.</p>
                <form action="/CGI.py/listing" method="get">
                    <button type="submit"> 뒤로 가기 </button>
                </form>
                </body> 
                </html>
                """
                responseForm(200, html, "text/html", len(html.encode("utf-8")))


    def delete(self):
        file = self.form["delete"].value
        try:
            os.remove(os.path.join(UPLOAD_DIR, file))
            self.fileList = self.listFiles()
        except FileNotFoundError:
            self.display()

    def displaySuccess(self):
        html = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>파일 업로드 성공</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    text-align: center;
                    padding: 50px;
                }
                h1 {
                    font-size: 36px;
                    color: #4CAF50;
                    margin-bottom: 20px;
                }
                p {
                    font-size: 18px;
                    color: #4ecdc4;
                }
                button {
                    background-color: #4ecdc4;
                    border: none;
                    color: white;
                    padding: 15px 32px;
                    text-align: center;
                    text-decoration: none;
                    display: inline-block;
                    font-size: 16px;
                    margin: 4px 2px;
                    cursor: pointer;
                }
            </style>
        </head>
        <body>
            <h1>파일 업로드 성공</h1>
            <p>파일이 성공적으로 업로드 되었습니다.</p>
            <form action="/CGI.py/listing" method="get">
                    <button type="submit">파일 목록 보기</button>
            </form>
        </body>
        </html>
        """
        responseForm(200, html, "text/html", len(html.encode("utf-8")))