# Git Statistics Utility

A lightweight, fast C utility that provides comprehensive statistics for git repositories without requiring network connectivity.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)

## Features

- 📊 **Repository Overview**: Total commits, authors, branches, and files
- 👥 **Contributor Analysis**: Top contributors with commit counts and line changes
- 🌿 **Branch Information**: Local branches with commit statistics
- 📁 **File Type Analysis**: Breakdown by file extensions with line counts
- ⚡ **Fast & Lightweight**: Pure C implementation with minimal dependencies
- 🔒 **Offline Operation**: Works entirely with local git data, no network required
- 🎯 **Cross-Platform**: Supports Linux, macOS, and Windows

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
git-stat           # Analyze current repository
git-stat --help    # Show help information
git-stat -h        # Show help information
```

### Example Output

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
  c            45 files,     8,234 lines
  h            23 files,     2,109 lines
  md            8 files,       876 lines
  txt           12 files       543 lines
  json          5 files        234 lines
```

## Development

### Project Structure

```
git-stat/
├── main.c          # Main source code
├── Makefile        # Build configuration
├── README.md       # This file
└── LICENSE         # MIT license
```

### Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Coding Standards

- Follow C17 standard
- Use consistent indentation (4 spaces)
- Include comments for complex logic
- Test on multiple platforms
- Keep functions focused and small

## Technical Details

### Dependencies

- Standard C library
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

- [ ] JSON/CSV export options
- [ ] Historical trend analysis
- [ ] Git hook integration
- [ ] Configuration file support
- [ ] Custom report templates
- [ ] Language-specific statistics

## Support

If you encounter issues or have suggestions:

1. Check existing [Issues](https://github.com/yourusername/git-stat/issues)
2. Create a new issue with detailed information
3. Include your OS, git version, and repository size

---

Made with ❤️ for the git community