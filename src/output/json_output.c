#include "formatters.h"
#include "../git_stats.h"
#include <string.h>
#include <assert.h>

/* Forward declarations */
static void print_hotspots_json(const GitStats *stats);
static void print_activity_json(const GitStats *stats);

/**
 * Print statistics in JSON format
 */
void print_stats_json(const GitStats *stats, AnalysisMode mode) {
    assert(stats != NULL);

    printf("{\n");
    printf("  \"repository\": {\n");
    printf("    \"name\": \"%s\",\n", stats->repo_name);
    printf("    \"current_branch\": \"%s\"\n", stats->current_branch);
    printf("  },\n");

    printf("  \"summary\": {\n");
    printf("    \"total_commits\": %d,\n", stats->total_commits);
    printf("    \"total_authors\": %d,\n", stats->total_authors);
    printf("    \"total_branches\": %d,\n", stats->total_branches);
    printf("    \"total_files\": %d,\n", stats->total_files);
    printf("    \"total_lines\": %ld\n", stats->total_lines);
    printf("  },\n");

    /* Authors array */
    printf("  \"authors\": [\n");
    int authors_to_show = (stats->total_authors < MAX_AUTHORS_DISPLAY) ?
                         stats->total_authors : MAX_AUTHORS_DISPLAY;
    for (int i = 0; i < authors_to_show; i++) {
        printf("    {\n");
        printf("      \"name\": \"%s\",\n", stats->authors[i].name);
        printf("      \"commits\": %d,\n", stats->authors[i].commit_count);
        printf("      \"lines_added\": %d,\n", stats->authors[i].lines_added);
        printf("      \"lines_deleted\": %d\n", stats->authors[i].lines_deleted);
        printf("    }%s\n", (i < authors_to_show - 1) ? "," : "");
    }
    printf("  ],\n");

    /* File types array */
    printf("  \"file_types\": [\n");
    if (stats->file_type_count > 0) {
        FileType temp_types[MAX_FILE_TYPES];
        memcpy(temp_types, stats->file_types, sizeof(FileType) * stats->file_type_count); // NOLINT(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        qsort(temp_types, stats->file_type_count, sizeof(FileType), compare_file_types_by_count);

        int types_to_show = (stats->file_type_count < MAX_FILE_TYPES_DISPLAY) ?
                           stats->file_type_count : MAX_FILE_TYPES_DISPLAY;

        for (int i = 0; i < types_to_show; i++) {
            double percentage = (stats->total_lines > 0) ?
                               (double)temp_types[i].total_lines * 100.0 / stats->total_lines : 0.0;
            printf("    {\n");
            printf("      \"extension\": \"%s\",\n", temp_types[i].extension);
            printf("      \"files\": %d,\n", temp_types[i].count);
            printf("      \"lines\": %ld,\n", temp_types[i].total_lines);
            printf("      \"percentage\": %.1f\n", percentage);
            printf("    }%s\n", (i < types_to_show - 1) ? "," : "");
        }
    }
    printf("  ]");

    /* Add analysis-specific sections */
    if (mode == ANALYSIS_HOTSPOTS) {
        printf(",\n");
        print_hotspots_json(stats);
    } else if (mode == ANALYSIS_ACTIVITY) {
        printf(",\n");
        print_activity_json(stats);
    } else {
        printf("\n");
    }

    printf("}\n");
}

/**
 * Print hotspots in JSON format
 */
static void print_hotspots_json(const GitStats *stats) {
    assert(stats != NULL);

    printf("  \"hotspots\": [\n");
    if (stats->hotspot_count > 0) {
        int hotspots_to_show = (stats->hotspot_count < 15) ? stats->hotspot_count : 15;

        for (int i = 0; i < hotspots_to_show; i++) {
            printf("    {\n");
            printf("      \"filename\": \"%s\",\n", stats->hotspots[i].filename);
            printf("      \"commits\": %d,\n", stats->hotspots[i].commit_count);
            printf("      \"lines_added\": %d,\n", stats->hotspots[i].lines_added);
            printf("      \"lines_deleted\": %d,\n", stats->hotspots[i].lines_deleted);
            printf("      \"hotspot_score\": %.1f\n", stats->hotspots[i].hotspot_score);
            printf("    }%s\n", (i < hotspots_to_show - 1) ? "," : "");
        }
    }
    printf("  ]");
}

/**
 * Print activity analysis in JSON format
 */
static void print_activity_json(const GitStats *stats) {
    assert(stats != NULL);

    /* Calculate summary statistics */
    int active_count = 0;
    int single_commit_count = 0;

    for (int i = 0; i < stats->activity_count; i++) {
        if (stats->activities[i].is_active) active_count++;
        if (stats->activities[i].commit_count == 1) single_commit_count++;
    }

    printf("  \"activity_summary\": {\n");
    printf("    \"total_contributors\": %d,\n", stats->activity_count);
    printf("    \"active_contributors\": %d,\n", active_count);
    printf("    \"single_commit_contributors\": %d\n", single_commit_count);
    printf("  },\n");

    printf("  \"author_activity\": [\n");
    if (stats->activity_count > 0) {
        int contributors_to_show = (stats->activity_count < 15) ? stats->activity_count : 15;

        for (int i = 0; i < contributors_to_show; i++) {
            printf("    {\n");
            printf("      \"name\": \"%s\",\n", stats->activities[i].name);
            printf("      \"commits\": %d,\n", stats->activities[i].commit_count);
            printf("      \"lines_added\": %d,\n", stats->activities[i].lines_added);
            printf("      \"lines_deleted\": %d,\n", stats->activities[i].lines_deleted);
            printf("      \"first_commit_date\": \"%s\",\n", stats->activities[i].first_commit_date);
            printf("      \"last_commit_date\": \"%s\",\n", stats->activities[i].last_commit_date);
            printf("      \"days_since_last_commit\": %d,\n", stats->activities[i].days_since_last_commit);
            printf("      \"is_active\": %s,\n", stats->activities[i].is_active ? "true" : "false");
            printf("      \"activity_score\": %.1f\n", stats->activities[i].activity_score);
            printf("    }%s\n", (i < contributors_to_show - 1) ? "," : "");
        }
    }
    printf("  ]");
}
