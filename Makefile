all:
	gcc -g main.c
	./a.out
	python3 convert.py
clean:
	rm -f ./out.sfd
	rm -f ./Quilt.woff2
	rm -f ./a.out
