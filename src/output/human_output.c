#include "formatters.h"
#include "../git_stats.h"
#include "../analysis/hotspots.h"
#include "../analysis/activity.h"
#include "../version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * Print comprehensive statistics in human-readable format
 */
void print_stats_human(const GitStats *stats, AnalysisMode mode) {
    assert(stats != NULL);
    
    printf("Repository Statistics for: %s\n", stats->repo_name);
    printf("==========================================\n\n");
    
    printf("📊 General Information:\n");
    printf("  Current Branch: %s\n", stats->current_branch);
    printf("  Total Commits: %d\n", stats->total_commits);
    printf("  Total Authors: %d\n", stats->total_authors);
    printf("  Total Branches: %d\n", stats->total_branches);
    printf("  Total Files: %d\n", stats->total_files);
    printf("  Total Lines of Code: %ld\n\n", stats->total_lines);
    
    /* Print top contributors */
    printf("👥 Top Contributors:\n");
    int authors_to_show = (stats->total_authors < MAX_AUTHORS_DISPLAY) ? 
                         stats->total_authors : MAX_AUTHORS_DISPLAY;
    
    for (int i = 0; i < authors_to_show; i++) {
        printf("  %2d. %-30s %4d commits", i + 1, 
               stats->authors[i].name, stats->authors[i].commit_count);
        if (stats->authors[i].lines_added > 0 || stats->authors[i].lines_deleted > 0) {
            printf(" (+%d/-%d lines)", stats->authors[i].lines_added, 
                   stats->authors[i].lines_deleted);
        }
        printf("\n");
    }
    printf("\n");
    
    /* Print branches */
    printf("🌿 Branches:\n");
    int branches_to_show = (stats->total_branches < MAX_BRANCHES_DISPLAY) ? 
                          stats->total_branches : MAX_BRANCHES_DISPLAY;
    
    for (int i = 0; i < branches_to_show; i++) {
        printf("  %-20s %4d commits\n", stats->branches[i].name, 
               stats->branches[i].commit_count);
    }
    if (stats->total_branches > MAX_BRANCHES_DISPLAY) {
        printf("  ... and %d more branches\n", 
               stats->total_branches - MAX_BRANCHES_DISPLAY);
    }
    printf("\n");
    
    /* Print file types */
    printf("📁 File Types:\n");
    if (stats->file_type_count > 0) {
        /* Create a copy for sorting */
        FileType temp_types[MAX_FILE_TYPES];
        memcpy(temp_types, stats->file_types, 
               sizeof(FileType) * stats->file_type_count);
        
        /* Sort by count */
        qsort(temp_types, stats->file_type_count, sizeof(FileType), 
              compare_file_types_by_count);
        
        int types_to_show = (stats->file_type_count < MAX_FILE_TYPES_DISPLAY) ? 
                           stats->file_type_count : MAX_FILE_TYPES_DISPLAY;
        
        for (int i = 0; i < types_to_show; i++) {
            double percentage = (stats->total_lines > 0) ? 
                               (double)temp_types[i].total_lines * 100.0 / stats->total_lines : 0.0;
            printf("  %-10s %4d files, %8ld lines (%5.1f%%)\n", 
                   temp_types[i].extension, temp_types[i].count, 
                   temp_types[i].total_lines, percentage);
        }
        
        if (stats->file_type_count > MAX_FILE_TYPES_DISPLAY) {
            printf("  ... and %d more file types\n", 
                   stats->file_type_count - MAX_FILE_TYPES_DISPLAY);
        }
    }
    printf("\n");
    
    /* Print additional analysis based on mode */
    if (mode == ANALYSIS_HOTSPOTS) {
        print_hotspots_human(stats);
    } else if (mode == ANALYSIS_ACTIVITY) {
        print_activity_human(stats);
    }
}

/**
 * Print hotspot analysis in human-readable format
 */
void print_hotspots_human(const GitStats *stats) {
    assert(stats != NULL);
    
    printf("🔥 Hotspot Analysis (Files with High Churn):\n");
    
    if (stats->hotspot_count == 0) {
        printf("  No hotspots found.\n\n");
        return;
    }
    
    int hotspots_to_show = (stats->hotspot_count < 15) ? stats->hotspot_count : 15;
    
    for (int i = 0; i < hotspots_to_show; i++) {
        printf("  %2d. %-40s %3d commits, +%d/-%d lines (score: %.1f)\n", 
               i + 1,
               stats->hotspots[i].filename,
               stats->hotspots[i].commit_count,
               stats->hotspots[i].lines_added,
               stats->hotspots[i].lines_deleted,
               stats->hotspots[i].hotspot_score);
    }
    
    if (stats->hotspot_count > 15) {
        printf("  ... and %d more files\n", stats->hotspot_count - 15);
    }
    
    printf("\n");
    printf("  📊 Hotspot Score = commits × √(lines_added + lines_deleted + 1)\n");
    printf("  💡 High scores indicate files that change frequently with significant modifications\n");
    printf("\n");
}

/**
 * Print author activity analysis in human-readable format
 */
void print_activity_human(const GitStats *stats) {
    assert(stats != NULL);
    
    printf("📈 Author Activity Analysis:\n");
    
    if (stats->activity_count == 0) {
        printf("  No activity data found.\n\n");
        return;
    }
    
    /* Count active vs inactive contributors */
    int active_count = 0;
    int single_commit_count = 0;
    
    for (int i = 0; i < stats->activity_count; i++) {
        if (stats->activities[i].is_active) active_count++;
        if (stats->activities[i].commit_count == 1) single_commit_count++;
    }
    
    printf("  📊 Summary: %d total contributors, %d active (< 90 days), %d single-commit\n\n",
           stats->activity_count, active_count, single_commit_count);
    
    /* Show top contributors by activity score */
    printf("  🏆 Top Contributors by Activity:\n");
    int contributors_to_show = (stats->activity_count < 10) ? stats->activity_count : 10;
    
    for (int i = 0; i < contributors_to_show; i++) {
        const char* status = stats->activities[i].is_active ? "ACTIVE" : "INACTIVE";
        printf("  %2d. %-25s %3d commits, last: %s (%d days ago) [%s]\n",
               i + 1,
               stats->activities[i].name,
               stats->activities[i].commit_count,
               stats->activities[i].last_commit_date,
               stats->activities[i].days_since_last_commit,
               status);
    }
    
    printf("\n  📅 Activity Details:\n");
    for (int i = 0; i < contributors_to_show; i++) {
        printf("      %s: %s → %s (%d commits, +%d/-%d lines, score: %.1f)\n",
               stats->activities[i].name,
               stats->activities[i].first_commit_date,
               stats->activities[i].last_commit_date,
               stats->activities[i].commit_count,
               stats->activities[i].lines_added,
               stats->activities[i].lines_deleted,
               stats->activities[i].activity_score);
    }
    
    printf("\n  💡 Activity Score = commits × (10000 / (days_since_last + 1)) × log(lines + 1)\n");
    printf("  ✨ Higher scores indicate recent, frequent, and substantial contributors\n");
    printf("\n");
}

/**
 * Print help information
 */
void print_help(void) {
    printf("%s v%s\n", PROGRAM_NAME, VERSION_STRING);
    printf("============================\n\n");
    printf("Usage: git-stat [OPTIONS]\n\n");
    printf("%s\n\n", PROGRAM_DESCRIPTION);
    printf("Options:\n");
    printf("  -h, --help          Show this help message\n");
    printf("  -v, --version       Show version information\n");
    printf("  --output FORMAT     Output format (default: human-readable)\n");
    printf("                      Supported formats: json\n");
    printf("  --hotspots          Analyze and display file hotspots (high churn)\n");
    printf("  --activity          Analyze author activity over time\n\n");
    printf("Features:\n");
    printf("  • Repository overview (commits, authors, branches, files)\n");
    printf("  • Top contributors with commit counts and line changes\n");
    printf("  • Branch information and commit counts\n");
    printf("  • File type analysis with line counts and percentages\n");
    printf("  • Hotspot detection for identifying high-churn files\n");
    printf("  • Author activity analysis over time\n");
    printf("  • Works completely offline with local git data\n\n");
    printf("Examples:\n");
    printf("  git-stat                    # Analyze current repository\n");
    printf("  git-stat --hotspots         # Include hotspot analysis\n");
    printf("  git-stat --activity         # Include author activity analysis\n");
    printf("  git-stat --output json      # Output in JSON format\n");
    printf("  git-stat --hotspots --output json  # Hotspots in JSON format\n");
    printf("  git-stat --activity --output json  # Activity analysis in JSON format\n");
    printf("  git-stat --help             # Show this help\n");
    printf("  git-stat --version          # Show version info\n\n");
    printf("Exit Codes:\n");
    printf("  0  Success\n");
    printf("  1  General error\n");
    printf("  2  Not a git repository\n\n");
    printf("Note: Must be run from within a git repository.\n\n");
    printf("%s\n", PROGRAM_COPYRIGHT);
    printf("License: %s\n", PROGRAM_LICENSE);
}