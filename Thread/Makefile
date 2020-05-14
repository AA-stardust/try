CFLAGS += -o1
SRCS := thread1.c
NAME := thread1
$(NAME):
	gcc $(CFLAGS) $(SRCS) -o $@ -lpthread
clean:
	rm $(NAME)
run:
	./$(NAME)