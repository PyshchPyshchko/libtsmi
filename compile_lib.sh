clang -std=c99 -Wall -pedantic -O0 -g -I./include -c -fPIC `pkg-config --cflags glib-2.0` tsmi.c 
clang -Wl,-rpath,. -L. -ltcod -lSDL_ttf -shared -Wl,-soname,libtsmi.so `pkg-config --libs glib-2.0` -o libtsmi.so *.o 
