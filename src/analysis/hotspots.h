#ifndef HOTSPOTS_H
#define HOTSPOTS_H

#include "../git_stats.h"

/**
 * Get file hotspot statistics
 * @param stats GitStats structure to populate with hotspot data
 * @return 0 on success, -1 on error
 */
int get_hotspot_stats(GitStats *stats);

/**
 * Calculate hotspot score based on commits and line changes
 * @param commits Number of commits affecting the file
 * @param lines_added Lines added to the file
 * @param lines_deleted Lines deleted from the file
 * @return Calculated hotspot score
 */
double calculate_hotspot_score(int commits, int lines_added, int lines_deleted);

/**
 * Comparison function for sorting hotspots by score
 * @param a First hotspot to compare
 * @param b Second hotspot to compare
 * @return Comparison result for qsort (descending order)
 */
int compare_hotspots_by_score(const void* a, const void* b);

#endif /* HOTSPOTS_H */