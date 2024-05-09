CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g
NAME = webserv

SRCS =	Server/EchoServer.cpp \
		request/Client.cpp \
		request/RequestHandle.cpp \
		request/Request.cpp \
		socket/socket.cpp \
		Parse/Config.cpp \
		Parse/ServerConfig.cpp \
		Parse/LocationConfig.cpp \
		tmp/mainTest.cpp \
		utils/utils.cpp \

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

.cpp.o: 
	${CC} $(CFLAGS) -c -o $@ $<

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re:	
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all bonus clean fclean re
