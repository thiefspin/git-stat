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
 * Print hotspot analysis in human-readable format
 * @param stats GitStats structure containing hotspot data
 */
void print_hotspots_human(const GitStats *stats);

/**
 * Print hotspots in JSON format
 * @param stats GitStats structure containing hotspot data
 */
void print_hotspots_json(const GitStats *stats);

/**
 * Print author activity analysis in human-readable format
 * @param stats GitStats structure containing activity data
 */
void print_activity_human(const GitStats *stats);

/**
 * Print activity analysis in JSON format
 * @param stats GitStats structure containing activity data
 */
void print_activity_json(const GitStats *stats);

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