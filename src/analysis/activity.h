#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "../git_stats.h"

/**
 * Get author activity statistics over time
 * @param stats GitStats structure to populate with activity data
 * @return 0 on success, -1 on error
 */
int get_activity_stats(GitStats *stats);

/**
 * Calculate activity score based on commits, recency, and line changes
 * @param commits Number of commits by the author
 * @param days_since_last Days since the author's last commit
 * @param lines_changed Total lines added and deleted by the author
 * @return Calculated activity score
 */
double calculate_activity_score(int commits, int days_since_last, int lines_changed);

/**
 * Comparison function for sorting activities by score
 * @param a First activity to compare
 * @param b Second activity to compare
 * @return Comparison result for qsort (descending order)
 */
int compare_activities_by_score(const void* a, const void* b);

#endif /* ACTIVITY_H */