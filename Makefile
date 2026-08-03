NAME = codexion

SRC = src/parser.c src/set_table.c src/utils.c \
	  src/miniheap.c src/coders1.c src/coders2.c \
	  src/cleanup.c src/codexion.c  src/monitor.c \
	  src/main.c

OBJ = $(SRC:.c=.o)

HEADER = src/codex.h

CFLAGS = -Wall -Wextra -Werror -pthread

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	cc $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(HEADER)
	cc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all


