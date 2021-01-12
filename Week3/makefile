# CC = gcc
# CFLAGS  = -Wall
# TARGET_1 = server
# TARGET_2 = client
# # EXTENSION = _w3e1.c

# all: $(TARGET_1) $(TARGET_2)

# $(TARGET_1): $(TARGET_1)
# 	$(CC) $(CFLAGS) -o $(TARGET_1) $(TARGET_1).c

# $(TARGET_2): $(TARGET_2)
# 	$(CC) $(CFLAGS) -o $(TARGET_2) $(TARGET_2).c
# clean:
# 	$(RM) $(TARGET_1) $(TARGET_2) *.o

main: server.c client.c
	gcc client.c -o client
	gcc server.c -o server
clean:
	rm -f client server  *.o *~