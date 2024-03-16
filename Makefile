NAME = irc_serv
CC = c++
CFLAGS =-Wall -Wextra -Werror -g -fsanitize=address
SRCDIR = .
HDRDIR = headers
OBJDIR = obj
SRCS = $(shell find $(SRCDIR) -type f -name '*.cpp')
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
INCLUD = $(shell find $(HDRDIR) -type f -name '*.hpp')

.PHONY: all clean fclean re

all: $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCLUD)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJECTS) $(LIB)
	$(CC) $(CFLAGS) $(OBJECTS) $(LIB) -o $(NAME) $(LDFLAGS)

run: all
	./$(NAME) 5000 x

clean:
	rm -rf $(OBJDIR)

	@echo "Cleaning objects"

fclean: clean
	rm -rf $(NAME)
	@echo "Cleaning objects and executable"

re: fclean all

.PRECIOUS: $(OBJECTS)