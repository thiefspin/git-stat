# C Coding Standards and Best Practices

This document outlines the C coding standards and best practices implemented in the Git Statistics Utility project.

## Table of Contents

1. [Memory Management](#memory-management)
2. [Error Handling](#error-handling)
3. [Buffer Safety](#buffer-safety)
4. [Function Design](#function-design)
5. [Code Organization](#code-organization)
6. [Documentation](#documentation)
7. [Security Considerations](#security-considerations)
8. [Performance](#performance)
9. [Standards Compliance](#standards-compliance)
10. [Modular Architecture](#modular-architecture)

## Memory Management

### Dynamic Memory Allocation
- **Always check malloc() return values** before using allocated memory
- **Pair every malloc() with free()** to prevent memory leaks
- **Set pointers to NULL after freeing** to prevent double-free errors

```c
char *result = malloc(MAX_LINE_LENGTH);
if (result == NULL) {
    pclose(fp);
    return NULL;
}
// Use result...
free(result);
result = NULL;  // Good practice
```

### Resource Management
- **Close file handles** with proper error checking
- **Close pipes** using pclose() for popen()
- **Check for errors** on file operations using ferror()

```c
FILE *file = fopen(filename, "r");
if (file == NULL) {
    return -1;  // Error handling
}

// ... use file ...

if (ferror(file)) {
    fclose(file);
    return -1;  // Check for read errors
}
fclose(file);
```

## Error Handling

### Return Value Conventions
- **Use consistent return codes**:
  - `0` for success
  - `-1` or non-zero for errors
  - Specific error codes for different error types

```c
#define EXIT_SUCCESS_CODE 0
#define EXIT_ERROR_CODE 1
#define EXIT_NOT_GIT_REPO 2
```

### Graceful Degradation
- **Continue operation when possible** with warnings instead of fatal errors
- **Provide meaningful error messages** to stderr

```c
if (get_author_stats(stats) != 0) {
    fprintf(stderr, "Warning: Failed to get author statistics\n");
    // Continue with other statistics
}
```

### Input Validation
- **Check all function parameters** using assertions or explicit checks
- **Validate system call results** before using them

```c
static int get_commit_stats(GitStats *stats) {
    assert(stats != NULL);  // Parameter validation
    
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
```

## Buffer Safety

### String Operations
- **Always use safe string functions** with explicit length limits
- **Ensure null termination** of strings

```c
static void safe_string_copy(char* dest, const char* src, size_t dest_size) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(dest_size > 0);
    
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';  // Ensure null termination
}
```

### Buffer Bounds Checking
- **Check array bounds** before accessing elements
- **Validate buffer sizes** in snprintf() calls

```c
int ret = snprintf(command, sizeof(command), 
        "git log --author=\"%s\" --pretty=tformat: --numstat 2>/dev/null | "
        "awk '{add+=$1; del+=$2} END {print add\" \"del}'", 
        stats->authors[i].name);

if (ret < 0 || ret >= (int)sizeof(command)) {
    continue; /* Command too long, skip this author */
}
```

### Constant Definitions
- **Use named constants** instead of magic numbers
- **Define buffer sizes** explicitly

```c
#define MAX_LINE_LENGTH 1024
#define MAX_NAME_LENGTH 256
#define MAX_EXTENSION_LENGTH 16
```

## Function Design

### Single Responsibility
- **Each function should have one clear purpose**
- **Keep functions small and focused** (typically < 50 lines)

### Function Signatures
- **Use const for read-only parameters**
- **Pass structures by pointer** for efficiency
- **Use static for internal functions**

```c
static void print_stats(const GitStats *stats);
static int get_file_stats(GitStats *stats);
static void safe_string_copy(char* dest, const char* src, size_t dest_size);
```

### Return Values
- **Use meaningful return types**
- **Document return value conventions** in comments

```c
/**
 * Count lines in a file
 * @param filename Path to the file
 * @return Number of lines, or -1 on error
 */
static int count_lines_in_file(const char* filename);
```

## Code Organization

### Header Files
- **Use include guards** or #pragma once
- **Include only necessary headers**
- **Group includes logically** (system, local)

```c
#include <stdio.h>      // System headers first
#include <stdlib.h>
#include <string.h>
// ... other system headers
#include "version.h"    // Local headers last
```

### Structure Definitions
- **Use meaningful structure names**
- **Document structure members**
- **Use appropriate data types** for the data range

```c
/**
 * Author statistics structure
 */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int commit_count;
    int lines_added;
    int lines_deleted;
} Author;
```

### Global Constants
- **Use #define for compile-time constants**
- **Group related constants together**
- **Use descriptive names**

## Documentation

### Function Documentation
- **Document all public functions** with purpose, parameters, and return values
- **Use consistent documentation style**

```c
/**
 * Execute a git command and return its output
 * Caller is responsible for freeing the returned string
 * @param command Git command to execute
 * @return Allocated string with command output, or NULL on error
 */
static char* execute_git_command(const char* command);
```

### Code Comments
- **Explain complex algorithms** and business logic
- **Document assumptions** and limitations
- **Use block comments for sections**

```c
/* Get line statistics for top authors */
for (int i = 0; i < author_count && i < MAX_AUTHORS_DISPLAY; i++) {
    // Complex git command construction...
}
```

## Security Considerations

### Input Sanitization
- **Validate all external input** (command line, git output)
- **Use safe command construction** with proper escaping

```c
/* Redirect stderr to prevent command injection feedback */
result = execute_git_command("git branch --show-current 2>/dev/null");
```

### Buffer Overflow Prevention
- **Always check buffer boundaries**
- **Use safe string functions**
- **Validate input lengths**

### Command Injection Prevention
- **Avoid system() calls** when possible
- **Use popen() with careful command construction**
- **Validate and escape shell metacharacters**

## Performance

### Efficient Algorithms
- **Use appropriate data structures** (arrays for small collections)
- **Use qsort() for sorting** instead of bubble sort for larger datasets

```c
/* Sort by count using standard library */
qsort(temp_types, stats->file_type_count, sizeof(FileType), 
      compare_file_types_by_count);
```

### Memory Efficiency
- **Use stack allocation** for small, fixed-size buffers
- **Minimize dynamic allocation** in loops
- **Reuse buffers** when possible

### I/O Efficiency
- **Batch git operations** when possible
- **Use buffered I/O** (already provided by stdio)
- **Close files promptly** to free system resources

## Standards Compliance

### C17 Standard
- **Use C17 standard features** consistently
- **Avoid compiler-specific extensions**
- **Use standard library functions**

### Compiler Warnings
- **Enable all warnings** (-Wall -Wextra)
- **Treat warnings as errors** in development
- **Address all compiler warnings**

### Portability
- **Use POSIX functions** for cross-platform compatibility
- **Avoid platform-specific code** when possible
- **Use feature detection** for optional functionality

## Code Quality Checklist

### Before Committing
- [ ] All functions have proper error handling
- [ ] Memory allocations are paired with deallocations
- [ ] Buffer operations are bounds-checked
- [ ] Functions are documented
- [ ] Code compiles without warnings
- [ ] All assertions pass
- [ ] Manual testing completed

### Review Criteria
- [ ] Code follows consistent style
- [ ] Functions have single responsibility
- [ ] Error paths are tested
- [ ] Memory leaks are prevented
- [ ] Security considerations addressed
- [ ] Performance is reasonable

## Tools and Static Analysis

### Recommended Tools
- **clang-tidy**: Static analysis and style checking
- **valgrind**: Memory leak detection (Linux/macOS)
- **cppcheck**: Additional static analysis
- **Address Sanitizer**: Runtime error detection

### Usage Examples
```bash
# Static analysis
clang-tidy main.c -- -Wall -Wextra -std=c17

# Memory leak detection
valgrind --leak-check=full ./git-stat

# Address sanitizer build
clang -fsanitize=address -g -o git-stat-debug main.c
```

## JSON Output Best Practices

### Format Consistency
- **Use consistent indentation** (2 spaces for readability)
- **Follow JSON standards** strictly for machine parsing
- **Escape special characters** in string values

```c
/* Proper JSON string escaping */
printf("      \"name\": \"%s\",\n", stats->authors[i].name);
```

### Structure Design
- **Use meaningful key names** that are self-documenting
- **Group related data** into logical objects
- **Maintain consistent data types** across similar fields

```c
printf("  \"summary\": {\n");
printf("    \"total_commits\": %d,\n", stats->total_commits);
printf("    \"total_authors\": %d,\n", stats->total_authors);
printf("  },\n");
```

### Array Handling
- **Handle empty arrays** gracefully
- **Use consistent formatting** for array elements
- **Proper comma placement** (no trailing commas)

```c
for (int i = 0; i < authors_to_show; i++) {
    printf("    {\n");
    printf("      \"name\": \"%s\",\n", stats->authors[i].name);
    printf("      \"commits\": %d\n", stats->authors[i].commit_count);
    printf("    }%s\n", (i < authors_to_show - 1) ? "," : "");
}
```

### Error Handling in JSON
- **Validate data before output** to prevent malformed JSON
- **Handle null/empty values** appropriately
- **Ensure valid JSON structure** even with missing data

## Hotspot Analysis Implementation

### Algorithm Design
- **Meaningful scoring formula** that combines frequency and magnitude of changes
- **Mathematical soundness** using sqrt to moderate the impact of very large changes
- **Intuitive results** where higher scores indicate more problematic files

```c
/**
 * Calculate hotspot score based on commits and line changes
 * Score = commits × √(lines_added + lines_deleted + 1)
 */
static double calculate_hotspot_score(int commits, int lines_added, int lines_deleted) {
    if (commits <= 0) return 0.0;
    
    int total_lines = lines_added + lines_deleted;
    
    /* The +1 prevents sqrt(0) and gives small weight to files with commits but no line data */
    return (double)commits * sqrt((double)(total_lines + 1));
}
```

### Data Collection Strategy
- **Efficient git commands** to minimize performance impact
- **Proper data aggregation** to handle large repositories
- **Robust parsing** of git output with error handling

```c
/* Use git log with specific format for efficient parsing */
fp = popen("git log --name-only --pretty=format: 2>/dev/null", "r");

/* Combine with numstat for line change data */
snprintf(command, sizeof(command),
        "git log --numstat --pretty=format: -- \"%s\" 2>/dev/null | "
        "awk '{add+=$1; del+=$2} END {print add\" \"del}'",
        filename);
```

### Performance Considerations
- **Limit results** to prevent overwhelming output (top 15 hotspots)
- **Sort efficiently** using qsort for large datasets
- **Memory management** with fixed-size arrays to avoid fragmentation
- **Optional analysis** only when requested to maintain fast default operation

### Interpretation Guidelines
- **High commit count** + **moderate line changes** = Frequent maintenance
- **Moderate commits** + **high line changes** = Major refactoring
- **High commits** + **high line changes** = Potential design issues
- **Consider context** - configuration files vs. core logic files

## Author Activity Analysis Implementation

### Temporal Data Collection
- **Efficient git log parsing** with specific format strings for date extraction
- **Robust date handling** using standard C time functions
- **Activity scoring algorithms** that balance recency, frequency, and impact

```c
/* Use git log with date format for temporal analysis */
fp = popen("git log --pretty=format:'%an|%ad|%s' --date=short --all 2>/dev/null", "r");

/* Parse author|date|subject format safely */
char *author = strtok(line, "|");
char *date = strtok(NULL, "|");
```

### Activity Scoring Formula
- **Multi-factor scoring** combining commits, recency, and line changes
- **Logarithmic scaling** for line changes to prevent outliers from dominating
- **Recency weighting** with exponential decay for older activity

```c
/**
 * Calculate activity score: commits × (10000 / (days_since_last + 1)) × log(lines + 1)
 * This balances frequency, recency, and impact
 */
static double calculate_activity_score(int commits, int days_since_last, int lines_changed) {
    if (commits <= 0) return 0.0;
    
    double recency_factor = 10000.0 / (double)(days_since_last + 1);
    double lines_factor = log((double)(lines_changed + 1));
    
    return (double)commits * recency_factor * lines_factor;
}
```

### Date Calculation Best Practices
- **Safe date parsing** with error handling for invalid formats
- **Cross-platform time handling** using standard C library functions
- **Reasonable fallbacks** for missing or malformed dates

```c
/* Parse YYYY-MM-DD format with validation */
int year, month, day;
if (sscanf(commit_date, "%d-%d-%d", &year, &month, &day) != 3) {
    return 9999; /* Invalid date format */
}

/* Use mktime for proper date arithmetic */
time_t commit_time = mktime(&commit_tm);
if (commit_time == -1) {
    return 9999; /* Invalid date */
}
```

### Activity Classification
- **Clear thresholds** for active vs inactive contributors (90 days)
- **Statistical summaries** for team health metrics
- **Meaningful categorization** (single-commit vs regular contributors)

### Performance Considerations
- **Batch processing** of git log output to minimize subprocess overhead
- **Efficient sorting** using standard library qsort
- **Memory-conscious** data structures with fixed limits
- **Optional analysis** to maintain fast default performance

This coding standard ensures our C code is:
- **Safe**: Protected against buffer overflows and memory leaks
- **Reliable**: Proper error handling and resource management
- **Maintainable**: Well-documented and consistently structured
- **Portable**: Standards-compliant and cross-platform
- **Secure**: Protected against common vulnerabilities
- **Interoperable**: Produces valid, parseable output formats
- **Analytical**: Provides actionable insights for code quality improvement
- **Temporal**: Handles time-based analysis with proper date arithmetic
- **Modular**: Well-organized with clear separation of concerns

## Modular Architecture

### Project Organization
- **Logical grouping** of related functionality into modules
- **Clear directory structure** reflecting code organization
- **Separation of concerns** with each module having a single responsibility

```
src/
├── main.c              # Entry point - minimal, focused on program flow
├── git_stats.h/.c      # Core data structures and basic operations
├── analysis/           # Analysis algorithms (hotspots, activity)
├── output/             # Formatting and display logic
└── utils/              # Reusable utility functions
```

### Module Design Principles
- **Single Responsibility**: Each module handles one aspect of functionality
- **Clear Interfaces**: Header files define clean APIs between modules
- **Minimal Dependencies**: Modules only depend on what they actually need
- **Consistent Error Handling**: Standardized error reporting across modules

```c
/* Good: Clear module interface */
int get_hotspot_stats(GitStats *stats);
double calculate_hotspot_score(int commits, int lines_added, int lines_deleted);

/* Good: Focused utility functions */
void safe_string_copy(char* dest, const char* src, size_t dest_size);
char* execute_git_command(const char* command);
```

### Header File Best Practices
- **Include guards** to prevent multiple inclusion
- **Forward declarations** to minimize dependencies
- **Documentation** of all public functions and data structures
- **Logical grouping** of related declarations

```c
#ifndef GIT_STATS_H
#define GIT_STATS_H

/* Core data structures */
typedef struct { ... } GitStats;

/* Public API functions */
int get_basic_git_stats(GitStats *stats);
void init_git_stats(GitStats *stats);

#endif /* GIT_STATS_H */
```

### Build System Organization
- **Modular compilation** with individual object files per module
- **Proper dependency tracking** in Makefile rules
- **Parallel build support** with correct dependency chains
- **Clean separation** of build artifacts by module

```makefile
# Organized object file definitions
OBJS = $(SRCDIR)/main.o \
       $(SRCDIR)/git_stats.o \
       $(ANALYSISDIR)/hotspots.o \
       $(OUTPUTDIR)/human_output.o

# Clear dependency relationships
$(ANALYSISDIR)/hotspots.o: $(ANALYSISDIR)/hotspots.c $(SRCDIR)/git_stats.h
```

### Benefits of Modular Design
- **Maintainability**: Easier to locate and fix bugs in specific modules
- **Testability**: Individual modules can be unit tested in isolation
- **Collaboration**: Multiple developers can work on different modules simultaneously
- **Extensibility**: New features can be added as new modules without affecting existing code
- **Reusability**: Utility modules can be reused across different parts of the application