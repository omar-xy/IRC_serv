SRC		=		src/Client.cpp	\
				src/Channel.cpp	\
				src/utils.cpp	\
				src/IRCserv.cpp	\
				src/main.cpp

OBJ		=		$(SRC:.cpp=.o)

NAME	=		ircserv

CXX		=		c++

CXXFLAGS	=		-W -Wall -Wextra -std=c++98 -I./include #-g3 -fsanitize=address

HEADER	=		headers/IRCserv.hpp \
				headers/Channel.hpp\
				headers/Client.hpp\
				headers/header.hpp

all:		$(NAME)

$(NAME):	$(OBJ)
		$(CXX) -o $(NAME) $(OBJ) $(CXXFLAGS)

%.o:		%.cpp $(HEADER)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
		rm -f $(OBJ)

fclean:		clean
		rm -f $(NAME)
	
re:		fclean all

run: all
	./$(NAME) 8000 xxx

.PHONY:		all clean fclean re