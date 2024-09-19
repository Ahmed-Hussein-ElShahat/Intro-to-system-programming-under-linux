gcc -g -c -fPIC *.c
gcc -shared -o libfoo.so *.o
rm *.o
LD_PRELOAD=./libfoo.so ls

