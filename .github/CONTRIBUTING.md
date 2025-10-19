# Contributing to Git Statistics Utility

Thank you for your interest in contributing to git-stat! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Contributing Guidelines](#contributing-guidelines)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Documentation](#documentation)
- [Release Process](#release-process)
- [Getting Help](#getting-help)

## Code of Conduct

This project follows a Code of Conduct to ensure a welcoming environment for all contributors. By participating, you agree to uphold this code.

### Our Standards

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

### Prerequisites

Before you begin, ensure you have:

- A C compiler (GCC 7+ or Clang 10+)
- Git installed and configured
- Make (GNU Make recommended)
- Basic familiarity with C programming and git repositories

### First Contribution

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/yourusername/git-stat.git
   cd git-stat
   ```
3. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. **Make your changes** following our coding standards
5. **Test your changes** thoroughly
6. **Submit a pull request**

### Good First Issues

Look for issues labeled with:
- `good first issue` - Perfect for newcomers
- `help wanted` - Community input needed
- `documentation` - Help improve docs
- `enhancement` - New features or improvements

## Development Setup

### Building the Project

```bash
# Clean build
make clean
make

# Debug build with extra information
make debug

# Build with specific compiler
make CC=clang
make CC=gcc
```

### Project Structure

```
git-stat/
├── src/                     # Source code
│   ├── main.c              # Entry point
│   ├── git_stats.c/.h      # Core statistics
│   ├── version.h           # Version definitions
│   ├── analysis/           # Analysis modules
│   │   ├── hotspots.c/.h   # Hotspot detection
│   │   └── activity.c/.h   # Activity analysis
│   ├── output/             # Output formatters
│   │   ├── formatters.h    # Output interfaces
│   │   ├── human_output.c  # Human-readable output
│   │   └── json_output.c   # JSON output
│   └── utils/              # Utility functions
│       ├── string_utils.c/.h
│       └── git_commands.c/.h
├── Makefile                # Build configuration
├── README.md              # User documentation
├── CODING_STANDARDS.md    # Code style guide
└── .github/               # GitHub workflows and templates
```

### Development Workflow

1. **Always work on a feature branch**
2. **Keep commits atomic and well-described**
3. **Test on multiple platforms** if possible
4. **Update documentation** for user-facing changes
5. **Run static analysis** before submitting

## Contributing Guidelines

### Types of Contributions

We welcome various types of contributions:

#### 🐛 Bug Reports
- Use the bug report template
- Include reproduction steps
- Provide system information
- Include error messages/output

#### ✨ Feature Requests
- Use the feature request template
- Describe the problem and proposed solution
- Consider implementation complexity
- Ensure alignment with project goals

#### 📝 Documentation
- Fix typos and improve clarity
- Add examples and use cases
- Update installation instructions
- Improve code comments

#### 🔧 Code Contributions
- Bug fixes
- Performance improvements
- New analysis features
- Code refactoring
- Test improvements

### What We're Looking For

**High Priority:**
- Bug fixes and stability improvements
- Performance optimizations
- Better error handling
- Cross-platform compatibility
- Memory safety improvements

**Medium Priority:**
- New analysis features (complexity metrics, language detection)
- Output format improvements
- Additional export formats (CSV, XML)
- Better documentation and examples

**Lower Priority:**
- GUI interfaces (keep CLI focus)
- Network-dependent features
- Heavy external dependencies

### What We're Not Looking For

- Features requiring network connectivity
- GUI applications (this is a CLI tool)
- Changes that significantly increase binary size
- Non-portable or platform-specific code without justification
- Features that duplicate existing functionality

## Pull Request Process

### Before Submitting

1. **Ensure your code compiles** without warnings
2. **Test thoroughly** on different repository types
3. **Run static analysis** (make lint)
4. **Update documentation** if needed
5. **Check your code style** matches our standards

### PR Checklist

- [ ] Code follows project coding standards
- [ ] All tests pass (when available)
- [ ] Documentation updated for user-facing changes
- [ ] Commit messages are clear and descriptive
- [ ] PR description explains the changes
- [ ] Related issues are referenced

### PR Review Process

1. **Automated checks** must pass (CI/CD pipeline)
2. **Code review** by maintainers
3. **Testing** on multiple platforms
4. **Documentation review** if applicable
5. **Final approval** and merge

### Feedback and Iteration

- Respond to review comments promptly
- Make requested changes in new commits
- Squash commits before final merge if requested
- Be open to suggestions and alternative approaches

## Coding Standards

We follow strict coding standards to maintain code quality and consistency. See [CODING_STANDARDS.md](CODING_STANDARDS.md) for detailed guidelines.

### Key Points

- **C17 standard** compliance
- **4 spaces** for indentation (no tabs)
- **Snake_case** for functions and variables
- **UPPER_CASE** for constants and macros
- **Comprehensive error handling**
- **Memory safety** (bounds checking, proper cleanup)
- **Clear, descriptive names**

### Static Analysis

Run these tools before submitting:

```bash
# Check code style and potential issues
make lint

# Check with cppcheck (install separately)
cppcheck --enable=all src/

# Check with clang-tidy
clang-tidy src/**/*.c -- -Wall -Wextra -std=c17
```

## Testing

### Manual Testing

Test your changes with various repository types:

```bash
# Basic functionality
./git-stat --help
./git-stat

# Different output formats
./git-stat --output json
./git-stat --hotspots --activity

# Edge cases
cd /tmp && mkdir test-repo && cd test-repo
git init && git config user.name "Test" && git config user.email "test@example.com"
echo "test" > file.txt && git add . && git commit -m "test"
/path/to/git-stat
```

### Test Different Repository Types

- Small repositories (< 100 commits)
- Medium repositories (100-10k commits)
- Large repositories (> 10k commits)
- Repositories with unusual characteristics:
  - Binary files
  - Non-ASCII filenames
  - Very long commit messages
  - Many branches
  - Merge commits

### Performance Testing

```bash
# Time the analysis
time ./git-stat

# Check memory usage (Linux)
valgrind --tool=massif ./git-stat

# Profile with gprof (if compiled with -pg)
gprof ./git-stat gmon.out > profile.txt
```

## Documentation

### Code Documentation

- **Header files** should document all public functions
- **Complex algorithms** need inline comments
- **Error handling** should be documented
- **Memory management** patterns should be clear

### User Documentation

- Update **README.md** for new features
- Add **examples** for new command-line options
- Update **help text** in the code
- Consider adding **troubleshooting** sections

### Documentation Style

- Use clear, concise language
- Include practical examples
- Explain the "why" not just the "how"
- Keep information up-to-date

## Release Process

### Version Numbers

We use [Semantic Versioning](https://semver.org/):
- **MAJOR.MINOR.PATCH** (e.g., 1.2.3)
- **MAJOR**: Breaking changes
- **MINOR**: New features (backward compatible)
- **PATCH**: Bug fixes

### Release Checklist

1. Update version in `src/version.h`
2. Update CHANGELOG.md
3. Create release branch if needed
4. Test extensively on all platforms
5. Create GitHub release with binaries
6. Update documentation

### Pre-release Testing

Before major releases:
- Test on all supported platforms
- Run extended performance tests
- Verify all examples in documentation
- Check for memory leaks and security issues

## Getting Help

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests, questions
- **GitHub Discussions**: General discussion, ideas, help
- **Pull Request Comments**: Code-specific discussions

### Questions and Support

- Check existing issues before creating new ones
- Use issue templates for better responses
- Provide context and examples
- Be patient and respectful

### Maintainer Response Times

We aim to:
- Acknowledge new issues within 48 hours
- Review PRs within 1 week
- Release bug fixes within 2 weeks of confirmation
- Release new features based on roadmap priorities

## Recognition

### Contributors

All contributors are recognized in:
- Git commit history
- GitHub contributors list
- Release notes for significant contributions
- Special thanks for major features or fixes

### Contribution Types

We value all types of contributions:
- Code contributions
- Documentation improvements
- Bug reports and testing
- Feature suggestions
- Community support

## Project Governance

### Maintainers

Current maintainers have:
- Commit access to the main repository
- Authority to merge pull requests
- Responsibility for code review
- Decision-making power for project direction

### Decision Making

- **Minor changes**: Can be made by any maintainer
- **Major changes**: Require discussion and consensus
- **Breaking changes**: Require RFC process and community input
- **Architecture changes**: Require design review

## Development Roadmap

### Current Focus

- Stability and bug fixes
- Performance optimizations
- Cross-platform compatibility
- Memory safety improvements

### Planned Features

- Additional analysis types (complexity metrics, language detection)
- More export formats (CSV, XML)
- Configuration file support
- Git hook integration

### Long-term Goals

- Maintain zero external dependencies
- Support very large repositories efficiently
- Comprehensive test suite
- Extensive documentation and examples

---

## Thank You! 🎉

Thank you for contributing to git-stat! Your efforts help make this tool better for developers everywhere.

**Questions?** Feel free to open an issue or start a discussion. We're here to help!

**First time contributing to open source?** Welcome! We're happy to help you get started.

**Experienced contributor?** We appreciate your expertise and guidance for newer contributors.

Happy coding! 🚀