#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "../git_stats.h"

/**
 * Get author activity statistics over time
 * @param stats GitStats structure to populate with activity data
 * @return 0 on success, -1 on error
 */
int get_activity_stats(GitStats *stats);



#endif /* ACTIVITY_H */