include config.mk

SRC = congrats.c
OBJ = $(SRC:.c=.o)

all: congrats

.c.o:
	@echo compiling $< with $(CC)
	@$(CC) -c $(CFLAGS) $<

$(OBJ): Makefile config.mk

congrats: $(OBJ)
	@echo linking $<
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@echo cleaning
	@rm -f congrats $(OBJ)
