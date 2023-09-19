CC=g++
CFLAGS=-I. -g

client: client.cpp helpers.cpp buffer.cpp
	$(CC) $(CFLAGS) -o client client.cpp helpers.cpp buffer.cpp -Wall

run: client
	./client

clean:
	rm -f *.o client
