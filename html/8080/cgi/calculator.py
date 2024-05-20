import cgi
import datetime
import sys


class Calculator:
	def __init__(self, form : cgi.FieldStorage):
		self.result = ''
		self.form = form
		
	def calculate(num1, operator, num2):
		if operator == '+':
			return num1 + num2
		elif operator == '-':
			return num1 - num2
		elif operator == '*':
			return num1 * num2
		elif operator == '/':
			if num2 != 0:
				return num1 / num2
			else:
				return "Error: Division by zero"

	def display(self):
		result = self.form.getvalue('result', '')
		print(result)
		if 'number' in self.form:
			num = self.form.getvalue('number')
			result += num
		elif 'decimal' in self.form:
			decimal = self.form.getvalue('decimal')
			result += decimal
		elif 'operator' in self.form:
			operator = self.form.getvalue('operator')
			if result:
				num1 = float(result)
				num2 = float(self.form.getvalue('number', '0'))
				result = str(self.calculate(num1, operator, num2))
			else:
				
				result = "Error: Invalid operation"
		elif 'equal' in self.form:
			result = result
		print("HTTP/1.1 200 OK\r")
		print("Content-type: text/html; charset=utf-8\r")
		print(f"Date: {datetime.datetime.now().strftime('%a, %d %b %Y %H:%M:%S %Z')}\r")
		Response = f"""
<!DOCTYPE html>
<html>
<head>
<title>Calculator</title>
<style>
	.calculator {{
	width: 300px;
	margin: 0 auto;
	background-color: black;
	border-radius: 10px;
	padding: 20px;
	}}
	
	.display {{
	background-color: white;
	padding: 10px;
	margin-bottom: 20px;
	text-align: right;
	font-size: 24px;
	border-radius: 5px;
	}}
	
	.buttons {{
	display: grid;
	grid-template-columns: repeat(4, 1fr);
	grid-gap: 10px;
	}}
	
	button {{
	background-color: #d8d8d8;
	border: none;
	border-radius: 5px;
	padding: 15px;
	font-size: 18px;
	cursor: pointer;
	}}
	
	.function {{
	background-color: #a5a5a5;
	color: black;
	}}
	
	.operator {{
	background-color: #ff9500;
	color: white;
	}}
	
	.zero {{
	grid-column: 1 / span 2;
	}}
</style>
</head>
<body>
<form action="/CGI.py/calculator" method="get" class="calculator">
	<div class="display">
	<input type="text" id="result" name="result" value="{result}" readonly>
	</div>
	<div class="buttons">
	<div class="buttons">
	<button type="button" class="function" onclick="clearDisplay()">AC</button>
	<button type="button" class="function" onclick="toggleSign()">±</button>
	<button type="button" class="function" onclick="calculatePercentage()">%</button>
	<button type="submit" name="operator" value="/" class="operator">÷</button>
	<button type="submit" name="number" value="7">7</button>
	<button type="submit" name="number" value="8">8</button>
	<button type="submit" name="number" value="9">9</button>
	<button type="submit" name="operator" value="*" class="operator">×</button>
	<button type="submit" name="number" value="4">4</button>
	<button type="submit" name="number" value="5">5</button>
	<button type="submit" name="number" value="6">6</button>
	<button type="submit" name="operator" value="-" class="operator">-</button>
	<button type="submit" name="number" value="1">1</button>
	<button type="submit" name="number" value="2">2</button>
	<button type="submit" name="number" value="3">3</button>
	<button type="submit" name="operator" value="+" class="operator">+</button>
	<button type="submit" name="number" value="0" class="zero">0</button>
	<button type="submit" name="decimal" value=".">.</button>
	<button type="submit" name="equal" value="=" class="operator">=</button>
	</div>
</form>
</body>
</html>"""
		print(f"Content-Length: {len(Response)}")
		print("Connection: keep-alive\r")
		print("\r")
		print(Response)
		sys.exit(0)