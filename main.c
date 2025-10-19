#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include "version.h"

#define MAX_LINE_LENGTH 1024
#define MAX_COMMAND_LENGTH 512
#define MAX_AUTHORS 100
#define MAX_BRANCHES 50
#define MAX_FILES 1000

typedef struct {
    char name[256];
    int commit_count;
    int lines_added;
    int lines_deleted;
} Author;

typedef struct {
    char name[256];
    int commit_count;
    char last_commit[20];
} Branch;

typedef struct {
    char extension[16];
    int count;
    long total_lines;
} FileType;

typedef struct {
    int total_commits;
    int total_authors;
    int total_branches;
    int total_files;
    long total_lines;
    char current_branch[256];
    char repo_name[256];
    Author authors[MAX_AUTHORS];
    Branch branches[MAX_BRANCHES];
    FileType file_types[50];
    int file_type_count;
} GitStats;

// Function prototypes
int is_git_repository(void);
void get_git_stats(GitStats *stats);
void get_repository_info(GitStats *stats);
void get_commit_stats(GitStats *stats);
void get_author_stats(GitStats *stats);
void get_branch_stats(GitStats *stats);
void get_file_stats(GitStats *stats);
void print_stats(const GitStats *stats);
void print_help(void);
char* execute_git_command(const char* command);
int count_lines_in_file(const char* filename);
void get_file_extension(const char* filename, char* extension);

int main(int argc, char *argv[]) {
    printf("%s v%s\n", PROGRAM_NAME, VERSION_STRING);
    printf("============================\n\n");

    // Check for help flag
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        print_help();
        return 0;
    }
    
    // Check for version flag
    if (argc > 1 && (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)) {
        printf("%s\n", get_version_string());
        return 0;
    }

    // Check if we're in a git repository
    if (!is_git_repository()) {
        fprintf(stderr, "Error: Not a git repository (or any of the parent directories)\n");
        fprintf(stderr, "Run this command from within a git repository.\n");
        return 1;
    }

    GitStats stats = {0};
    get_git_stats(&stats);
    print_stats(&stats);

    return 0;
}

int is_git_repository(void) {
    struct stat st;
    return (stat(".git", &st) == 0 && S_ISDIR(st.st_mode));
}

void get_git_stats(GitStats *stats) {
    printf("Analyzing repository...\n");
    
    get_repository_info(stats);
    get_commit_stats(stats);
    get_author_stats(stats);
    get_branch_stats(stats);
    get_file_stats(stats);
    
    printf("Analysis complete!\n\n");
}

void get_repository_info(GitStats *stats) {
    char *result;
    
    // Get current branch
    result = execute_git_command("git branch --show-current");
    if (result) {
        strncpy(stats->current_branch, result, sizeof(stats->current_branch) - 1);
        // Remove newline
        char *newline = strchr(stats->current_branch, '\n');
        if (newline) *newline = '\0';
        free(result);
    }
    
    // Get repository name from current directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char *repo_name = strrchr(cwd, '/');
        if (repo_name) {
            strncpy(stats->repo_name, repo_name + 1, sizeof(stats->repo_name) - 1);
        }
    }
}

void get_commit_stats(GitStats *stats) {
    char *result = execute_git_command("git rev-list --all --count");
    if (result) {
        stats->total_commits = atoi(result);
        free(result);
    }
}

void get_author_stats(GitStats *stats) {
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    
    // Get author commit counts
    fp = popen("git shortlog -sn --all", "r");
    if (fp == NULL) return;
    
    int author_count = 0;
    while (fgets(line, sizeof(line), fp) && author_count < MAX_AUTHORS) {
        char *tab = strchr(line, '\t');
        if (tab) {
            *tab = '\0';
            stats->authors[author_count].commit_count = atoi(line);
            strncpy(stats->authors[author_count].name, tab + 1, sizeof(stats->authors[author_count].name) - 1);
            
            // Remove newline
            char *newline = strchr(stats->authors[author_count].name, '\n');
            if (newline) *newline = '\0';
            
            author_count++;
        }
    }
    pclose(fp);
    stats->total_authors = author_count;
    
    // Get line statistics for top authors
    for (int i = 0; i < author_count && i < 10; i++) {
        char command[512];
        snprintf(command, sizeof(command), 
                "git log --author=\"%s\" --pretty=tformat: --numstat | awk '{add+=$1; del+=$2} END {print add\" \"del}'", 
                stats->authors[i].name);
        
        char *result = execute_git_command(command);
        if (result) {
            sscanf(result, "%d %d", &stats->authors[i].lines_added, &stats->authors[i].lines_deleted);
            free(result);
        }
    }
}

void get_branch_stats(GitStats *stats) {
    FILE *fp = popen("git branch -a", "r");
    if (fp == NULL) return;
    
    char line[MAX_LINE_LENGTH];
    int branch_count = 0;
    
    while (fgets(line, sizeof(line), fp) && branch_count < MAX_BRANCHES) {
        // Skip remote branches and current branch marker
        if (strstr(line, "remotes/") || strlen(line) < 3) continue;
        
        char *branch_name = line;
        if (line[0] == '*' || line[0] == ' ') branch_name += 2;
        
        // Remove newline
        char *newline = strchr(branch_name, '\n');
        if (newline) *newline = '\0';
        
        strncpy(stats->branches[branch_count].name, branch_name, sizeof(stats->branches[branch_count].name) - 1);
        
        // Get commit count for this branch
        char command[512];
        snprintf(command, sizeof(command), "git rev-list --count %s", branch_name);
        char *result = execute_git_command(command);
        if (result) {
            stats->branches[branch_count].commit_count = atoi(result);
            free(result);
        }
        
        branch_count++;
    }
    pclose(fp);
    stats->total_branches = branch_count;
}

void get_file_stats(GitStats *stats) {
    FILE *fp = popen("git ls-files", "r");
    if (fp == NULL) return;
    
    char filename[MAX_LINE_LENGTH];
    int file_count = 0;
    long total_lines = 0;
    
    // Initialize file type counters
    stats->file_type_count = 0;
    
    while (fgets(filename, sizeof(filename), fp)) {
        // Remove newline
        char *newline = strchr(filename, '\n');
        if (newline) *newline = '\0';
        
        file_count++;
        
        // Count lines in file
        int lines = count_lines_in_file(filename);
        total_lines += lines;
        
        // Get file extension and update statistics
        char extension[16];
        get_file_extension(filename, extension);
        
        // Find or create file type entry
        int found = 0;
        for (int i = 0; i < stats->file_type_count; i++) {
            if (strcmp(stats->file_types[i].extension, extension) == 0) {
                stats->file_types[i].count++;
                stats->file_types[i].total_lines += lines;
                found = 1;
                break;
            }
        }
        
        if (!found && stats->file_type_count < 50) {
            strncpy(stats->file_types[stats->file_type_count].extension, extension, 
                   sizeof(stats->file_types[stats->file_type_count].extension) - 1);
            stats->file_types[stats->file_type_count].count = 1;
            stats->file_types[stats->file_type_count].total_lines = lines;
            stats->file_type_count++;
        }
    }
    pclose(fp);
    
    stats->total_files = file_count;
    stats->total_lines = total_lines;
}

char* execute_git_command(const char* command) {
    FILE *fp = popen(command, "r");
    if (fp == NULL) return NULL;
    
    char *result = malloc(MAX_LINE_LENGTH);
    if (result && fgets(result, MAX_LINE_LENGTH, fp)) {
        pclose(fp);
        return result;
    }
    
    pclose(fp);
    if (result) free(result);
    return NULL;
}

int count_lines_in_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 0;
    
    int lines = 0;
    char ch;
    
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') lines++;
    }
    
    fclose(file);
    return lines;
}

void get_file_extension(const char* filename, char* extension) {
    const char *dot = strrchr(filename, '.');
    if (dot && dot != filename) {
        strncpy(extension, dot + 1, 15);
        extension[15] = '\0';
    } else {
        strcpy(extension, "no_ext");
    }
}

void print_stats(const GitStats *stats) {
    printf("Repository Statistics for: %s\n", stats->repo_name);
    printf("==========================================\n\n");
    
    printf("📊 General Information:\n");
    printf("  Current Branch: %s\n", stats->current_branch);
    printf("  Total Commits: %d\n", stats->total_commits);
    printf("  Total Authors: %d\n", stats->total_authors);
    printf("  Total Branches: %d\n", stats->total_branches);
    printf("  Total Files: %d\n", stats->total_files);
    printf("  Total Lines of Code: %ld\n\n", stats->total_lines);
    
    printf("👥 Top Contributors:\n");
    for (int i = 0; i < stats->total_authors && i < 10; i++) {
        printf("  %2d. %-30s %4d commits", i + 1, stats->authors[i].name, stats->authors[i].commit_count);
        if (stats->authors[i].lines_added > 0 || stats->authors[i].lines_deleted > 0) {
            printf(" (+%d/-%d lines)", stats->authors[i].lines_added, stats->authors[i].lines_deleted);
        }
        printf("\n");
    }
    printf("\n");
    
    printf("🌿 Branches:\n");
    for (int i = 0; i < stats->total_branches && i < 10; i++) {
        printf("  %-20s %4d commits\n", stats->branches[i].name, stats->branches[i].commit_count);
    }
    if (stats->total_branches > 10) {
        printf("  ... and %d more branches\n", stats->total_branches - 10);
    }
    printf("\n");
    
    printf("📁 File Types:\n");
    // Sort file types by count (simple bubble sort for small arrays)
    FileType temp_types[50];
    memcpy(temp_types, stats->file_types, sizeof(FileType) * stats->file_type_count);
    
    for (int i = 0; i < stats->file_type_count - 1; i++) {
        for (int j = 0; j < stats->file_type_count - i - 1; j++) {
            if (temp_types[j].count < temp_types[j + 1].count) {
                FileType temp = temp_types[j];
                temp_types[j] = temp_types[j + 1];
                temp_types[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < stats->file_type_count && i < 10; i++) {
        printf("  %-10s %4d files, %8ld lines\n", 
               temp_types[i].extension, temp_types[i].count, temp_types[i].total_lines);
    }
    printf("\n");
}

void print_help(void) {
    printf("%s v%s\n", PROGRAM_NAME, VERSION_STRING);
    printf("============================\n\n");
    printf("Usage: git-stat [OPTIONS]\n\n");
    printf("%s\n\n", PROGRAM_DESCRIPTION);
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -v, --version  Show version information\n\n");
    printf("Features:\n");
    printf("  • Repository overview (commits, authors, branches, files)\n");
    printf("  • Top contributors with commit counts and line changes\n");
    printf("  • Branch information and commit counts\n");
    printf("  • File type analysis with line counts\n");
    printf("  • Works completely offline with local git data\n\n");
    printf("Examples:\n");
    printf("  git-stat          # Analyze current repository\n");
    printf("  git-stat --help   # Show this help\n");
    printf("  git-stat --version # Show version info\n\n");
    printf("Note: Must be run from within a git repository.\n\n");
    printf("%s\n", PROGRAM_COPYRIGHT);
    printf("License: %s\n", PROGRAM_LICENSE);
}