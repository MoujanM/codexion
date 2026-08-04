NAME = codexion

SRC = src/parser.c src/setup.c src/utils.c \
	  src/miniheap.c src/coders1.c src/coders2.c \
	  src/cleanup.c src/codexion.c  src/monitor.c \
	  src/main.c

OBJ = $(SRC:.c=.o)
DEBUG_OBJ = $(SRC:.c=.debug.o)

HEADER = src/codex.h
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread

.PHONY: all clean fclean re debug

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

%.debug.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -g -c $< -o $@

debug: $(DEBUG_OBJ)
	$(CC) $(CFLAGS) -g $(DEBUG_OBJ) -o d_cod

clean:
	rm -f $(OBJ)
	rm -f $(DEBUG_OBJ)

fclean: clean
	rm -f $(NAME)
	rm -f d_cod

re: fclean all


