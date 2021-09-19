all:
	gcc -g main.c
	./a.out
	python3 convert.py
