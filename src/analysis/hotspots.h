#ifndef HOTSPOTS_H
#define HOTSPOTS_H

#include "../git_stats.h"

/**
 * Get file hotspot statistics
 * @param stats GitStats structure to populate with hotspot data
 * @return 0 on success, -1 on error
 */
int get_hotspot_stats(GitStats *stats);



#endif /* HOTSPOTS_H */