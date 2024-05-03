# user_info.py
import cgi

# FieldStorage 객체 생성
form = cgi.FieldStorage()

# 폼 데이터 읽기
name = form.getvalue("name", "")
age = form.getvalue("age", "")
print(name, age)

# HTTP 응답 헤더 출력
print("Content-type: text/html\n")

# HTML 출력
print("<html>")
print("<head><title>User Info</title></head>")
print("<body>")
print(f"<h1>User Information</h1>")
print(f"<p>Name: {name}</p>")
print(f"<p>Age: {age}</p>")
print("</body>")
print("</html>")