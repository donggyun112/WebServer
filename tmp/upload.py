import cgi
import os
from http import cookies

# 업로드 파일 저장 경로
UPLOAD_DIR = './uploaded'

# 환경 설정 및 디렉토리 생성
if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

# 파일 업로드 처리
form = cgi.FieldStorage()

if 'file' in form:
    fileitem = form['file']

    if fileitem.filename:
        # 클라이언트에서 전송된 파일명 추출
        fn = os.path.basename(fileitem.filename.replace("\\", "/"))
        file_path = os.path.join(UPLOAD_DIR, fn)
        
        # 파일 저장
        with open(file_path, 'wb') as f:
            f.write(fileitem.file.read())

        message = 'file "{}" upload done.'.format(fn)
    else:
        message = 'can\'t "{}" uploaed .'
else:
    message = 'please upload file.'

# 결과 페이지 출력
print("""<!DOCTYPE html>
<html>
<head>
    <title>Upload Result</title>
</head>
<body>
    <p>{}</p>
    <a href="/">Return to the main page...</a>
</body>
</html>""".format(message))