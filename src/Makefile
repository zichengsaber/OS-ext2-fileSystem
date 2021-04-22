SRC=$(shell find ./ -name "*.c") # 找到文件下面所有的.c
SHELLOBJS=$(SRC:%.c=%.o)
CC=gcc
Target=shell

$(Target): $(SHELLOBJS)
	$(CC) -g -o $(Target) $(SHELLOBJS) -Wall
#	$(CC) -g -o shell $(SHELLOBJS) -Wall
%.o:%.c 
	$(CC) -c $< -o $@

clean:
	rm $(SHELLOBJS)
	rm $(Target)
