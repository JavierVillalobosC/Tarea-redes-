
CLIENT_TARGET = Client


SERVER_TARGET = Server


CC = g++


CFLAGS = -std=c++11 -Wall


CLIENT_SOURCES = mClient.cpp


SERVER_SOURCES = mServer.cpp


CLIENT_OBJECTS = $(CLIENT_SOURCES:.cpp=.o)


SERVER_OBJECTS = $(SERVER_SOURCES:.cpp=.o)


$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(CLIENT_OBJECTS) -o $(CLIENT_TARGET)


$(SERVER_TARGET): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(SERVER_OBJECTS) -o $(SERVER_TARGET)


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


#clean:
#	rm -f $(CLIENT_OBJECTS) $(CLIENT_TARGET) $(SERVER_OBJECTS) $(SERVER_TARGET)

