APP := shell 
CC  := cc
CFLAGS := -std=c17 -Wall -Wextra -O2

all: $(APP)

$(APP): shell.c
	$(CC) $(CFLAGS) $< -o $@

run: $(APP)
	./$(APP)

clean:
	rm -f $(APP)
