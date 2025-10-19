# Git Statistics Utility

A lightweight, fast C utility that provides comprehensive statistics for git repositories without requiring network connectivity.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)

## Features

- 📊 **Repository Overview**: Total commits, authors, branches, and files
- 👥 **Contributor Analysis**: Top contributors with commit counts and line changes
- 🌿 **Branch Information**: Local branches with commit statistics
- 📁 **File Type Analysis**: Breakdown by file extensions with line counts and percentages
- 🔥 **Hotspot Detection**: Identify files with high churn (frequent changes + significant modifications)
- 📈 **Author Activity Analysis**: Track contributor activity over time, identify active vs inactive contributors
- ⚡ **Fast & Lightweight**: Pure C implementation with minimal dependencies
- 🔒 **Offline Operation**: Works entirely with local git data, no network required
- 🎯 **Cross-Platform**: Supports Linux, macOS, and Windows
- 📤 **Multiple Output Formats**: Human-readable and JSON output for automation

## Installation

### Prerequisites

- A C compiler (GCC, Clang, or MSVC)
- Git installed and accessible from command line
- Make (optional, for easy building)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/git-stat.git
cd git-stat

# Build using Make
make

# Or build manually
clang -Wall -Wextra -O2 -std=c17 -o git-stat main.c
```

### Installation

```bash
# Copy to local bin directory
cp git-stat ~/.local/bin/

# Or copy to system-wide location (requires sudo)
sudo cp git-stat /usr/local/bin/
```

## Usage

Run `git-stat` from within any git repository:

```bash
cd /path/to/your/git/repository
git-stat
```

### Command Line Options

```bash
git-stat                         # Analyze current repository (human-readable output)
git-stat --hotspots              # Include hotspot analysis (high-churn files)
git-stat --activity              # Include author activity analysis over time
git-stat --output json           # Output in JSON format
git-stat --hotspots --output json # Hotspots analysis in JSON format
git-stat --activity --output json # Activity analysis in JSON format
git-stat --help                  # Show help information
git-stat -h                      # Show help information
```

### Example Output

#### Human-Readable Format (Default)

```
Git Statistics Utility v1.0
============================

Analyzing repository...
Analysis complete!

Repository Statistics for: my-awesome-project
==========================================

📊 General Information:
  Current Branch: main
  Total Commits: 247
  Total Authors: 12
  Total Branches: 8
  Total Files: 156
  Total Lines of Code: 15,432

👥 Top Contributors:
   1. John Doe                    98 commits (+12,543/-3,221 lines)
   2. Jane Smith                  67 commits (+8,765/-2,109 lines)
   3. Bob Johnson                 45 commits (+5,432/-1,876 lines)
   4. Alice Williams              23 commits (+2,109/-543 lines)
   5. Charlie Brown               14 commits (+1,234/-321 lines)

🌿 Branches:
  main                    247 commits
  feature/new-ui           23 commits
  bugfix/memory-leak       12 commits
  development              189 commits
  hotfix/security           5 commits

📁 File Types:
  c            45 files,     8,234 lines ( 67.2%)
  h            23 files,     2,109 lines ( 17.2%)
  md            8 files,       876 lines (  7.1%)
  txt           12 files       543 lines (  4.4%)
  json          5 files        234 lines (  1.9%)

🔥 Hotspot Analysis (Files with High Churn):
   1. src/core/engine.c                         23 commits, +2,341/-1,123 lines (score: 543.2)
   2. src/api/handlers.c                        18 commits, +1,867/-943 lines (score: 301.4)
   3. config/settings.json                      15 commits, +234/-187 lines (score: 95.8)
   4. README.md                                 12 commits, +445/-67 lines (score: 86.2)
   5. src/utils/helpers.c                       9 commits, +678/-234 lines (score: 85.7)

  📊 Hotspot Score = commits × √(lines_added + lines_deleted + 1)
  💡 High scores indicate files that change frequently with significant modifications

📈 Author Activity Analysis:
  📊 Summary: 12 total contributors, 5 active (< 90 days), 3 single-commit

  🏆 Top Contributors by Activity:
   1. John Doe                    98 commits, last: 2024-01-15 (5 days ago) [ACTIVE]
   2. Jane Smith                  67 commits, last: 2024-01-10 (10 days ago) [ACTIVE]
   3. Bob Johnson                 45 commits, last: 2023-11-20 (65 days ago) [ACTIVE]
   4. Alice Williams              23 commits, last: 2023-08-15 (160 days ago) [INACTIVE]
   5. Charlie Brown               14 commits, last: 2023-06-01 (235 days ago) [INACTIVE]

  📅 Activity Details:
      John Doe: 2023-03-15 → 2024-01-15 (98 commits, +12,543/-3,221 lines, score: 85,432.1)
      Jane Smith: 2023-05-20 → 2024-01-10 (67 commits, +8,765/-2,109 lines, score: 67,890.4)

  💡 Activity Score = commits × (10000 / (days_since_last + 1)) × log(lines + 1)
  ✨ Higher scores indicate recent, frequent, and substantial contributors
```

#### JSON Format

When using `--output json`, the utility outputs structured JSON data suitable for automation and integration with other tools:

```json
{
  "repository": {
    "name": "my-awesome-project",
    "current_branch": "main"
  },
  "summary": {
    "total_commits": 247,
    "total_authors": 12,
    "total_branches": 8,
    "total_files": 156,
    "total_lines": 15432
  },
  "authors": [
    {
      "name": "John Doe",
      "commits": 98,
      "lines_added": 12543,
      "lines_deleted": 3221
    },
    {
      "name": "Jane Smith", 
      "commits": 67,
      "lines_added": 8765,
      "lines_deleted": 2109
    }
  ],
  "branches": [
    {
      "name": "main",
      "commits": 247
    },
    {
      "name": "feature/new-ui",
      "commits": 23
    }
  ],
  "file_types": [
    {
      "extension": "c",
      "files": 45,
      "lines": 8234,
      "percentage": 67.2
    },
    {
      "extension": "h", 
      "files": 23,
      "lines": 2109,
      "percentage": 17.2
    }
  ],
  "hotspots": [
    {
      "filename": "src/core/engine.c",
      "commits": 23,
      "lines_added": 2341,
      "lines_deleted": 1123,
      "hotspot_score": 543.2
    },
    {
      "filename": "src/api/handlers.c",
      "commits": 18,
      "lines_added": 1867,
      "lines_deleted": 943,
      "hotspot_score": 301.4
    }
  ],
  "activity_summary": {
    "total_contributors": 12,
    "active_contributors": 5,
    "single_commit_contributors": 3
  },
  "author_activity": [
    {
      "name": "John Doe",
      "commits": 98,
      "lines_added": 12543,
      "lines_deleted": 3221,
      "first_commit_date": "2023-03-15",
      "last_commit_date": "2024-01-15",
      "days_since_last_commit": 5,
      "is_active": true,
      "activity_score": 85432.1
    },
    {
      "name": "Jane Smith",
      "commits": 67,
      "lines_added": 8765,
      "lines_deleted": 2109,
      "first_commit_date": "2023-05-20",
      "last_commit_date": "2024-01-10",
      "days_since_last_commit": 10,
      "is_active": true,
      "activity_score": 67890.4
    }
  ]
}
```

## Development

### Project Structure

```
git-stat/
├── src/                  # Source code directory
│   ├── main.c           # Entry point and main program logic (127 lines)
│   ├── git_stats.h      # Core data structures and API definitions
│   ├── git_stats.c      # Basic statistics gathering (300 lines)
│   ├── version.h        # Version management system
│   ├── analysis/        # Analysis modules
│   │   ├── hotspots.h   # Hotspot analysis interface
│   │   ├── hotspots.c   # Hotspot detection implementation (111 lines)
│   │   ├── activity.h   # Activity analysis interface
│   │   └── activity.c   # Activity analysis implementation (141 lines)
│   ├── output/          # Output formatting modules
│   │   ├── formatters.h # Output formatting interfaces
│   │   ├── human_output.c # Human-readable output (227 lines)
│   │   └── json_output.c  # JSON output formatting (141 lines)
│   └── utils/           # Utility functions
│       ├── string_utils.h/.c  # String manipulation utilities
│       └── git_commands.h/.c  # Git command execution utilities
├── Makefile             # Enhanced build system for modular compilation
├── install.sh           # Automated installation script
├── CODING_STANDARDS.md  # C best practices documentation
├── README.md           # This file
├── LICENSE             # MIT license
└── .gitignore          # Git ignore rules
```

### Contributing

The modular architecture makes contributing much easier:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Choose the appropriate module for your changes:
   - **Core functionality**: `src/git_stats.c`
   - **New analysis types**: Add to `src/analysis/`
   - **Output formats**: Modify `src/output/`
   - **Utility functions**: Add to `src/utils/`
4. Commit your changes (`git commit -m 'Add amazing feature'`)
5. Push to the branch (`git push origin feature/amazing-feature`)
6. Open a Pull Request

#### Module Guidelines
- **Single Responsibility**: Each module should have one clear purpose
- **Clean Interfaces**: Use header files to define clear APIs
- **Error Handling**: Follow existing patterns for error reporting
- **Documentation**: Document all public functions in headers

### Coding Standards

- Follow C17 standard
- Use consistent indentation (4 spaces)
- Include comments for complex logic
- Test on multiple platforms
- **Keep functions focused and small** (largest file now ~300 lines vs 1300+)
- **Modular design**: Separate concerns into appropriate modules
- **Clear interfaces**: Use header files to define module boundaries
- **Consistent error handling**: Follow established patterns across modules

## Technical Details

### Dependencies

- Standard C library
- Math library (libm) for hotspot score calculations
- POSIX system calls (for file operations)
- Git command-line interface

### Performance

- Optimized for repositories with up to 100,000 commits
- Memory usage typically under 10MB
- Analysis time scales linearly with repository size
- No external dependencies beyond git and libc

### Limitations

- Requires git command-line tools to be installed
- Only analyzes local repository data
- Large repositories (>1M commits) may take longer to analyze
- Binary files are counted but not analyzed for content

## Compatibility

| Platform | Status | Notes |
|----------|--------|--------|
| Linux    | ✅ Tested | All distributions with git |
| macOS    | ✅ Tested | Intel and Apple Silicon |
| Windows  | ⚠️ Beta | WSL recommended |

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Changelog

### v1.0.0 (2024-01-XX)
- Initial release
- Basic repository statistics
- Author and branch analysis
- File type breakdown
- Cross-platform support

## Roadmap

- [x] JSON export options
- [x] Hotspot detection for high-churn files
- [x] Author activity analysis over time
- [ ] CSV export options
- [ ] Historical trend analysis (time-based charts)
- [ ] Git hook integration
- [ ] Configuration file support
- [ ] Custom report templates
- [ ] Language-specific statistics
- [ ] Hotspot and activity thresholds and filtering options
- [ ] Team collaboration metrics

## Support

If you encounter issues or have suggestions:

1. Check existing [Issues](https://github.com/yourusername/git-stat/issues)
2. Create a new issue with detailed information
3. Include your OS, git version, and repository size

---

Made with ❤️ for the git community