# Pull Request

## Description

Brief description of the changes in this pull request.

### Type of Change

Please delete options that are not relevant:

- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Performance improvement
- [ ] Code refactoring (no functional changes)
- [ ] Documentation update
- [ ] Build/CI changes
- [ ] Other (please describe):

## Related Issues

Fixes #(issue number)
Closes #(issue number)
Relates to #(issue number)

## Changes Made

### Core Changes
- [ ] Modified core analysis logic
- [ ] Updated data structures
- [ ] Changed command-line interface
- [ ] Added new analysis features
- [ ] Modified output formatting

### Specific Files Changed
List the main files modified and what was changed:

- `src/main.c`: 
- `src/git_stats.c`: 
- `src/analysis/`: 
- `src/output/`: 
- `Makefile`: 
- `README.md`: 

## Testing

### Manual Testing
- [ ] Tested with small repositories (< 100 commits)
- [ ] Tested with medium repositories (100-10k commits)
- [ ] Tested with large repositories (> 10k commits)
- [ ] Tested all command-line options
- [ ] Tested JSON output format
- [ ] Tested human-readable output format
- [ ] Tested error handling (invalid repositories, etc.)

### Platforms Tested
- [ ] Linux (Ubuntu/Debian)
- [ ] Linux (Other distribution): 
- [ ] macOS (Intel)
- [ ] macOS (Apple Silicon)
- [ ] Windows (WSL)
- [ ] Windows (MinGW/MSYS2)

### Compilers Tested
- [ ] GCC version: 
- [ ] Clang version: 
- [ ] Other: 

### Test Commands Used
```bash
# List the exact commands you used to test
git-stat --help
git-stat
git-stat --output json
git-stat --hotspots
git-stat --activity
# Add any other specific test commands
```

## Performance Impact

### Memory Usage
- [ ] No significant change in memory usage
- [ ] Reduced memory usage
- [ ] Increased memory usage (justified because: )

### Execution Time
- [ ] No significant change in execution time
- [ ] Improved performance
- [ ] Slower performance (justified because: )

### Repository Size Impact
How do these changes affect analysis of different repository sizes?

- Small repos (< 100 commits): 
- Medium repos (100-10k commits): 
- Large repos (> 10k commits): 

## Code Quality

### Code Standards
- [ ] Follows C17 standard
- [ ] Uses consistent indentation (4 spaces)
- [ ] No trailing whitespace
- [ ] No tabs (spaces only)
- [ ] Proper error handling
- [ ] Memory management (no leaks)
- [ ] Bounds checking for arrays/buffers
- [ ] Meaningful variable and function names

### Documentation
- [ ] Added/updated comments for complex logic
- [ ] Updated header file documentation
- [ ] Updated README if user-facing changes
- [ ] Updated CODING_STANDARDS.md if needed

### Static Analysis
- [ ] Passes clang-tidy without warnings
- [ ] Passes cppcheck without issues
- [ ] No compiler warnings with -Wall -Wextra

## Backward Compatibility

- [ ] This change is fully backward compatible
- [ ] This change has minor breaking changes (documented below)
- [ ] This change has major breaking changes (documented below)

### Breaking Changes (if any)
Describe any breaking changes and migration path:

## Security Considerations

- [ ] No security implications
- [ ] Security improvements
- [ ] Potential security concerns (documented below)

### Security Notes
If there are security implications, describe them:

## Dependencies

- [ ] No new dependencies added
- [ ] New system dependencies (list below)
- [ ] New build dependencies (list below)

### New Dependencies (if any)
List any new dependencies and justify why they're needed:

## Integration Impact

### Command-Line Interface
- [ ] No changes to CLI
- [ ] New command-line options (documented below)
- [ ] Changed existing options (documented below)

### Output Format Changes
- [ ] No changes to output format
- [ ] Added new output fields
- [ ] Changed existing output fields (backward compatible)
- [ ] Breaking changes to output format

### New CLI Options (if any)
Document new command-line options:

```bash
--new-option    Description of what this option does
```

## Examples

### Before (if changing existing functionality)
```
# Show example of old behavior
git-stat --example
```

### After
```
# Show example of new behavior
git-stat --example
```

## Screenshots (if applicable)

If this change affects output formatting, include before/after screenshots or copy the text output.

## Checklist

### Development
- [ ] My code follows the project's coding standards
- [ ] I have performed a self-review of my code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have made corresponding changes to the documentation
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works (if applicable)
- [ ] New and existing unit tests pass locally with my changes (if applicable)

### Testing
- [ ] I have tested this change on multiple platforms
- [ ] I have tested this change with repositories of different sizes
- [ ] I have tested edge cases and error conditions
- [ ] I have verified memory usage and performance impact

### Documentation
- [ ] Updated README.md if needed
- [ ] Updated help text if adding new options
- [ ] Updated CODING_STANDARDS.md if needed
- [ ] Added inline documentation for new functions

### Git Hygiene
- [ ] My commit messages are clear and descriptive
- [ ] I have rebased my branch against the latest main
- [ ] My branch has a clean commit history
- [ ] I have resolved any merge conflicts

## Additional Notes

Add any additional notes, concerns, or questions for reviewers:

---

## For Reviewers

### Review Focus Areas
Please pay special attention to:
- [ ] Memory management and potential leaks
- [ ] Buffer overflow protection
- [ ] Error handling completeness
- [ ] Performance implications
- [ ] Code clarity and maintainability
- [ ] Test coverage adequacy

### Review Checklist
- [ ] Code follows project standards
- [ ] Changes are well-tested
- [ ] Documentation is adequate
- [ ] Performance impact is acceptable
- [ ] Security implications considered
- [ ] Backward compatibility maintained (or breaking changes justified)

Thank you for your contribution to git-stat! 🚀