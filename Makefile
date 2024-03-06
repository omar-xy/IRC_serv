SRC		=		src/Client.cpp	\
				src/IRCserv.cpp	\
				src/main.cpp

OBJ		=		$(SRC:.cpp=.o)

NAME	=		ircserv

CXX		=		c++

CXXFLAGS	=		-W -Wall -Wextra -std=c++98 -I./include

HEADER	=		headers/IRCserv.hpp \
				headers/Client.hpp\
				headers/header.hpp

all:		$(NAME)

$(NAME):	$(OBJ)
		$(CXX) -o $(NAME) $(OBJ)

%.o:		%.cpp $(HEADER)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

clean:
		rm -f $(OBJ)

fclean:		clean
		rm -f $(NAME)
	
re:		fclean all

.PHONY:		all clean fclean re