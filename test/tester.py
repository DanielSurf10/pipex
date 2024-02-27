arq = open("test/tests.txt", "r")
str = arq.read()
arq.close()

a = str.split('\n')
command = []

for i in a:
	if (i != '' and '#' not in i):
		command.append(i)

print(*command, sep='\n')
