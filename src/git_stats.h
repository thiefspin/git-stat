#ifndef GIT_STATS_H
#define GIT_STATS_H

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
#include <math.h>

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
#define MAX_FILES 1000

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

/* Analysis modes */
typedef enum {
    ANALYSIS_BASIC,
    ANALYSIS_HOTSPOTS,
    ANALYSIS_ACTIVITY
} AnalysisMode;

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
 * Author activity structure for temporal analysis
 */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int commit_count;
    int lines_added;
    int lines_deleted;
    char first_commit_date[32];
    char last_commit_date[32];
    int days_since_last_commit;
    int is_active;  /* 1 if active (committed within last 90 days), 0 otherwise */
    double activity_score;
} AuthorActivity;

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
 * File hotspot structure for churn analysis
 */
typedef struct {
    char filename[MAX_PATH_LENGTH];
    int commit_count;
    int lines_added;
    int lines_deleted;
    double hotspot_score;
} FileHotspot;

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
    FileHotspot hotspots[MAX_FILES];
    int hotspot_count;
    AuthorActivity activities[MAX_AUTHORS];
    int activity_count;
} GitStats;

/* Core API functions */
int is_git_repository(void);
void init_git_stats(GitStats *stats);
int get_basic_git_stats(GitStats *stats);
int get_repository_info(GitStats *stats);
int get_commit_stats(GitStats *stats);
int get_author_stats(GitStats *stats);
int get_branch_stats(GitStats *stats);
int get_file_stats(GitStats *stats);

/* Comparison functions for sorting */
int compare_file_types_by_count(const void* a, const void* b);

#endif /* GIT_STATS_H */