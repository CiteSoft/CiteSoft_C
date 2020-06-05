CC=gcc
CFLAGS=#-lyaml
DEPS = citesoft.h datastruct.h stringhash.h semver.h

ODIR=obj

$(ODIR)/%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

citesoft: citesoft.o datastruct.o stringhash.o semver.o
		$(CC) -o citesoft citesoft.o datastruct.o stringhash.o semver.o $(CFLAGS)

.PHONY: clean
clean :
				rm citesoft citesoft.o datastruct.o stringhash.o semver.o
