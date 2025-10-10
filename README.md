# Personal Finance Manager

A command-line personal finance management system written in C for tracking income, expenses, budgets, and generating financial reports.

## Overview

This is a single-file C application that provides comprehensive personal finance tracking capabilities. It stores data locally using binary files with optional XOR obfuscation for basic data protection.

## Features

### Transaction Management
- **Add Transactions**: Record income and expenses with date, amount, category, and notes
- **List Transactions**: View all transactions or filter by date range
- **Edit Transactions**: Modify existing transaction details
- **Delete Transactions**: Remove transactions from the system
- **Search Transactions**: Advanced search by date range, category, amount range, and text in notes

### Category Management
- **Create Categories**: Organize transactions into custom categories
- **List Categories**: View all available categories
- **Edit Categories**: Rename existing categories
- **Delete Categories**: Remove unused categories (protected if transactions reference them)

### Budget Tracking
- **Set Budgets**: Define monthly spending limits for each category
- **List Budgets**: View all configured budgets
- **Budget Reports**: Compare actual spending against budget limits

### Reporting
- **Monthly Summary**: Overview of total income and expenses for a specific month
- **Category Summary**: Breakdown of spending by category for a given period
- **Budget Report**: Analysis of budget adherence with warnings for overspending

### Data Import/Export
- **CSV Export**: Export all transactions to CSV format for backup or analysis
- **CSV Import**: Import transactions from CSV files
- **File Obfuscation**: Optional XOR-based obfuscation for data files (basic protection, not cryptographically secure)

## Requirements

- C compiler with C11 support (GCC, Clang, or compatible)
- Standard C libraries (no external dependencies)
- POSIX-compliant operating system (Linux, macOS, Unix-like systems)

## Installation

### Building from Source

1. Clone or download the repository:
```bash
git clone https://github.com/SnehaghoshBarsha444/AI_Powdered_PersonalFinance.git
cd AI_Powdered_PersonalFinance
```

2. Compile the program:
```bash
gcc -std=c11 -Wall -Wextra -o finance finance.c
```

3. Run the application:
```bash
./finance
```

### Alternative Build Options

For optimized builds:
```bash
gcc -std=c11 -O2 -Wall -Wextra -o finance finance.c
```

For debugging:
```bash
gcc -std=c11 -g -Wall -Wextra -o finance finance.c
```

## Usage

### Starting the Application

Run the compiled program:
```bash
./finance
```

You will be presented with an interactive menu:

```
=== Menu ===
1) Add transaction
2) List transactions
3) Edit transaction
4) Delete transaction
5) Add category
6) List/Edit/Delete categories
7) Set/List budgets
8) Reports (monthly/category/budget)
9) Export CSV
10) Import CSV
11) Search transactions
12) Toggle file obfuscation (current: OFF)
0) Save & Exit
```

### Basic Workflow

1. **Create Categories** (Option 5):
   - Start by creating categories for your transactions (e.g., "Groceries", "Salary", "Rent")
   - Categories help organize and track spending patterns

2. **Add Transactions** (Option 1):
   - Record income and expenses
   - Dates follow YYYY-MM-DD format (e.g., 2024-03-15)
   - Select transaction type: 0 for Expense, 1 for Income
   - Assign a category to each transaction
   - Optionally add notes for reference

3. **View Transactions** (Option 2):
   - List all transactions or filter by date range
   - Review your financial activity

4. **Set Budgets** (Option 7):
   - Define monthly spending limits for categories
   - Track budget adherence over time

5. **Generate Reports** (Option 8):
   - View monthly summaries of income and expenses
   - Analyze spending by category
   - Check budget compliance with alerts for overspending

6. **Search Transactions** (Option 11):
   - Find specific transactions using multiple criteria
   - Filter by date range, category, amount, or note content

7. **Export Data** (Option 9):
   - Backup your data to CSV format
   - Analyze data in spreadsheet applications

### Date Format

All dates must be entered in **YYYY-MM-DD** format:
- Valid: `2024-03-15`, `2023-12-31`, `2024-01-01`
- Invalid: `15-03-2024`, `3/15/2024`, `2024-3-15`

### Data Files

The application stores data in the current working directory:
- `transactions.dat` - Transaction records
- `categories.dat` - Category definitions
- `budgets.dat` - Budget settings

**Important**: Do not manually edit these binary files. Use the application's import/export features for data manipulation.

## Data Security

### File Obfuscation

The application includes an optional XOR-based obfuscation feature (Option 12):
- Provides basic protection against casual viewing
- **NOT cryptographically secure** - do not rely on this for sensitive data
- Use proper encryption tools if strong security is required

### Backup Recommendations

- Regularly export your data to CSV format (Option 9)
- Keep backups of the `.dat` files in a secure location
- Consider using encrypted volumes or drives for sensitive financial data

## CSV Import/Export

### Export Format

Exported CSV files contain the following columns:
```
id,date,type,amount,category,note
```

Example:
```csv
id,date,type,amount,category,note
1,2024-03-15,0,45.50,Groceries,Weekly shopping
2,2024-03-01,1,3000.00,Salary,Monthly salary
```

### Import Requirements

When importing CSV files:
- First row must be a header (will be skipped)
- Categories must already exist in the system
- Transaction type: 0 = Expense, 1 = Income
- Dates must follow YYYY-MM-DD format

## Example Usage Session

```
./finance

Personal Finance Manager (C) — Advanced
Note: This program stores data in current directory.
Optional file obfuscation (XOR) is available from menu.

=== Menu ===
...
Choice: 5
Category name: Groceries
Added category 'Groceries' (id=1).

Choice: 1
Date (YYYY-MM-DD) [default 2024-03-15]: 
Type: 0=Expense, 1=Income [0]: 0
Amount: 45.50
Categories:
  1: Groceries
Enter category id for this transaction: 1
Note (optional): Weekly shopping
Added transaction #1

Choice: 2
List all or range? (a/r) a
=== Transactions ===
ID=1 Date=2024-03-15 [Expense] Amount=45.50 Category=Groceries Note=Weekly shopping

Choice: 0
Goodbye.
```

## Technical Details

### Architecture
- Single-file C program (~800 lines)
- In-memory data structures with persistent storage
- Dynamic array allocation for scalability
- Binary file format for efficient storage

### Data Structures
- **Transaction**: Stores financial transactions with date, amount, type, category, and notes
- **Category**: Defines transaction categories
- **Budget**: Monthly spending limits per category

### Memory Management
- Dynamic memory allocation with automatic capacity expansion
- Graceful handling of memory allocation failures
- Clean shutdown with data persistence

## Limitations

- No multi-user support
- Basic XOR obfuscation (not secure encryption)
- Local storage only (no cloud sync)
- Command-line interface only
- Single-currency support

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0).

See the [LICENSE](LICENSE) file for full license text.

## Troubleshooting

### Compilation Errors
- Ensure you have a C11-compliant compiler installed
- Try different compilers (gcc, clang) if issues persist
- Check that all required headers are available

### Runtime Issues
- **Data files corrupted**: Use CSV export/import to recover data
- **Permission errors**: Ensure write access to the current directory
- **Segmentation faults**: Report with details of actions leading to the crash

### Data Recovery
If data files become corrupted:
1. Stop the application immediately
2. Backup the existing `.dat` files
3. If you have CSV exports, use the import feature to restore data
4. Consider implementing regular automated backups

## Best Practices

1. **Regular Backups**: Export to CSV weekly or monthly
2. **Consistent Categories**: Use standardized category names
3. **Detailed Notes**: Add context to transactions for future reference
4. **Budget Reviews**: Check budget reports monthly to track spending
5. **Date Accuracy**: Always verify transaction dates before saving
6. **Clean Data**: Remove test or erroneous entries promptly

## Future Enhancements

Potential improvements for future versions:
- Multiple currency support
- Recurring transactions
- Graphical reports and charts
- Database backend (SQLite)
- Web interface
- Multi-user accounts
- Secure encryption
- Mobile application
- Cloud synchronization
- Automated bank statement imports
