#!/usr/bin/env python3
import cgi
import cgitb
from list import FileControl
from calculator import Calculator

cgitb.enable(display=0, logdir="/logs")

form = cgi.FieldStorage()


def responseForm(status, content, contentType, contentLength, location=None):
	print(f"Content-Type: {contentType}")
	if location:
		print(f"Location: {location}")
	print(f"Content-Length: {contentLength}")
	print(f"Status: {status}")
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


def handleGet():
	path_info = form.getvalue('PATH_INFO')
	if path_info == "/calculate":
		cal = Calculator(form)
		cal.display()
	elif path_info == "/listing":
		listing = FileControl(form)
		listing.display()
	else:
		html = HTMLForm("Bad Request")
		responseForm(400, html, "text/html", len(html))




def handlePost():
	contetenType = form.getvalue('CONTENT_TYPE')
	if contetenType == "multipart/form-data":
		listing = FileControl(form)
		listing.handleUpload()
	else:
		html = HTMLForm("Bad Request")
		responseForm(400, html, "text/html", len(html))
	


def handleMethod():
	method = form.getvalue('REQUEST_METHOD')
	if method == 'GET':
		handleGet()
	elif method == 'POST':
		handlePost()
	else:
		html = HTMLForm("Method Not Allowed")
		responseForm(405, html, "text/html", len(html))


def main():
	print(form)
	# if not checkProtocol():
		# html = HTMLForm("Bad Request")
		# responseForm(400, html, "text/html", len(html))
		# return 
	handleMethod()

if __name__ == '__main__':
	main()


