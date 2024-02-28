import os

arq = open("tests.txt", "r")
str = arq.read()
arq.close()

a = str.split('\n')
commands = []

for i in a:
	if (i != '' and '#' not in i):
		commands.append(i)

# print(*commands, sep='\n')

os.system("cp ../pipex .")

for command in commands:
	print(command)
	return_code = os.system(command)
	print((return_code >> 8) & 0xFF)
	input()
