gdbFlag = 0

ifeq ($(gdbFlag), 1)
gdb = -g
endif

hmm1: Hmm.c doubleLinked.c  stress_test.c
	gcc -o hmm1 $(gdb) Hmm.c doubleLinked.c stress_test.c

hmm2: Hmm.c doubleLinked.c test.c
	gcc -o hmm2 $(gdb) Hmm.c doubleLinked.c test.c

