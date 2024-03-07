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

# os.system("cp ../pipex .")
val = "valgrind -q --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all --trace-children-skip='*/bin/*,*/sbin/*' "

for command in commands:
	print(command.replace("./pipex", "../pipex"))
	return_code = os.system(val + command.replace("./pipex", "../pipex"))
	print((return_code >> 8) & 0xFF)
	# try:
	# 	input()
	# except:
	# 	pass
