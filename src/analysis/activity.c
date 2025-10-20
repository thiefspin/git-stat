#define _GNU_SOURCE
#include "activity.h"
#include "../utils/string_utils.h"
#include "../utils/git_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/* Forward declarations */
static double calculate_activity_score(int commits, int days_since_last, int lines_changed);
static int compare_activities_by_score(const void* a, const void* b);

/**
 * Get author activity statistics over time
 */
int get_activity_stats(GitStats *stats) {
    assert(stats != NULL);

    FILE *fp = popen("git log --pretty=format:'%an|%ad|%s' --date=short --all 2>/dev/null", "r");
    if (fp == NULL) {
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    stats->activity_count = 0;

    /* Parse commit log for author activity */
    while (fgets(line, sizeof(line), fp) != NULL) {
        remove_trailing_newline(line);

        /* Parse format: author|date|subject */
        const char *author = strtok(line, "|");
        const char *date = strtok(NULL, "|");

        if (author == NULL || date == NULL) continue;

        /* Find or create activity entry */
        int found = 0;
        for (int i = 0; i < stats->activity_count; i++) {
            if (strcmp(stats->activities[i].name, author) == 0) {
                stats->activities[i].commit_count++;

                /* Update first commit date (earliest) */
                if (strlen(stats->activities[i].first_commit_date) == 0 ||
                    strcmp(date, stats->activities[i].first_commit_date) < 0) {
                    safe_string_copy(stats->activities[i].first_commit_date, date,
                                   sizeof(stats->activities[i].first_commit_date));
                }

                /* Update last commit date (latest) */
                if (strlen(stats->activities[i].last_commit_date) == 0 ||
                    strcmp(date, stats->activities[i].last_commit_date) > 0) {
                    safe_string_copy(stats->activities[i].last_commit_date, date,
                                   sizeof(stats->activities[i].last_commit_date));
                }

                found = 1;
                break;
            }
        }

        if (!found && stats->activity_count < MAX_AUTHORS) {
            safe_string_copy(stats->activities[stats->activity_count].name, author,
                           sizeof(stats->activities[stats->activity_count].name));
            stats->activities[stats->activity_count].commit_count = 1;
            safe_string_copy(stats->activities[stats->activity_count].first_commit_date, date,
                           sizeof(stats->activities[stats->activity_count].first_commit_date));
            safe_string_copy(stats->activities[stats->activity_count].last_commit_date, date,
                           sizeof(stats->activities[stats->activity_count].last_commit_date));
            stats->activities[stats->activity_count].lines_added = 0;
            stats->activities[stats->activity_count].lines_deleted = 0;
            stats->activity_count++;
        }
    }
    pclose(fp);

    /* Get line change statistics and calculate activity metrics */
    for (int i = 0; i < stats->activity_count; i++) {
        /* Get line statistics for this author */
        char command[MAX_COMMAND_LENGTH];
        int ret = snprintf(command, sizeof(command),
                "git log --author=\"%s\" --pretty=tformat: --numstat 2>/dev/null | "
                "awk '{add+=$1; del+=$2} END {print add\" \"del}'",
                stats->activities[i].name);

        if (ret > 0 && ret < (int)sizeof(command)) {
            char *result = execute_git_command(command);
            if (result != NULL) {
                sscanf(result, "%d %d", &stats->activities[i].lines_added,
                       &stats->activities[i].lines_deleted);
                free(result);
            }
        }

        /* Calculate days since last commit */
        stats->activities[i].days_since_last_commit =
            calculate_days_since_commit(stats->activities[i].last_commit_date);

        /* Determine if author is active (committed within last 90 days) */
        stats->activities[i].is_active = (stats->activities[i].days_since_last_commit <= 90) ? 1 : 0;

        /* Calculate activity score */
        stats->activities[i].activity_score = calculate_activity_score(
            stats->activities[i].commit_count,
            stats->activities[i].days_since_last_commit,
            stats->activities[i].lines_added + stats->activities[i].lines_deleted
        );
    }

    /* Sort activities by score */
    qsort(stats->activities, stats->activity_count, sizeof(AuthorActivity),
          compare_activities_by_score);

    return 0;
}

/**
 * Calculate activity score based on commits, recency, and line changes
 */
static double calculate_activity_score(int commits, int days_since_last, int lines_changed) {
    if (commits <= 0) return 0.0;

    /* Recency factor: more recent activity gets higher weight */
    double recency_factor = 10000.0 / (double)(days_since_last + 1);

    /* Line change factor: log scale to prevent huge commits from dominating */
    double lines_factor = log((double)(lines_changed + 1));

    /* Score = commits * recency_factor * lines_factor */
    return (double)commits * recency_factor * lines_factor;
}

/**
 * Comparison function for sorting activities by score
 */
static int compare_activities_by_score(const void* a, const void* b) {
    const AuthorActivity* activity_a = (const AuthorActivity*)a;
    const AuthorActivity* activity_b = (const AuthorActivity*)b;

    /* Sort in descending order by activity score */
    if (activity_a->activity_score < activity_b->activity_score) return 1;
    if (activity_a->activity_score > activity_b->activity_score) return -1;
    return 0;
}
