CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g
NAME = webserv

SRCS = tmp/EchoServer.cpp request/Request.cpp request/Client.cpp socket/socket.cpp Parse/Config.cpp Parse/ServerConfig.cpp Parse/LocationConfig.cpp tmp/mainTest.cpp

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
