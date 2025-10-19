CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c17
LDFLAGS = -lm
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

# Source directories
SRCDIR = src
ANALYSISDIR = $(SRCDIR)/analysis
OUTPUTDIR = $(SRCDIR)/output
UTILSDIR = $(SRCDIR)/utils

# Object files
OBJS = $(SRCDIR)/main.o \
       $(SRCDIR)/git_stats.o \
       $(ANALYSISDIR)/hotspots.o \
       $(ANALYSISDIR)/activity.o \
       $(OUTPUTDIR)/human_output.o \
       $(OUTPUTDIR)/json_output.o \
       $(UTILSDIR)/string_utils.o \
       $(UTILSDIR)/git_commands.o

# Default target
all: git-stat

# Build the main executable
git-stat: $(OBJS)
	$(CC) $(CFLAGS) -o git-stat $(OBJS) $(LDFLAGS)

# Main source files
$(SRCDIR)/main.o: $(SRCDIR)/main.c $(SRCDIR)/git_stats.h $(SRCDIR)/version.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/main.c -o $(SRCDIR)/main.o

$(SRCDIR)/git_stats.o: $(SRCDIR)/git_stats.c $(SRCDIR)/git_stats.h
	$(CC) $(CFLAGS) -c $(SRCDIR)/git_stats.c -o $(SRCDIR)/git_stats.o

# Analysis modules
$(ANALYSISDIR)/hotspots.o: $(ANALYSISDIR)/hotspots.c $(ANALYSISDIR)/hotspots.h $(SRCDIR)/git_stats.h
	$(CC) $(CFLAGS) -c $(ANALYSISDIR)/hotspots.c -o $(ANALYSISDIR)/hotspots.o

$(ANALYSISDIR)/activity.o: $(ANALYSISDIR)/activity.c $(ANALYSISDIR)/activity.h $(SRCDIR)/git_stats.h
	$(CC) $(CFLAGS) -c $(ANALYSISDIR)/activity.c -o $(ANALYSISDIR)/activity.o

# Output formatters
$(OUTPUTDIR)/human_output.o: $(OUTPUTDIR)/human_output.c $(OUTPUTDIR)/formatters.h $(SRCDIR)/git_stats.h
	$(CC) $(CFLAGS) -c $(OUTPUTDIR)/human_output.c -o $(OUTPUTDIR)/human_output.o

$(OUTPUTDIR)/json_output.o: $(OUTPUTDIR)/json_output.c $(OUTPUTDIR)/formatters.h $(SRCDIR)/git_stats.h
	$(CC) $(CFLAGS) -c $(OUTPUTDIR)/json_output.c -o $(OUTPUTDIR)/json_output.o

# Utility modules
$(UTILSDIR)/string_utils.o: $(UTILSDIR)/string_utils.c $(UTILSDIR)/string_utils.h
	$(CC) $(CFLAGS) -c $(UTILSDIR)/string_utils.c -o $(UTILSDIR)/string_utils.o

$(UTILSDIR)/git_commands.o: $(UTILSDIR)/git_commands.c $(UTILSDIR)/git_commands.h $(SRCDIR)/git_stats.h
	$(CC) $(CFLAGS) -c $(UTILSDIR)/git_commands.c -o $(UTILSDIR)/git_commands.o

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
	rm -f git-stat $(OBJS)

# Test the binary
test: git-stat
	./git-stat --help

# Create distribution tarball
dist: clean
	tar -czf git-stat-1.0.tar.gz src/ *.md LICENSE install.sh Makefile

# Development targets
debug: CFLAGS += -g -DDEBUG
debug: git-stat

# Static analysis
lint:
	clang-tidy src/**/*.c -- $(CFLAGS)

# Directory creation (for parallel builds)
$(SRCDIR) $(ANALYSISDIR) $(OUTPUTDIR) $(UTILSDIR):
	mkdir -p $@

.PHONY: all install install-user uninstall uninstall-user clean test dist debug lint
