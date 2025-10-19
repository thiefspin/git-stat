#include "string_utils.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/**
 * Safe string copy with bounds checking
 */
void safe_string_copy(char* dest, const char* src, size_t dest_size) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(dest_size > 0);
    
    strncpy(dest, src, dest_size - 1); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    dest[dest_size - 1] = '\0';
}

/**
 * Remove trailing newline from string
 */
void remove_trailing_newline(char* str) {
    assert(str != NULL);
    
    char *newline = strchr(str, '\n');
    if (newline != NULL) {
        *newline = '\0';
    }
}

/**
 * Extract file extension from filename
 */
void get_file_extension(const char* filename, char* extension, size_t extension_size) {
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
 * Calculate days since commit date (approximate)
 */
int calculate_days_since_commit(const char* commit_date) {
    if (commit_date == NULL || strlen(commit_date) == 0) {
        return 9999; /* Very old if no date available */
    }
    
    /* Parse YYYY-MM-DD format */
    int year, month, day;
    if (sscanf(commit_date, "%d-%d-%d", &year, &month, &day) != 3) { // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        return 9999; /* Invalid date format */
    }
    
    /* Get current time */
    time_t now;
    time(&now);
    
    /* Create tm structure for commit date */
    struct tm commit_tm = {0};
    commit_tm.tm_year = year - 1900;
    commit_tm.tm_mon = month - 1;
    commit_tm.tm_mday = day;
    
    /* Calculate difference in seconds and convert to days */
    time_t commit_time = mktime(&commit_tm);
    if (commit_time == -1) {
        return 9999; /* Invalid date */
    }
    
    double diff_seconds = difftime(now, commit_time);
    int days = (int)(diff_seconds / (24 * 60 * 60));
    
    return (days < 0) ? 0 : days;
}