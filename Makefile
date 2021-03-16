.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
C=-Wall --pedantic -std=c99 -fsanitize=address
LDLIBS=
OBJDIR=obj
vpath %.c src
vpath %.h src

objects = $(addprefix $(OBJDIR)/, interpret.o lex.o parse.o terms.o utils.o utlami.o )

utlami : $(objects)
	$(CC) $(CFLAGS) -o utlami $(objects) $(LDLIBS)

$(OBJDIR)/%.o : %.c utlam.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY : clean
clean :
	rm $(objects)
