# Contributing to Personal Finance Manager

Thank you for your interest in contributing to the Personal Finance Manager project! This document provides guidelines for contributing to this open-source C-based personal finance tracking application.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Standards](#coding-standards)
- [Submitting Changes](#submitting-changes)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)
- [Community](#community)

## Code of Conduct

We are committed to providing a welcoming and inclusive environment for all contributors. Please be respectful and constructive in all interactions.

### Our Standards

- **Be Respectful**: Treat everyone with respect and kindness
- **Be Constructive**: Provide helpful feedback and suggestions
- **Be Collaborative**: Work together to improve the project
- **Be Patient**: Remember that everyone has different skill levels and time constraints

## Getting Started

### Prerequisites

Before you begin contributing, ensure you have:

- A C compiler with C11 support (GCC, Clang, or compatible)
- Git for version control
- A POSIX-compliant operating system (Linux, macOS, Unix-like systems)
- Basic understanding of C programming
- Familiarity with command-line tools

### Understanding the Project

1. **Read the Documentation**: Start by thoroughly reading the [README.md](README.md) to understand the project's purpose, features, and architecture.

2. **Explore the Code**: The project is a single-file C application (`finance.c`). Review the code structure to understand:
   - Data structures (Transaction, Category, BudgetEntry)
   - CRUD operations
   - File persistence mechanisms
   - Menu system and user interface

3. **Build and Run**: Follow the installation instructions in the README to compile and run the application locally.

## Development Setup

### 1. Fork and Clone the Repository

```bash
# Fork the repository on GitHub, then clone your fork
git clone https://github.com/YOUR_USERNAME/AI_Powdered_PersonalFinance.git
cd AI_Powdered_PersonalFinance
```

### 2. Set Up Upstream Remote

```bash
# Add the original repository as upstream
git remote add upstream https://github.com/SnehaghoshBarsha444/AI_Powdered_PersonalFinance.git

# Verify remotes
git remote -v
```

### 3. Build the Project

```bash
# Standard build
gcc -std=c11 -Wall -Wextra -o finance finance.c

# Debug build (recommended during development)
gcc -std=c11 -g -Wall -Wextra -o finance finance.c

# Optimized build
gcc -std=c11 -O2 -Wall -Wextra -o finance finance.c
```

### 4. Test the Application

```bash
# Run the application
./finance

# Test basic workflows:
# - Create categories
# - Add transactions
# - Generate reports
# - Export/import CSV
```

## How to Contribute

### Types of Contributions Welcome

1. **Bug Fixes**: Identify and fix issues in the code
2. **Feature Enhancements**: Implement new features from the roadmap
3. **Documentation**: Improve README, code comments, or create guides
4. **Code Quality**: Refactoring, optimization, or improving code structure
5. **Testing**: Add test cases or improve validation
6. **Translations**: Help make error messages clearer

### Contribution Workflow

1. **Check Existing Issues**: Look for existing issues or create a new one to discuss your proposed changes
2. **Create a Branch**: Create a feature branch for your work
3. **Make Changes**: Implement your changes following the coding standards
4. **Test Thoroughly**: Ensure your changes work and don't break existing functionality
5. **Commit**: Write clear, descriptive commit messages
6. **Push**: Push your changes to your fork
7. **Pull Request**: Create a pull request with a detailed description

## Coding Standards

### C Code Style

#### General Guidelines

- **C Standard**: Use C11 features and syntax
- **Indentation**: Use 4 spaces (no tabs)
- **Line Length**: Keep lines under 100 characters when practical
- **Braces**: Use K&R style (opening brace on same line for functions and control structures)

#### Naming Conventions

```c
// Functions: lowercase with underscores
void add_transaction();
void save_binary_file();

// Types: PascalCase
typedef struct Transaction Transaction;
typedef enum TxnType TxnType;

// Constants/Macros: UPPERCASE with underscores
#define MAX_NOTE 256
#define DATA_DIR "."

// Variables: lowercase with underscores
int transaction_count;
char category_name[64];
```

#### Comments

- **Header Comments**: Add brief function descriptions for complex functions
- **Inline Comments**: Use sparingly, only when the code isn't self-explanatory
- **TODO Comments**: Mark areas needing future work with `// TODO: description`

#### Example

```c
/* Parse date string in YYYY-MM-DD format */
int parse_date(const char *s, struct tm *out) {
    if (!s || strlen(s) < 10) return 0;
    
    int y, m, d;
    if (sscanf(s, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    
    /* Validate date ranges */
    if (y < 1900 || y > 9999 || m < 1 || m > 12 || d < 1 || d > 31) {
        return 0;
    }
    
    return 1;
}
```

### Code Quality

- **Compiler Warnings**: Fix all warnings (`-Wall -Wextra`)
- **Memory Safety**: Avoid memory leaks; use proper allocation/deallocation
- **Error Handling**: Check return values and handle errors gracefully
- **Input Validation**: Validate all user input
- **Buffer Overflow**: Use safe string functions (e.g., `strncpy`, check bounds)

### Security Considerations

- **XOR Obfuscation**: Remember that the current obfuscation is NOT cryptographically secure
- **Input Sanitization**: Always validate and sanitize user input
- **File Operations**: Use secure file handling practices
- **No Hardcoded Secrets**: Never commit passwords or sensitive data

## Submitting Changes

### Before Submitting

1. **Compile with Warnings**:
   ```bash
   gcc -std=c11 -Wall -Wextra -Wpedantic -o finance finance.c
   ```

2. **Test Your Changes**: Verify:
   - The application compiles without errors or warnings
   - Your changes work as intended
   - Existing functionality still works
   - Edge cases are handled

3. **Update Documentation**: If your changes affect usage:
   - Update README.md if needed
   - Add code comments for complex logic
   - Update any affected documentation

### Commit Message Guidelines

Write clear, concise commit messages following this format:

```
Short summary (50 characters or less)

More detailed explanation if needed. Wrap at 72 characters.
Explain the problem this commit solves and why this approach
was chosen.

Fixes #123
```

**Examples**:
```
Add date validation for transaction entries

Implement proper date range checking to prevent invalid dates
from being entered. Validates year (1900-9999), month (1-12),
and day (1-31) ranges.

Fixes #45
```

```
Fix memory leak in category management

Release allocated memory properly when removing categories.
Previously, category names weren't being freed, causing a
memory leak during bulk category deletions.
```

### Pull Request Process

1. **Create Pull Request**:
   - Use a clear, descriptive title
   - Reference related issues (e.g., "Fixes #123")
   - Provide detailed description of changes
   - List any breaking changes

2. **PR Description Template**:
   ```markdown
   ## Description
   Brief description of changes made.

   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Code refactoring
   - [ ] Documentation update
   - [ ] Other (specify)

   ## Testing Done
   Describe how you tested your changes.

   ## Checklist
   - [ ] Code compiles without errors or warnings
   - [ ] Changes have been tested
   - [ ] Documentation updated (if applicable)
   - [ ] Commit messages are clear and descriptive
   - [ ] No compiler warnings introduced

   ## Related Issues
   Fixes #(issue number)
   ```

3. **Review Process**:
   - Maintainers will review your PR
   - Address any feedback or requested changes
   - Once approved, your PR will be merged

4. **After Merge**:
   - Your contribution will be part of the project!
   - Update your fork with the latest changes from upstream

## Reporting Bugs

### Before Reporting

1. **Check Existing Issues**: Search for similar issues to avoid duplicates
2. **Verify the Bug**: Ensure it's reproducible with the latest version
3. **Collect Information**: Gather relevant details about the bug

### Bug Report Template

```markdown
**Description**
A clear description of the bug.

**To Reproduce**
Steps to reproduce the behavior:
1. Run the application with '...'
2. Select option '...'
3. Enter data '...'
4. See error

**Expected Behavior**
What you expected to happen.

**Actual Behavior**
What actually happened.

**Environment**
- OS: [e.g., Ubuntu 22.04, macOS 13.0]
- Compiler: [e.g., GCC 11.3.0, Clang 14.0]
- Compilation flags: [e.g., -std=c11 -Wall -Wextra]

**Additional Context**
Any other relevant information, error messages, or screenshots.
```

### Critical Issues

For security vulnerabilities or data corruption issues:
- **Do not** create a public issue
- Contact maintainers directly
- Provide detailed information privately

## Suggesting Features

### Feature Request Guidelines

1. **Check Roadmap**: Review [Future Enhancements](README.md#future-enhancements) in README.md
2. **Search Existing Issues**: Check if someone has already suggested it
3. **Be Specific**: Provide clear use cases and benefits

### Feature Request Template

```markdown
**Feature Description**
A clear description of the feature you'd like to see.

**Use Case**
Explain why this feature would be useful and who would benefit.

**Proposed Implementation**
If you have ideas on how to implement it, share them here.

**Alternatives Considered**
Any alternative solutions or workarounds you've thought about.

**Additional Context**
Any other information, mockups, or examples.
```

## Priority Features for Contributors

Consider contributing to these areas (from README Future Enhancements):

1. **Multiple Currency Support**: Add functionality for different currencies
2. **Recurring Transactions**: Implement automatic recurring income/expenses
3. **Enhanced Security**: Replace XOR obfuscation with proper encryption
4. **Database Backend**: Migrate from binary files to SQLite
5. **Improved Reports**: Add graphical charts and visualizations
6. **Import Enhancements**: Support various bank statement formats

## Development Tips

### Debugging

```bash
# Compile with debug symbols
gcc -std=c11 -g -Wall -Wextra -o finance finance.c

# Use GDB for debugging
gdb ./finance

# Common GDB commands:
# (gdb) break main           # Set breakpoint at main
# (gdb) run                  # Run the program
# (gdb) next                 # Step to next line
# (gdb) print variable_name  # Print variable value
# (gdb) backtrace            # Show call stack
```

### Memory Leak Detection

```bash
# Using Valgrind (if available)
valgrind --leak-check=full ./finance

# Address Sanitizer (compile-time)
gcc -std=c11 -g -fsanitize=address -Wall -Wextra -o finance finance.c
./finance
```

### Code Analysis

```bash
# Static analysis with clang
clang -std=c11 --analyze finance.c

# Check for common issues
cppcheck finance.c
```

## Project Structure

```
AI_Powdered_PersonalFinance/
├── README.md           # Project documentation
├── LICENSE             # GPL-3.0 license
├── CONTRIBUTING.md     # This file
├── finance.c           # Main application (single file)
└── .gitignore          # Git ignore rules
```

### Data Files (Generated at Runtime)
- `transactions.dat` - Binary file storing transactions
- `categories.dat` - Binary file storing categories
- `budgets.dat` - Binary file storing budget information

## License

By contributing to this project, you agree that your contributions will be licensed under the GNU General Public License v3.0 (GPL-3.0), the same license as the project.

This means:
- Your code will be open source
- Others can use, modify, and distribute your contributions
- All derivative works must also be GPL-3.0 licensed
- You retain copyright to your contributions

## Community

### Getting Help

- **Documentation**: Start with [README.md](README.md)
- **Issues**: Ask questions by creating an issue with the "question" label
- **Discussions**: Engage with other contributors in issue discussions

### Communication Guidelines

- Be clear and concise in your communications
- Provide context and examples when asking questions
- Be patient waiting for responses (contributors are volunteers)
- Help others when you can - we're all learning!

## Recognition

All contributors will be recognized for their work:
- Contributors are listed in the project's commit history
- Significant contributions may be highlighted in release notes
- We appreciate every contribution, no matter how small!

## Questions?

If you have questions not covered in this guide:
1. Check the [README.md](README.md)
2. Search existing issues for similar questions
3. Create a new issue with the "question" label
4. Be patient - maintainers will respond as soon as possible

---

Thank you for contributing to Personal Finance Manager! Your efforts help make financial management accessible to everyone.

**Happy Coding! 💰💻**
