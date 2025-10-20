#define _GNU_SOURCE
#include "git_stats.h"
#include "utils/string_utils.h"
#include "utils/git_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Forward declarations */
static int get_repository_info(GitStats *stats);
static int get_commit_stats(GitStats *stats);
static int get_author_stats(GitStats *stats);
static int get_branch_stats(GitStats *stats);
static int get_file_stats(GitStats *stats);

/**
 * Check if current directory is a git repository
 */
int is_git_repository(void) {
    struct stat st;
    return (stat(".git", &st) == 0 && S_ISDIR(st.st_mode));
}

/**
 * Initialize GitStats structure with default values
 */
void init_git_stats(GitStats *stats) {
    assert(stats != NULL);

    memset(stats, 0, sizeof(GitStats));
    safe_string_copy(stats->current_branch, "unknown", sizeof(stats->current_branch));
    safe_string_copy(stats->repo_name, "unknown", sizeof(stats->repo_name));
}

/**
 * Gather basic git statistics
 */
int get_basic_git_stats(GitStats *stats) {
    assert(stats != NULL);

    get_repository_info(stats);

    if (get_commit_stats(stats) != 0) {
        fprintf(stderr, "Warning: Failed to get commit statistics\n");
    }

    if (get_author_stats(stats) != 0) {
        fprintf(stderr, "Warning: Failed to get author statistics\n");
    }

    if (get_branch_stats(stats) != 0) {
        fprintf(stderr, "Warning: Failed to get branch statistics\n");
    }

    if (get_file_stats(stats) != 0) {
        fprintf(stderr, "Warning: Failed to get file statistics\n");
    }

    return 0;
}

/**
 * Get basic repository information
 */
static int get_repository_info(GitStats *stats) {
    assert(stats != NULL);

    char *result;

    /* Get current branch */
    result = execute_git_command("git branch --show-current 2>/dev/null");
    if (result != NULL) {
        remove_trailing_newline(result);
        safe_string_copy(stats->current_branch, result, sizeof(stats->current_branch));
        free(result);
    }

    /* Get repository name from current directory */
    char cwd[MAX_PATH_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        const char *repo_name = strrchr(cwd, '/');
        if (repo_name != NULL) {
            safe_string_copy(stats->repo_name, repo_name + 1, sizeof(stats->repo_name));
        }
    }

    return 0;
}

/**
 * Get commit statistics
 */
static int get_commit_stats(GitStats *stats) {
    assert(stats != NULL);

    char *result = execute_git_command("git rev-list --all --count 2>/dev/null");
    if (result != NULL) {
        long commit_count = strtol(result, NULL, 10);
        if (commit_count >= 0 && commit_count <= INT_MAX) {
            stats->total_commits = (int)commit_count;
        }
        free(result);
        return 0;
    }

    return -1;
}

/**
 * Get author statistics
 */
static int get_author_stats(GitStats *stats) {
    assert(stats != NULL);

    FILE *fp;
    char line[MAX_LINE_LENGTH];

    /* Get author commit counts */
    fp = popen("git shortlog -sn --all 2>/dev/null", "r");
    if (fp == NULL) {
        return -1;
    }

    int author_count = 0;
    while (fgets(line, sizeof(line), fp) != NULL && author_count < MAX_AUTHORS) {
        char *tab = strchr(line, '\t');
        if (tab != NULL) {
            *tab = '\0';

            /* Parse commit count */
            long commits = strtol(line, NULL, 10);
            if (commits >= 0 && commits <= INT_MAX) {
                stats->authors[author_count].commit_count = (int)commits;
            }

            /* Copy author name */
            char *author_name = tab + 1;
            remove_trailing_newline(author_name);
            safe_string_copy(stats->authors[author_count].name, author_name,
                           sizeof(stats->authors[author_count].name));

            author_count++;
        }
    }
    pclose(fp);
    stats->total_authors = author_count;

    /* Get line statistics for top authors */
    for (int i = 0; i < author_count && i < MAX_AUTHORS_DISPLAY; i++) {
        char command[MAX_COMMAND_LENGTH];
        int ret = snprintf(command, sizeof(command),
                "git log --author=\"%s\" --pretty=tformat: --numstat 2>/dev/null | "
                "awk '{add+=$1; del+=$2} END {print add\" \"del}'",
                stats->authors[i].name);

        if (ret < 0 || ret >= (int)sizeof(command)) {
            continue; /* Command too long, skip this author */
        }

        char *result = execute_git_command(command);
        if (result != NULL) {
            sscanf(result, "%d %d", &stats->authors[i].lines_added,
                   &stats->authors[i].lines_deleted);
            free(result);
        }
    }

    return 0;
}

/**
 * Get branch statistics
 */
static int get_branch_stats(GitStats *stats) {
    assert(stats != NULL);

    FILE *fp = popen("git branch 2>/dev/null", "r");
    if (fp == NULL) {
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int branch_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL && branch_count < MAX_BRANCHES) {
        /* Skip if line is too short */
        if (strlen(line) < 3) continue;

        char *branch_name = line;
        /* Skip current branch marker and whitespace */
        if (line[0] == '*' || line[0] == ' ') {
            branch_name += 2;
        }

        /* Remove trailing newline */
        remove_trailing_newline(branch_name);

        /* Skip empty branch names */
        if (strlen(branch_name) == 0) continue;

        safe_string_copy(stats->branches[branch_count].name, branch_name,
                        sizeof(stats->branches[branch_count].name));

        /* Get commit count for this branch */
        char command[MAX_COMMAND_LENGTH];
        int ret = snprintf(command, sizeof(command),
                "git rev-list --count \"%s\" 2>/dev/null", branch_name);

        if (ret > 0 && ret < (int)sizeof(command)) {
            char *result = execute_git_command(command);
            if (result != NULL) {
                long commits = strtol(result, NULL, 10);
                if (commits >= 0 && commits <= INT_MAX) {
                    stats->branches[branch_count].commit_count = (int)commits;
                }
                free(result);
            }
        }

        branch_count++;
    }
    pclose(fp);
    stats->total_branches = branch_count;

    return 0;
}

/**
 * Get file statistics
 */
static int get_file_stats(GitStats *stats) {
    assert(stats != NULL);

    FILE *fp = popen("git ls-files 2>/dev/null", "r");
    if (fp == NULL) {
        return -1;
    }

    char filename[MAX_LINE_LENGTH];
    int file_count = 0;
    long total_lines = 0;

    /* Initialize file type counters */
    stats->file_type_count = 0;

    while (fgets(filename, sizeof(filename), fp) != NULL) {
        /* Remove trailing newline */
        remove_trailing_newline(filename);

        /* Skip empty filenames */
        if (strlen(filename) == 0) continue;

        file_count++;

        /* Count lines in file */
        int lines = count_lines_in_file(filename);
        if (lines >= 0) {
            total_lines += lines;
        }

        /* Get file extension and update statistics */
        char extension[MAX_EXTENSION_LENGTH];
        get_file_extension(filename, extension, sizeof(extension));

        /* Find or create file type entry */
        int found = 0;
        for (int i = 0; i < stats->file_type_count; i++) {
            if (strcmp(stats->file_types[i].extension, extension) == 0) {
                stats->file_types[i].count++;
                if (lines >= 0) {
                    stats->file_types[i].total_lines += lines;
                }
                found = 1;
                break;
            }
        }

        if (!found && stats->file_type_count < MAX_FILE_TYPES) {
            safe_string_copy(stats->file_types[stats->file_type_count].extension,
                           extension, sizeof(stats->file_types[stats->file_type_count].extension));
            stats->file_types[stats->file_type_count].count = 1;
            stats->file_types[stats->file_type_count].total_lines = (lines >= 0) ? lines : 0;
            stats->file_type_count++;
        }
    }
    pclose(fp);

    stats->total_files = file_count;
    stats->total_lines = total_lines;

    return 0;
}

/**
 * Comparison function for sorting file types by count
 */
int compare_file_types_by_count(const void* a, const void* b) {
    const FileType* type_a = (const FileType*)a;
    const FileType* type_b = (const FileType*)b;

    /* Sort in descending order */
    if (type_a->count < type_b->count) return 1;
    if (type_a->count > type_b->count) return -1;
    return 0;
}