#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0
#define VERSION_STRING "1.0.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

// Git information (to be filled by build system)
#ifndef GIT_COMMIT
#define GIT_COMMIT "unknown"
#endif

#ifndef GIT_BRANCH
#define GIT_BRANCH "unknown"
#endif

// Feature flags
#define FEATURE_COLOR_OUTPUT 1
#define FEATURE_JSON_EXPORT 1
#define FEATURE_CSV_EXPORT 0

// Program metadata
#define PROGRAM_NAME "Git Statistics Utility"
#define PROGRAM_DESCRIPTION "A lightweight, fast C utility for git repository analysis"
#define PROGRAM_COPYRIGHT "Copyright (c) 2024 Git-Stat Contributors"
#define PROGRAM_LICENSE "MIT License"
#define PROGRAM_WEBSITE "https://github.com/yourusername/git-stat"

// Version comparison macros
#define VERSION_CODE(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))

#define CURRENT_VERSION_CODE \
    VERSION_CODE(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH)

/* Inline functions for version information */

/**
 * Get full version string with build information
 * @return Formatted version string
 */
static inline const char* get_version_string(void) {
    static char version_buffer[256];
    snprintf(version_buffer, sizeof(version_buffer),
             "%s v%s (built %s %s, commit %s)",
             PROGRAM_NAME, VERSION_STRING, BUILD_DATE, BUILD_TIME, GIT_COMMIT);
    return version_buffer;
}

/**
 * Get short version string
 * @return Version string only
 */
static inline const char* get_short_version(void) {
    return VERSION_STRING;
}

/**
 * Check if this is a development version
 * @return 1 if development version, 0 otherwise
 */
static inline int is_development_version(void) {
    return (VERSION_PATCH == 0 && strcmp(GIT_COMMIT, "unknown") != 0);
}

#endif /* VERSION_H */