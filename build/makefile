SRCS := ../source
LIBS := -lboost_system -lboost_regex -lpthread

DIRS := \
	common \
	client \
	server
	
OBJ_DIRS := $(addprefix obj/, $(DIRS))

COMMON_SOURCES := \
	common/acceptor.cpp \
	common/logger.cpp \
	common/message.cpp \
	common/session.cpp

CLIENT_SOURCES := \
	$(COMMON_SOURCES) \
	client.cpp \
	client/sender.cpp
	
SERVER_SOURCES := \
	$(COMMON_SOURCES) \
	server.cpp \
	server/data_handler.cpp \
	server/flow_handler.cpp \
	server/manager.cpp

CLIENT_OBJS := $(patsubst %.cpp, obj/%.o, $(CLIENT_SOURCES))
SERVER_OBJS := $(patsubst %.cpp, obj/%.o, $(SERVER_SOURCES))

vpath %.cpp $(SRCS)

all: client server

client : dirs $(CLIENT_OBJS)
	g++ -o lesen-client $(CLIENT_OBJS) $(LIBS)

server : dirs $(SERVER_OBJS)
	g++ -o lesen-server $(SERVER_OBJS) $(LIBS)
	
clean:
	rm -rf obj/
	rm -rf lesen-*
	
dirs:
	mkdir -p $(OBJ_DIRS)

obj/%.o : %.cpp
	g++ -std=c++11 -I $(SRCS) -Wall -c -o "$@" "$<"