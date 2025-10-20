#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>

/**
 * Safe string copy with bounds checking
 * @param dest Destination buffer
 * @param src Source string
 * @param dest_size Size of destination buffer
 */
void safe_string_copy(char* dest, const char* src, size_t dest_size);

/**
 * Remove trailing newline from string
 * @param str String to modify (modified in place)
 */
void remove_trailing_newline(char * const str);

/**
 * Extract file extension from filename
 * @param filename Input filename
 * @param extension Output buffer for extension
 * @param extension_size Size of extension buffer
 */
void get_file_extension(const char* filename, char* extension, size_t extension_size);

/**
 * Calculate days since commit date (approximate)
 * @param commit_date Date string in YYYY-MM-DD format
 * @return Number of days since the date, or 9999 if invalid
 */
int calculate_days_since_commit(const char* commit_date);

#endif /* STRING_UTILS_H */
