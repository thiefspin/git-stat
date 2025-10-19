#define _GNU_SOURCE
#include "git_commands.h"
#include "../git_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Execute a git command and return its output
 * Caller is responsible for freeing the returned string
 */
char* execute_git_command(const char* command) {
    assert(command != NULL);
    
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        return NULL;
    }
    
    char *result = malloc(MAX_LINE_LENGTH);
    if (result == NULL) {
        pclose(fp);
        return NULL;
    }
    
    if (fgets(result, MAX_LINE_LENGTH, fp) != NULL) {
        pclose(fp);
        return result;
    }
    
    pclose(fp);
    free(result);
    return NULL;
}

/**
 * Count lines in a file
 * Returns -1 on error
 */
int count_lines_in_file(const char* filename) {
    assert(filename != NULL);
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    
    int lines = 0;
    int ch;
    
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }
    
    if (ferror(file)) {
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return lines;
}