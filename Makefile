SRC	=	main.cpp Server.cpp Client.cpp

OBJ	=	$(SRC:.cpp=.o)

HEAD	=	-I./include

CXXFLAGS	= -std=c++98 -g3 -fsanitize=address

CXX	=	c++

NAME	=	ircserv

all:	$(NAME)

$(NAME):	$(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(HEAD) $(CXXFLAGS)

%.o:	%.cpp $(HEAD)
	$(CXX) -c $< -o $@ $(HEAD) $(CXXFLAGS)

clean:
	rm -f $(OBJ)

fclean:	clean
	rm -f $(NAME)

run:	all
	./$(NAME) 5000 pass

re:	fclean all