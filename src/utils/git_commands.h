#ifndef GIT_COMMANDS_H
#define GIT_COMMANDS_H

/**
 * Execute a git command and return its output
 * Caller is responsible for freeing the returned string
 * @param command Git command to execute
 * @return Allocated string with command output, or NULL on error
 */
char* execute_git_command(const char* command);

/**
 * Count lines in a file
 * @param filename Path to the file
 * @return Number of lines, or -1 on error
 */
int count_lines_in_file(const char* filename);

#endif /* GIT_COMMANDS_H */