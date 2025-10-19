#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include "version.h"

/* Buffer size constants */
#define MAX_LINE_LENGTH 1024
#define MAX_COMMAND_LENGTH 512
#define MAX_PATH_LENGTH 1024
#define MAX_NAME_LENGTH 256
#define MAX_EXTENSION_LENGTH 16

/* Collection size limits */
#define MAX_AUTHORS 100
#define MAX_BRANCHES 50
#define MAX_FILE_TYPES 50

/* Display limits */
#define MAX_AUTHORS_DISPLAY 10
#define MAX_BRANCHES_DISPLAY 10
#define MAX_FILE_TYPES_DISPLAY 10

/* Return codes */
#define EXIT_SUCCESS_CODE 0
#define EXIT_ERROR_CODE 1
#define EXIT_NOT_GIT_REPO 2
#define EXIT_HELP_SHOWN 3
#define EXIT_VERSION_SHOWN 4

/* Output formats */
typedef enum {
    OUTPUT_DEFAULT,
    OUTPUT_JSON
} OutputFormat;

/**
 * Author statistics structure
 */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int commit_count;
    int lines_added;
    int lines_deleted;
} Author;

/**
 * Branch information structure
 */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int commit_count;
    char last_commit[41]; /* Git SHA-1 hash length */
} Branch;

/**
 * File type statistics structure
 */
typedef struct {
    char extension[MAX_EXTENSION_LENGTH];
    int count;
    long total_lines;
} FileType;

/**
 * Main statistics container
 */
typedef struct {
    int total_commits;
    int total_authors;
    int total_branches;
    int total_files;
    long total_lines;
    char current_branch[MAX_NAME_LENGTH];
    char repo_name[MAX_NAME_LENGTH];
    Author authors[MAX_AUTHORS];
    Branch branches[MAX_BRANCHES];
    FileType file_types[MAX_FILE_TYPES];
    int file_type_count;
} GitStats;

/* Function prototypes */
static int is_git_repository(void);
static int get_git_stats(GitStats *stats);
static int get_repository_info(GitStats *stats);
static int get_commit_stats(GitStats *stats);
static int get_author_stats(GitStats *stats);
static int get_branch_stats(GitStats *stats);
static int get_file_stats(GitStats *stats);
static void print_stats(const GitStats *stats);
static void print_stats_json(const GitStats *stats);
static void print_help(void);
static int parse_arguments(int argc, char *argv[], OutputFormat *format);
static char* execute_git_command(const char* command);
static int count_lines_in_file(const char* filename);
static void get_file_extension(const char* filename, char* extension, size_t extension_size);
static void safe_string_copy(char* dest, const char* src, size_t dest_size);
static void remove_trailing_newline(char* str);
static int compare_file_types_by_count(const void* a, const void* b);
static void init_git_stats(GitStats *stats);

/**
 * Main entry point
 */
int main(int argc, char *argv[]) {
    OutputFormat output_format = OUTPUT_DEFAULT;
    
    /* Parse command line arguments */
    int parse_result = parse_arguments(argc, argv, &output_format);
    if (parse_result == EXIT_HELP_SHOWN || parse_result == EXIT_VERSION_SHOWN) {
        return EXIT_SUCCESS_CODE;
    }
    if (parse_result != 0) {
        return parse_result;
    }
    
    /* Print header for default output only */
    if (output_format == OUTPUT_DEFAULT) {
        printf("%s v%s\n", PROGRAM_NAME, VERSION_STRING);
        printf("============================\n\n");
    }

    /* Verify we're in a git repository */
    if (!is_git_repository()) {
        fprintf(stderr, "Error: Not a git repository (or any of the parent directories)\n");
        fprintf(stderr, "Run this command from within a git repository.\n");
        return EXIT_NOT_GIT_REPO;
    }

    /* Initialize and gather statistics */
    GitStats stats;
    init_git_stats(&stats);
    
    if (get_git_stats(&stats) != 0) {
        fprintf(stderr, "Error: Failed to gather git statistics\n");
        return EXIT_ERROR_CODE;
    }

    /* Output results in requested format */
    if (output_format == OUTPUT_JSON) {
        print_stats_json(&stats);
    } else {
        print_stats(&stats);
    }
    
    return EXIT_SUCCESS_CODE;
}

/**
 * Check if current directory is a git repository
 */
static int is_git_repository(void) {
    struct stat st;
    return (stat(".git", &st) == 0 && S_ISDIR(st.st_mode));
}

/**
 * Initialize GitStats structure with default values
 */
static void init_git_stats(GitStats *stats) {
    assert(stats != NULL);
    
    memset(stats, 0, sizeof(GitStats));
    safe_string_copy(stats->current_branch, "unknown", sizeof(stats->current_branch));
    safe_string_copy(stats->repo_name, "unknown", sizeof(stats->repo_name));
}

/**
 * Gather all git statistics
 */
static int get_git_stats(GitStats *stats) {
    assert(stats != NULL);
    
    /* Only show progress for default output */
    
    if (get_repository_info(stats) != 0) {
        fprintf(stderr, "Warning: Failed to get repository information\n");
    }
    
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
        char *repo_name = strrchr(cwd, '/');
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
 * Execute a git command and return its output
 * Caller is responsible for freeing the returned string
 */
static char* execute_git_command(const char* command) {
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
static int count_lines_in_file(const char* filename) {
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

/**
 * Extract file extension from filename
 */
static void get_file_extension(const char* filename, char* extension, size_t extension_size) {
    assert(filename != NULL);
    assert(extension != NULL);
    assert(extension_size > 0);
    
    const char *dot = strrchr(filename, '.');
    if (dot != NULL && dot != filename && strlen(dot + 1) > 0) {
        safe_string_copy(extension, dot + 1, extension_size);
    } else {
        safe_string_copy(extension, "no_ext", extension_size);
    }
}

/**
 * Safe string copy with bounds checking
 */
static void safe_string_copy(char* dest, const char* src, size_t dest_size) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(dest_size > 0);
    
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

/**
 * Remove trailing newline from string
 */
static void remove_trailing_newline(char* str) {
    assert(str != NULL);
    
    char *newline = strchr(str, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }
}

/**
 * Comparison function for sorting file types by count
 */
static int compare_file_types_by_count(const void* a, const void* b) {
    const FileType* type_a = (const FileType*)a;
    const FileType* type_b = (const FileType*)b;
    
    /* Sort in descending order */
    if (type_a->count < type_b->count) return 1;
    if (type_a->count > type_b->count) return -1;
    return 0;
}

/**
 * Print comprehensive statistics
 */
static void print_stats(const GitStats *stats) {
    assert(stats != NULL);
    
    printf("Repository Statistics for: %s\n", stats->repo_name);
    printf("==========================================\n\n");
    
    printf("📊 General Information:\n");
    printf("  Current Branch: %s\n", stats->current_branch);
    printf("  Total Commits: %d\n", stats->total_commits);
    printf("  Total Authors: %d\n", stats->total_authors);
    printf("  Total Branches: %d\n", stats->total_branches);
    printf("  Total Files: %d\n", stats->total_files);
    printf("  Total Lines of Code: %ld\n\n", stats->total_lines);
    
    /* Print top contributors */
    printf("👥 Top Contributors:\n");
    int authors_to_show = (stats->total_authors < MAX_AUTHORS_DISPLAY) ? 
                         stats->total_authors : MAX_AUTHORS_DISPLAY;
    
    for (int i = 0; i < authors_to_show; i++) {
        printf("  %2d. %-30s %4d commits", i + 1, 
               stats->authors[i].name, stats->authors[i].commit_count);
        if (stats->authors[i].lines_added > 0 || stats->authors[i].lines_deleted > 0) {
            printf(" (+%d/-%d lines)", stats->authors[i].lines_added, 
                   stats->authors[i].lines_deleted);
        }
        printf("\n");
    }
    printf("\n");
    
    /* Print branches */
    printf("🌿 Branches:\n");
    int branches_to_show = (stats->total_branches < MAX_BRANCHES_DISPLAY) ? 
                          stats->total_branches : MAX_BRANCHES_DISPLAY;
    
    for (int i = 0; i < branches_to_show; i++) {
        printf("  %-20s %4d commits\n", stats->branches[i].name, 
               stats->branches[i].commit_count);
    }
    if (stats->total_branches > MAX_BRANCHES_DISPLAY) {
        printf("  ... and %d more branches\n", 
               stats->total_branches - MAX_BRANCHES_DISPLAY);
    }
    printf("\n");
    
    /* Print file types */
    printf("📁 File Types:\n");
    if (stats->file_type_count > 0) {
        /* Create a copy for sorting */
        FileType temp_types[MAX_FILE_TYPES];
        memcpy(temp_types, stats->file_types, 
               sizeof(FileType) * stats->file_type_count);
        
        /* Sort by count */
        qsort(temp_types, stats->file_type_count, sizeof(FileType), 
              compare_file_types_by_count);
        
        int types_to_show = (stats->file_type_count < MAX_FILE_TYPES_DISPLAY) ? 
                           stats->file_type_count : MAX_FILE_TYPES_DISPLAY;
        
        for (int i = 0; i < types_to_show; i++) {
            double percentage = (stats->total_lines > 0) ? 
                               (double)temp_types[i].total_lines * 100.0 / stats->total_lines : 0.0;
            printf("  %-10s %4d files, %8ld lines (%5.1f%%)\n", 
                   temp_types[i].extension, temp_types[i].count, 
                   temp_types[i].total_lines, percentage);
        }
        
        if (stats->file_type_count > MAX_FILE_TYPES_DISPLAY) {
            printf("  ... and %d more file types\n", 
                   stats->file_type_count - MAX_FILE_TYPES_DISPLAY);
        }
    }
    printf("\n");
}

/**
 * Print statistics in JSON format
 */
static void print_stats_json(const GitStats *stats) {
    assert(stats != NULL);
    
    printf("{\n");
    printf("  \"repository\": {\n");
    printf("    \"name\": \"%s\",\n", stats->repo_name);
    printf("    \"current_branch\": \"%s\"\n", stats->current_branch);
    printf("  },\n");
    
    printf("  \"summary\": {\n");
    printf("    \"total_commits\": %d,\n", stats->total_commits);
    printf("    \"total_authors\": %d,\n", stats->total_authors);
    printf("    \"total_branches\": %d,\n", stats->total_branches);
    printf("    \"total_files\": %d,\n", stats->total_files);
    printf("    \"total_lines\": %ld\n", stats->total_lines);
    printf("  },\n");
    
    /* Authors array */
    printf("  \"authors\": [\n");
    int authors_to_show = (stats->total_authors < MAX_AUTHORS_DISPLAY) ? 
                         stats->total_authors : MAX_AUTHORS_DISPLAY;
    for (int i = 0; i < authors_to_show; i++) {
        printf("    {\n");
        printf("      \"name\": \"%s\",\n", stats->authors[i].name);
        printf("      \"commits\": %d,\n", stats->authors[i].commit_count);
        printf("      \"lines_added\": %d,\n", stats->authors[i].lines_added);
        printf("      \"lines_deleted\": %d\n", stats->authors[i].lines_deleted);
        printf("    }%s\n", (i < authors_to_show - 1) ? "," : "");
    }
    printf("  ],\n");
    
    /* Branches array */
    printf("  \"branches\": [\n");
    int branches_to_show = (stats->total_branches < MAX_BRANCHES_DISPLAY) ? 
                          stats->total_branches : MAX_BRANCHES_DISPLAY;
    for (int i = 0; i < branches_to_show; i++) {
        printf("    {\n");
        printf("      \"name\": \"%s\",\n", stats->branches[i].name);
        printf("      \"commits\": %d\n", stats->branches[i].commit_count);
        printf("    }%s\n", (i < branches_to_show - 1) ? "," : "");
    }
    printf("  ],\n");
    
    /* File types array */
    printf("  \"file_types\": [\n");
    if (stats->file_type_count > 0) {
        /* Create a copy for sorting */
        FileType temp_types[MAX_FILE_TYPES];
        memcpy(temp_types, stats->file_types, 
               sizeof(FileType) * stats->file_type_count);
        
        /* Sort by count */
        qsort(temp_types, stats->file_type_count, sizeof(FileType), 
              compare_file_types_by_count);
        
        int types_to_show = (stats->file_type_count < MAX_FILE_TYPES_DISPLAY) ? 
                           stats->file_type_count : MAX_FILE_TYPES_DISPLAY;
        
        for (int i = 0; i < types_to_show; i++) {
            double percentage = (stats->total_lines > 0) ? 
                               (double)temp_types[i].total_lines * 100.0 / stats->total_lines : 0.0;
            printf("    {\n");
            printf("      \"extension\": \"%s\",\n", temp_types[i].extension);
            printf("      \"files\": %d,\n", temp_types[i].count);
            printf("      \"lines\": %ld,\n", temp_types[i].total_lines);
            printf("      \"percentage\": %.1f\n", percentage);
            printf("    }%s\n", (i < types_to_show - 1) ? "," : "");
        }
    }
    printf("  ]\n");
    printf("}\n");
}

/**
 * Parse command line arguments
 */
static int parse_arguments(int argc, char *argv[], OutputFormat *format) {
    assert(format != NULL);
    
    *format = OUTPUT_DEFAULT;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return EXIT_HELP_SHOWN;
        }
        
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("%s\n", get_version_string());
            return EXIT_VERSION_SHOWN;
        }
        
        if (strcmp(argv[i], "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: --output requires a format argument\n");
                fprintf(stderr, "Supported formats: json\n");
                return EXIT_ERROR_CODE;
            }
            
            i++; /* Move to format argument */
            if (strcmp(argv[i], "json") == 0) {
                *format = OUTPUT_JSON;
            } else {
                fprintf(stderr, "Error: Unknown output format '%s'\n", argv[i]);
                fprintf(stderr, "Supported formats: json\n");
                return EXIT_ERROR_CODE;
            }
        } else {
            /* Unknown argument */
            fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
            fprintf(stderr, "Use --help for usage information.\n");
            return EXIT_ERROR_CODE;
        }
    }
    
    return 0;
}

/**
 * Print help information
 */
static void print_help(void) {
    printf("%s v%s\n", PROGRAM_NAME, VERSION_STRING);
    printf("============================\n\n");
    printf("Usage: git-stat [OPTIONS]\n\n");
    printf("%s\n\n", PROGRAM_DESCRIPTION);
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -v, --version       Show version information\n");
    printf("  --output FORMAT     Output format (default: human-readable)\n");
    printf("                      Supported formats: json\n\n");
    printf("Features:\n");
    printf("  • Repository overview (commits, authors, branches, files)\n");
    printf("  • Top contributors with commit counts and line changes\n");
    printf("  • Branch information and commit counts\n");
    printf("  • File type analysis with line counts\n");
    printf("  • Works completely offline with local git data\n\n");
    printf("Examples:\n");
    printf("  git-stat                    # Analyze current repository\n");
    printf("  git-stat --output json      # Output in JSON format\n");
    printf("  git-stat --help             # Show this help\n");
    printf("  git-stat --version          # Show version info\n\n");
    printf("Exit Codes:\n");
    printf("  0  Success\n");
    printf("  1  General error\n");
    printf("  2  Not a git repository\n\n");
    printf("Note: Must be run from within a git repository.\n\n");
    printf("%s\n", PROGRAM_COPYRIGHT);
    printf("License: %s\n", PROGRAM_LICENSE);
}