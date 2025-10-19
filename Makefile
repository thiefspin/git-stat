CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c17
LDFLAGS = -lm
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

# Default target
all: git-stat

# Build the main executable
git-stat: main.o
	$(CC) $(CFLAGS) -o git-stat main.o $(LDFLAGS)

# Compile main source file
main.o: main.c version.h
	$(CC) $(CFLAGS) -c main.c

# Install to system
install: git-stat
	install -d $(BINDIR)
	install -m 755 git-stat $(BINDIR)

# Install to user's local bin
install-user: git-stat
	install -d $(HOME)/.local/bin
	install -m 755 git-stat $(HOME)/.local/bin

# Uninstall from system
uninstall:
	rm -f $(BINDIR)/git-stat

# Uninstall from user's local bin
uninstall-user:
	rm -f $(HOME)/.local/bin/git-stat

# Clean build artifacts
clean:
	rm -f git-stat main.o

# Test the binary
test: git-stat
	./git-stat --help

# Create distribution tarball
dist: clean
	tar -czf git-stat-1.0.tar.gz *.c *.h Makefile README.md LICENSE install.sh

# Development targets
debug: CFLAGS += -g -DDEBUG
debug: git-stat

# Static analysis
lint:
	clang-tidy main.c -- $(CFLAGS)

.PHONY: all install install-user uninstall uninstall-user clean test dist debug lint
