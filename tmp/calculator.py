#!/usr/bin/env python

import cgi

# CGI 필드 파싱
form = cgi.FieldStorage()

# GET 또는 POST로 전달된 값 가져오기
# lft = form.getvalue('lft', '0')
# rgt = form.getvalue('rgt', '0')
# operator = form.getvalue('operator', '+')

lft = 2
rgt = 3
operator = '+'

# 계산 수행
result = 0
if operator == '+':
    result = int(lft) + int(rgt)
elif operator == '-':
    result = int(lft) - int(rgt)
elif operator == '*':
    result = int(lft) * int(rgt)
elif operator == '/':
    result = int(lft) / int(rgt)

# HTML 응답 생성
print("""<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>계산기 결과</title>
</head>
<body>
    <h1>계산 결과</h1>
    <p>{} {} {} = {}</p>
</body>
</html>""".format(lft, operator, rgt, result))