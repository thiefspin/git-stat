#include "git_stats.h"
#include "analysis/hotspots.h"
#include "analysis/activity.h"
#include "output/formatters.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Parse command line arguments
 */
static int parse_arguments(int argc, char *argv[], OutputFormat *format, AnalysisMode *mode) {
    assert(format != NULL);
    assert(mode != NULL);
    
    *format = OUTPUT_DEFAULT;
    *mode = ANALYSIS_BASIC;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return EXIT_HELP_SHOWN;
        }
        
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("%s\n", get_version_string());
            return EXIT_VERSION_SHOWN;
        }
        
        if (strcmp(argv[i], "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: --output requires a format argument\n");
                fprintf(stderr, "Supported formats: json\n");
                return EXIT_ERROR_CODE;
            }
            
            i++; /* Move to format argument */
            if (strcmp(argv[i], "json") == 0) {
                *format = OUTPUT_JSON;
            } else {
                fprintf(stderr, "Error: Unknown output format '%s'\n", argv[i]);
                fprintf(stderr, "Supported formats: json\n");
                return EXIT_ERROR_CODE;
            }
        } else if (strcmp(argv[i], "--hotspots") == 0) {
            *mode = ANALYSIS_HOTSPOTS;
        } else if (strcmp(argv[i], "--activity") == 0) {
            *mode = ANALYSIS_ACTIVITY;
        } else {
            /* Unknown argument */
            fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
            fprintf(stderr, "Use --help for usage information.\n");
            return EXIT_ERROR_CODE;
        }
    }
    
    return 0;
}

/**
 * Main entry point
 */
int main(int argc, char *argv[]) {
    OutputFormat output_format = OUTPUT_DEFAULT;
    AnalysisMode analysis_mode = ANALYSIS_BASIC;
    
    /* Parse command line arguments */
    int parse_result = parse_arguments(argc, argv, &output_format, &analysis_mode);
    if (parse_result == EXIT_HELP_SHOWN || parse_result == EXIT_VERSION_SHOWN) {
        return EXIT_SUCCESS_CODE;
    }
    if (parse_result != 0) {
        return parse_result;
    }
    
    /* Print header for default output only */
    if (output_format == OUTPUT_DEFAULT) {
        printf("%s v%s\n", PROGRAM_NAME, VERSION_STRING);
        printf("============================\n\n");
    }

    /* Verify we're in a git repository */
    if (!is_git_repository()) {
        fprintf(stderr, "Error: Not a git repository (or any of the parent directories)\n");
        fprintf(stderr, "Run this command from within a git repository.\n");
        return EXIT_NOT_GIT_REPO;
    }

    /* Initialize and gather basic statistics */
    GitStats stats;
    init_git_stats(&stats);
    
    if (get_basic_git_stats(&stats) != 0) {
        fprintf(stderr, "Error: Failed to gather basic git statistics\n");
        return EXIT_ERROR_CODE;
    }

    /* Gather additional analysis data based on mode */
    switch (analysis_mode) {
        case ANALYSIS_HOTSPOTS:
            if (get_hotspot_stats(&stats) != 0) {
                fprintf(stderr, "Warning: Failed to get hotspot statistics\n");
            }
            break;
            
        case ANALYSIS_ACTIVITY:
            if (get_activity_stats(&stats) != 0) {
                fprintf(stderr, "Warning: Failed to get activity statistics\n");
            }
            break;
            
        case ANALYSIS_BASIC:
        default:
            /* No additional analysis needed */
            break;
    }

    /* Output results in requested format */
    if (output_format == OUTPUT_JSON) {
        print_stats_json(&stats, analysis_mode);
    } else {
        print_stats_human(&stats, analysis_mode);
    }
    
    return EXIT_SUCCESS_CODE;
}