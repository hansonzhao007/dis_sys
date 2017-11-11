CC=gcc
CFLAGS=
DEPS = lc_util.h mylib.h lc_msg_api.h
OBJ = main.o lc_util.o lc_msg_api.o
DEBUG=#-g -O0
%.o: %.c $(DEPS)
	$(CC) $(DEBUG) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc $(DEBUG) -pthread -o  $@ $^ $(CFLAGS)

clean:
	-rm lg_deamon $(OBJ)
