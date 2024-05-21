import cgi
import datetime

class Calculator:
    def __init__(self, form: cgi.FieldStorage):
        self.result = ''
        self.form = form

    def calculate(self):
        first_operand = float(self.form.getvalue('firstOperand'))
        second_operand = float(self.form.getvalue('secondOperand'))
        operator = self.form.getvalue('operator')

        if operator == '+':
            self.result = first_operand + second_operand
        elif operator == '-':
            self.result = first_operand - second_operand
        elif operator == '*':
            self.result = first_operand * second_operand
        elif operator == '/':
            self.result = first_operand / second_operand if second_operand != 0 else 'Error'

    def display(self):
        print("HTTP/1.1 200 OK")
        print("Content-Type: text/plain")
        print(f"Content-Length: {len(str(self.result))}")
        print()
        print(self.result)