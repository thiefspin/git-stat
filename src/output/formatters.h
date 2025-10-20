#ifndef FORMATTERS_H
#define FORMATTERS_H

#include "../git_stats.h"

/**
 * Print comprehensive statistics in human-readable format
 * @param stats GitStats structure containing all statistics
 * @param mode Analysis mode to determine what sections to include
 */
void print_stats_human(const GitStats *stats, AnalysisMode mode);

/**
 * Print statistics in JSON format
 * @param stats GitStats structure containing all statistics
 * @param mode Analysis mode to determine what sections to include
 */
void print_stats_json(const GitStats *stats, AnalysisMode mode);



/**
 * Print help information
 */
void print_help(void);

#endif /* FORMATTERS_H */