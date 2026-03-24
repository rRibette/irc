####################### SETUP ##########################
CC = c++

RM = rm -rf

NAME = ircserv

####################### FLAGS ##########################

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -g3

####################### SRC & OBJ ######################

SRC_PATH = srcs/
INC_PATH = includes/

SRC_FILES = $(SRC_PATH)main.cpp $(SRC_PATH)server.cpp $(SRC_PATH)client.cpp $(SRC_PATH)channel.cpp $(SRC_PATH)commands.cpp $(SRC_PATH)server_reply.cpp $(SRC_PATH)responses.cpp

OBJ_FILES = $(SRC_FILES:.cpp=.o)

TSRC_FILES = $(SRC_PATH)server.cpp $(SRC_PATH)client.cpp $(SRC_PATH)channel.cpp $(SRC_PATH)commands.cpp $(SRC_PATH)server_reply.cpp $(SRC_PATH)responses.cpp

TOBJ_FILES = $(TSRC_FILES:.cpp=.o)
####################### RULES ##########################

all: $(NAME)

$(NAME): $(OBJ_FILES)
	$(CC) $(CPPFLAGS) -o $(NAME) $(OBJ_FILES)

testRomain: testChannel.o $(TOBJ_FILES)
	$(CC) $(CPPFLAGS) -g3 -o $@ $^

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	@$(RM) $(OBJ_FILES)

fclean: clean
	@$(RM) $(NAME)

tclean:
	@$(RM) $(TOBJ_FILES) testRomain testChannel.o

re: fclean \
	all
