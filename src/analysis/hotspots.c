#define _GNU_SOURCE
#include "hotspots.h"
#include "../utils/string_utils.h"
#include "../utils/git_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/* Forward declarations */
static double calculate_hotspot_score(int commits, int lines_added, int lines_deleted);
static int compare_hotspots_by_score(const void* a, const void* b);

/**
 * Get file hotspot statistics
 */
int get_hotspot_stats(GitStats *stats) {
    assert(stats != NULL);

    FILE *fp = popen("git log --name-only --pretty=format: 2>/dev/null", "r");
    if (fp == NULL) {
        return -1;
    }

    char filename[MAX_LINE_LENGTH];
    stats->hotspot_count = 0;

    /* Count commits per file */
    while (fgets(filename, sizeof(filename), fp) != NULL) {
        remove_trailing_newline(filename);

        /* Skip empty lines */
        if (strlen(filename) == 0) continue;

        /* Skip if filename is too long */
        if (strlen(filename) >= MAX_PATH_LENGTH) continue;

        /* Find or create hotspot entry */
        int found = 0;
        for (int i = 0; i < stats->hotspot_count; i++) {
            if (strcmp(stats->hotspots[i].filename, filename) == 0) {
                stats->hotspots[i].commit_count++;
                found = 1;
                break;
            }
        }

        if (!found && stats->hotspot_count < MAX_FILES) {
            safe_string_copy(stats->hotspots[stats->hotspot_count].filename,
                           filename, sizeof(stats->hotspots[stats->hotspot_count].filename));
            stats->hotspots[stats->hotspot_count].commit_count = 1;
            stats->hotspots[stats->hotspot_count].lines_added = 0;
            stats->hotspots[stats->hotspot_count].lines_deleted = 0;
            stats->hotspot_count++;
        }
    }
    pclose(fp);

    /* Get line change statistics for each file */
    for (int i = 0; i < stats->hotspot_count; i++) {
        char command[MAX_COMMAND_LENGTH];
        int ret = snprintf(command, sizeof(command),
                "git log --numstat --pretty=format: -- \"%s\" 2>/dev/null | "
                "awk '{add+=$1; del+=$2} END {print add\" \"del}'",
                stats->hotspots[i].filename);

        if (ret > 0 && ret < (int)sizeof(command)) {
            char *result = execute_git_command(command);
            if (result != NULL) {
                sscanf(result, "%d %d", &stats->hotspots[i].lines_added,
                       &stats->hotspots[i].lines_deleted);
                free(result);
            }
        }

        /* Calculate hotspot score */
        stats->hotspots[i].hotspot_score = calculate_hotspot_score(
            stats->hotspots[i].commit_count,
            stats->hotspots[i].lines_added,
            stats->hotspots[i].lines_deleted
        );
    }

    /* Sort hotspots by score */
    qsort(stats->hotspots, stats->hotspot_count, sizeof(FileHotspot),
          compare_hotspots_by_score);

    return 0;
}

/**
 * Calculate hotspot score based on commits and line changes
 */
static double calculate_hotspot_score(int commits, int lines_added, int lines_deleted) {
    if (commits <= 0) return 0.0;

    int total_lines = lines_added + lines_deleted;

    /* Score = commits × √(total_lines + 1) */
    /* The +1 prevents sqrt(0) and gives small weight to files with commits but no line data */
    return (double)commits * sqrt((double)(total_lines + 1));
}

/**
 * Comparison function for sorting hotspots by score
 */
static int compare_hotspots_by_score(const void* a, const void* b) {
    const FileHotspot* hotspot_a = (const FileHotspot*)a;
    const FileHotspot* hotspot_b = (const FileHotspot*)b;

    /* Sort in descending order by hotspot score */
    if (hotspot_a->hotspot_score < hotspot_b->hotspot_score) return 1;
    if (hotspot_a->hotspot_score > hotspot_b->hotspot_score) return -1;
    return 0;
}