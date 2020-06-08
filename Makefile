CC := gcc
SRCD := src
#TSTD := tests
BLDD := build
BIND := bin
INCD := include
#LIBD := ../lib

SRCF := $(shell find $(SRCD) -type f -name *.c)
SRC_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(SRCF:.c=.o))

INC := -I $(INCD)

CFLAGS= -Wall #-L lib -l libsemver.so#-lyaml

#LIBS := -L $(LIBD) -l libsemver.so

ODIR=obj

EXEC := citesoft

all: setup $(EXEC)

.PHONY: clean all

setup:
	@echo $(EC)
	mkdir -p bin build


# $(EXEC): $(SRC_OBJF)
# 	$(CC) $(CFLAGS) $(STD) $(INC) $(LIBD)/libsemver $^ -o $(BIND)/$@

$(EXEC): $(SRC_OBJF)
	$(CC) $(CFLAGS) $(STD) $(INC) $^ -o $(BIND)/$@

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(STD) $(INC) -c $< -o $@


clean:
	rm -rf $(BLDD) $(BIND)
