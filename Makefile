all: shell

clean:
	rm shell

leaks: shell
	valgrind --tool=memcheck --leak-check=full -v ./shell

try: shell
	./shell
	rm shell

tests: shell tests/ladoIzq.test tests/ladoDer.test
	cat tests/ladoIzq.test | ./shell > tests/ladoIzq.res
	cat tests/ladoDer.test | ./shell > tests/ladoDer.res
	diff tests/ladoIzq.res tests/ladoDer.res
	

shell: shell.c libs/operaciones.c libs/operaciones.h libs/straux.c libs/straux.h libs/chash.c libs/chash.h libs/conjunto.c libs/conjunto.h
	gcc -Wall -Werror -std=c99 -Wextra shell.c libs/operaciones.c libs/operaciones.h libs/straux.c libs/straux.h libs/chash.c libs/chash.h libs/conjunto.c libs/conjunto.h -o shell -lm
