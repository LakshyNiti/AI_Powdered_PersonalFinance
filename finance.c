/* finance.c
   Personal Finance Management System (advanced)
   Single-file C program (C11). No external libs required.
   NOTE: File "encryption" is a simple XOR obfuscation (NOT secure).
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DATA_DIR "."
#define TRAN_FILE DATA_DIR "/transactions.dat"
#define CAT_FILE DATA_DIR "/categories.dat"
#define BUD_FILE DATA_DIR "/budgets.dat"
#define TEMP_FILE DATA_DIR "/tmp_import.csv"
#define MAX_NOTE 256
#define DATE_STRLEN 11 /* "YYYY-MM-DD" + NUL */

typedef enum { TYPE_EXPENSE = 0, TYPE_INCOME = 1 } TxnType;

typedef struct {
    int id;                 /* unique id */
    char date[DATE_STRLEN]; /* "YYYY-MM-DD" */
    double amount;
    int category_id;        /* link to category */
    TxnType type;           /* expense/income */
    char note[MAX_NOTE];
} Transaction;

typedef struct {
    int id;
    char name[64];
} Category;

typedef struct {
    int category_id;
    int year;
    int month; /* 1..12 */
    double amount; /* budget amount for that month */
} BudgetEntry;

/* Dynamic arrays */
typedef struct {
    Transaction *data;
    size_t size;
    size_t cap;
    int next_id;
} TxnStore;

typedef struct {
    Category *data;
    size_t size;
    size_t cap;
    int next_id;
} CatStore;

typedef struct {
    BudgetEntry *data;
    size_t size;
    size_t cap;
} BudgetStore;

/* Global in-memory stores */
static TxnStore txns = {NULL,0,0,1};
static CatStore cats = {NULL,0,0,1};
static BudgetStore budgets = {NULL,0,0};

/* Simple XOR obfuscation for file content (optional) */
static int obfuscate_enabled = 0;
static unsigned char obf_key = 0;

/* Utility forward declarations */
void panic(const char *msg);
void *xmalloc(size_t s);
void ensure_txn_capacity();
void ensure_cat_capacity();
void ensure_budget_capacity();
int find_category_by_id(int id);
int find_category_index_by_id(int id);
int next_int_id_from_store();

/* Persistence */
void load_all();
void save_all();
void save_binary_file(const char *path, void *buf, size_t count, size_t sz);
size_t load_binary_file(const char *path, void *buf, size_t max_count, size_t sz);
void obfuscate_buffer(unsigned char *buf, size_t len);

/* CRUD */
void add_category();
void list_categories();
void edit_category();
void remove_category();

void add_transaction();
void list_transactions(const char *start_date, const char *end_date);
void edit_transaction();
void remove_transaction();

void set_budget();
void list_budgets();
double total_for_category_month(int cat_id, int year, int month);

/* Reports */
void monthly_summary(int year, int month);
void category_summary(int year, int month);
void budget_report(int year, int month);

/* Utilities: parse/format date, CSV import/export, search */
int parse_date(const char *s, struct tm *out);
int compare_dates(const char *a, const char *b); /* lexicographic works for YYYY-MM-DD */
void export_csv(const char *path);
void import_csv(const char *path);
void search_transactions();
void prompt_press_enter();
void clear_input();

int read_int();
double read_double();
void read_line(char *buf, size_t sz);

/* Simple interactive menu */
void interactive_menu();

int main(void) {
    printf("Personal Finance Manager (C) — Advanced\n");
    printf("Note: This program stores data in current directory.\n");
    printf("Optional file obfuscation (XOR) is available from menu.\n\n");
    load_all();
    interactive_menu();
    save_all();
    printf("Goodbye.\n");
    return 0;
}

/* -------------------- Utility implementations -------------------- */

void panic(const char *msg) {
    fprintf(stderr, "Fatal: %s\n", msg);
    exit(EXIT_FAILURE);
}

void *xmalloc(size_t s) {
    void *p = malloc(s);
    if (!p) panic("out of memory");
    return p;
}

void ensure_txn_capacity() {
    if (txns.size + 1 > txns.cap) {
        txns.cap = (txns.cap == 0) ? 16 : txns.cap * 2;
        txns.data = realloc(txns.data, txns.cap * sizeof(Transaction));
        if (!txns.data) panic("realloc txns");
    }
}
void ensure_cat_capacity() {
    if (cats.size + 1 > cats.cap) {
        cats.cap = (cats.cap == 0) ? 8 : cats.cap * 2;
        cats.data = realloc(cats.data, cats.cap * sizeof(Category));
        if (!cats.data) panic("realloc cats");
    }
}
void ensure_budget_capacity() {
    if (budgets.size + 1 > budgets.cap) {
        budgets.cap = (budgets.cap == 0) ? 8 : budgets.cap * 2;
        budgets.data = realloc(budgets.data, budgets.cap * sizeof(BudgetEntry));
        if (!budgets.data) panic("realloc budgets");
    }
}

int find_category_by_id(int id) {
    for (size_t i = 0; i < cats.size; ++i) {
        if (cats.data[i].id == id) return 1;
    }
    return 0;
}
int find_category_index_by_id(int id) {
    for (size_t i = 0; i < cats.size; ++i) {
        if (cats.data[i].id == id) return (int)i;
    }
    return -1;
}

/* -------------------- Persistence -------------------- */

void obfuscate_buffer(unsigned char *buf, size_t len) {
    if (!obfuscate_enabled || obf_key == 0) return;
    for (size_t i = 0; i < len; ++i) buf[i] ^= obf_key;
}

void save_binary_file(const char *path, void *buf, size_t count, size_t sz) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Warning: unable to save %s\n", path);
        return;
    }
    size_t bytes = count * sz;
    unsigned char *tmp = xmalloc(bytes);
    memcpy(tmp, buf, bytes);
    obfuscate_buffer(tmp, bytes);
    fwrite(tmp, 1, bytes, f);
    free(tmp);
    fclose(f);
}

size_t load_binary_file(const char *path, void *buf, size_t max_count, size_t sz) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    if (filesize <= 0) { fclose(f); return 0; }
    fseek(f, 0, SEEK_SET);
    size_t bytes = (size_t)filesize;
    unsigned char *tmp = xmalloc(bytes);
    size_t got = fread(tmp, 1, bytes, f);
    fclose(f);
    obfuscate_buffer(tmp, got);
    size_t count = got / sz;
    if (count > max_count) count = max_count;
    memcpy(buf, tmp, count * sz);
    free(tmp);
    return count;
}

void load_all() {
    /* load categories */
    Category catbuf[1024];
    size_t count = load_binary_file(CAT_FILE, catbuf, 1024, sizeof(Category));
    if (count) {
        cats.data = malloc(count * sizeof(Category));
        memcpy(cats.data, catbuf, count * sizeof(Category));
        cats.size = count;
        cats.cap = count;
        /* find next id */
        int maxid = 0;
        for (size_t i = 0; i < cats.size; ++i) if (cats.data[i].id > maxid) maxid = cats.data[i].id;
        cats.next_id = maxid + 1;
    }

    /* load transactions (we allow many) */
    /* Load iteratively */
    FILE *ft = fopen(TRAN_FILE, "rb");
    if (ft) {
        fseek(ft, 0, SEEK_END);
        long filesize = ftell(ft);
        fseek(ft, 0, SEEK_SET);
        if (filesize > 0) {
            unsigned char *tmp = xmalloc((size_t)filesize);
            fread(tmp, 1, (size_t)filesize, ft);
            fclose(ft);
            obfuscate_buffer(tmp, (size_t)filesize);
            size_t countt = (size_t)filesize / sizeof(Transaction);
            txns.data = malloc(countt * sizeof(Transaction));
            memcpy(txns.data, tmp, countt * sizeof(Transaction));
            txns.size = countt;
            txns.cap = countt;
            free(tmp);
            int maxid = 0;
            for (size_t i = 0; i < txns.size; ++i) if (txns.data[i].id > maxid) maxid = txns.data[i].id;
            txns.next_id = maxid + 1;
        } else fclose(ft);
    }

    /* load budgets */
    BudgetEntry bbuf[1024];
    size_t bc = load_binary_file(BUD_FILE, bbuf, 1024, sizeof(BudgetEntry));
    if (bc) {
        budgets.data = malloc(bc * sizeof(BudgetEntry));
        memcpy(budgets.data, bbuf, bc * sizeof(BudgetEntry));
        budgets.size = bc;
        budgets.cap = bc;
    }
}

void save_all() {
    if (cats.size) save_binary_file(CAT_FILE, cats.data, cats.size, sizeof(Category));
    if (txns.size) save_binary_file(TRAN_FILE, txns.data, txns.size, sizeof(Transaction));
    if (budgets.size) save_binary_file(BUD_FILE, budgets.data, budgets.size, sizeof(BudgetEntry));
}

/* -------------------- CRUD Category -------------------- */

void add_category() {
    char name[64];
    printf("Category name: ");
    read_line(name, sizeof(name));
    if (strlen(name) == 0) { printf("Empty name aborted.\n"); return; }
    ensure_cat_capacity();
    Category c;
    c.id = cats.next_id++;
    strncpy(c.name, name, sizeof(c.name)-1);
    c.name[sizeof(c.name)-1] = '\0';
    cats.data[cats.size++] = c;
    printf("Added category '%s' (id=%d).\n", c.name, c.id);
}

void list_categories() {
    printf("Categories:\n");
    if (cats.size == 0) { printf(" (none)\n"); return; }
    for (size_t i = 0; i < cats.size; ++i) {
        printf("  id=%d  %s\n", cats.data[i].id, cats.data[i].name);
    }
}

void edit_category() {
    list_categories();
    printf("Enter category id to edit: ");
    int id = read_int();
    int idx = find_category_index_by_id(id);
    if (idx < 0) { printf("Not found.\n"); return; }
    printf("New name (enter for keep '%s'): ", cats.data[idx].name);
    char buf[64];
    read_line(buf, sizeof(buf));
    if (strlen(buf)) strncpy(cats.data[idx].name, buf, sizeof(cats.data[idx].name)-1);
    printf("Updated.\n");
}

void remove_category() {
    list_categories();
    printf("Enter category id to remove: ");
    int id = read_int();
    int idx = find_category_index_by_id(id);
    if (idx < 0) { printf("Not found.\n"); return; }
    /* check transactions referencing it */
    for (size_t i = 0; i < txns.size; ++i) {
        if (txns.data[i].category_id == id) {
            printf("Category used by transactions — cannot delete.\n");
            return;
        }
    }
    /* remove by swapping last */
    cats.data[idx] = cats.data[cats.size-1];
    cats.size--;
    printf("Deleted.\n");
}

/* -------------------- CRUD Transactions -------------------- */

void add_transaction() {
    Transaction t;
    ensure_txn_capacity();
    t.id = txns.next_id++;

    /* date */
    time_t now = time(NULL);
    struct tm *tmnow = localtime(&now);
    char default_date[DATE_STRLEN];
    snprintf(default_date, sizeof(default_date), "%04d-%02d-%02d", tmnow->tm_year+1900, tmnow->tm_mon+1, tmnow->tm_mday);
    printf("Date (YYYY-MM-DD) [default %s]: ", default_date);
    read_line(t.date, sizeof(t.date));
    if (strlen(t.date) == 0) strcpy(t.date, default_date);
    if (!parse_date(t.date, NULL)) { printf("Invalid date format.\n"); return; }

    /* type */
    printf("Type: 0=Expense, 1=Income [0]: ");
    int tp = read_int();
    if (tp != 1) t.type = TYPE_EXPENSE; else t.type = TYPE_INCOME;

    /* amount */
    printf("Amount: ");
    t.amount = read_double();
    if (t.amount <= 0) { printf("Amount must be > 0.\n"); return; }

    /* category */
    list_categories();
    if (cats.size == 0) {
        printf("No categories exist — create one now.\n");
        add_category();
        if (cats.size == 0) { printf("No categories — abort.\n"); return; }
    }
    printf("Enter category id for this transaction: ");
    int cid = read_int();
    if (!find_category_by_id(cid)) { printf("Invalid category.\n"); return; }
    t.category_id = cid;

    /* note */
    printf("Note (optional): ");
    read_line(t.note, sizeof(t.note));
    txns.data[txns.size++] = t;
    printf("Transaction added (id=%d).\n", t.id);
}

void list_transactions(const char *start_date, const char *end_date) {
    printf("Transactions:");
    if (txns.size == 0) { printf(" (none)\n"); return; }
    printf("\n");
    for (size_t i = 0; i < txns.size; ++i) {
        Transaction *t = &txns.data[i];
        if (start_date && compare_dates(t->date, start_date) < 0) continue;
        if (end_date && compare_dates(t->date, end_date) > 0) continue;
        int idx = find_category_index_by_id(t->category_id);
        const char *cname = (idx >= 0) ? cats.data[idx].name : "UNKNOWN";
        printf("  id=%d  %s  %s  %.2f  [%s]  %s\n",
               t->id,
               t->date,
               (t->type == TYPE_INCOME ? "IN" : "EX"),
               t->amount,
               cname,
               t->note);
    }
}

int find_txn_index_by_id(int id) {
    for (size_t i = 0; i < txns.size; ++i) if (txns.data[i].id == id) return (int)i;
    return -1;
}

void edit_transaction() {
    list_transactions(NULL, NULL);
    printf("Enter transaction id to edit: ");
    int id = read_int();
    int idx = find_txn_index_by_id(id);
    if (idx < 0) { printf("Not found.\n"); return; }
    Transaction *t = &txns.data[idx];
    printf("Date [%s]: ", t->date);
    char buf[DATE_STRLEN];
    read_line(buf, sizeof(buf));
    if (strlen(buf)) {
        if (parse_date(buf, NULL)) strncpy(t->date, buf, sizeof(t->date));
        else printf("Invalid date — kept.\n");
    }
    printf("Type 0=Expense,1=Income [%d]: ", t->type);
    int tp = read_int();
    if (tp == 0 || tp == 1) t->type = tp;
    printf("Amount [%.2f]: ", t->amount);
    double a = read_double();
    if (a > 0) t->amount = a;
    list_categories();
    printf("Category id [%d]: ", t->category_id);
    int cid = read_int();
    if (cid && find_category_by_id(cid)) t->category_id = cid;
    printf("Note [%s]: ", t->note);
    char notebuf[MAX_NOTE];
    read_line(notebuf, sizeof(notebuf));
    if (strlen(notebuf)) strncpy(t->note, notebuf, sizeof(t->note)-1);
    printf("Updated.\n");
}

void remove_transaction() {
    list_transactions(NULL, NULL);
    printf("Enter transaction id to delete: ");
    int id = read_int();
    int idx = find_txn_index_by_id(id);
    if (idx < 0) { printf("Not found.\n"); return; }
    txns.data[idx] = txns.data[txns.size-1];
    txns.size--;
    printf("Deleted.\n");
}

/* -------------------- Budgets -------------------- */

void set_budget() {
    list_categories();
    printf("Enter category id to set budget: ");
    int cid = read_int();
    if (!find_category_by_id(cid)) { printf("Invalid category.\n"); return; }
    printf("Year (e.g., 2025): ");
    int year = read_int();
    printf("Month (1-12): ");
    int month = read_int();
    if (month < 1 || month > 12) { printf("Invalid month.\n"); return; }
    printf("Budget amount for %04d-%02d: ", year, month);
    double amt = read_double();
    if (amt < 0) { printf("Invalid amount.\n"); return; }
    /* find existing */
    for (size_t i = 0; i < budgets.size; ++i) {
        if (budgets.data[i].category_id == cid && budgets.data[i].year == year && budgets.data[i].month == month) {
            budgets.data[i].amount = amt;
            printf("Updated budget.\n");
            return;
        }
    }
    ensure_budget_capacity();
    BudgetEntry be = {cid, year, month, amt};
    budgets.data[budgets.size++] = be;
    printf("Budget set.\n");
}

void list_budgets() {
    if (budgets.size == 0) { printf("No budgets.\n"); return; }
    for (size_t i = 0; i < budgets.size; ++i) {
        int idx = find_category_index_by_id(budgets.data[i].category_id);
        const char *cname = (idx >= 0) ? cats.data[idx].name : "UNKNOWN";
        printf("  %04d-%02d  %s  %.2f\n", budgets.data[i].year, budgets.data[i].month, cname, budgets.data[i].amount);
    }
}

double total_for_category_month(int cat_id, int year, int month) {
    char smin[DATE_STRLEN], smax[DATE_STRLEN];
    snprintf(smin, sizeof(smin), "%04d-%02d-01", year, month);
    /* compute last day approximately by moving to next month and subtracting 1 day, but for filtering lexicographically use next month start minus small */
    int nmonth = month == 12 ? 1 : month + 1;
    int nyear = month == 12 ? year + 1 : year;
    snprintf(smax, sizeof(smax), "%04d-%02d-01", nyear, nmonth);
    double total = 0.0;
    for (size_t i = 0; i < txns.size; ++i) {
        Transaction *t = &txns.data[i];
        if (t->category_id != cat_id) continue;
        if (compare_dates(t->date, smin) < 0) continue;
        if (compare_dates(t->date, smax) >= 0) continue;
        if (t->type == TYPE_EXPENSE) total += t->amount;
        else total -= t->amount; /* treat income as negative for category spending */
    }
    return total;
}

/* -------------------- Reports -------------------- */

void monthly_summary(int year, int month) {
    char smin[DATE_STRLEN], smax[DATE_STRLEN];
    snprintf(smin, sizeof(smin), "%04d-%02d-01", year, month);
    int nmonth = (month == 12) ? 1 : month+1;
    int nyear = (month == 12) ? year+1 : year;
    snprintf(smax, sizeof(smax), "%04d-%02d-01", nyear, nmonth);
    double income = 0.0, expense = 0.0;
    for (size_t i = 0; i < txns.size; ++i) {
        Transaction *t = &txns.data[i];
        if (compare_dates(t->date, smin) < 0) continue;
        if (compare_dates(t->date, smax) >= 0) continue;
        if (t->type == TYPE_INCOME) income += t->amount;
        else expense += t->amount;
    }
    printf("Monthly Summary for %04d-%02d:\n", year, month);
    printf("  Total Income:  %.2f\n", income);
    printf("  Total Expense: %.2f\n", expense);
    printf("  Net Savings:   %.2f\n", income - expense);
}

void category_summary(int year, int month) {
    printf("Category Summary %04d-%02d:\n", year, month);
    if (cats.size == 0) { printf(" (no categories)\n"); return; }
    for (size_t i = 0; i < cats.size; ++i) {
        double total = total_for_category_month(cats.data[i].id, year, month);
        printf("  %-20s : %.2f\n", cats.data[i].name, total);
    }
}

void budget_report(int year, int month) {
    printf("Budget Report %04d-%02d:\n", year, month);
    int found = 0;
    for (size_t i = 0; i < budgets.size; ++i) {
        if (budgets.data[i].year == year && budgets.data[i].month == month) {
            found = 1;
            int cid = budgets.data[i].category_id;
            int idx = find_category_index_by_id(cid);
            const char *name = (idx >= 0) ? cats.data[idx].name : "UNKNOWN";
            double used = total_for_category_month(cid, year, month);
            double bamt = budgets.data[i].amount;
            printf("  %-16s Budget: %.2f  Used: %.2f  Remaining: %.2f\n", name, bamt, used, bamt - used);
        }
    }
    if (!found) printf("  No budgets set for this month.\n");
}

/* -------------------- CSV import/export and search -------------------- */

void export_csv(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) { printf("Unable to open file for export.\n"); return; }
    fprintf(f, "id,date,type,amount,category,note\n");
    for (size_t i = 0; i < txns.size; ++i) {
        Transaction *t = &txns.data[i];
        int idx = find_category_index_by_id(t->category_id);
        const char *cname = (idx >= 0) ? cats.data[idx].name : "UNKNOWN";
        fprintf(f, "%d,%s,%d,%.2f,%s,%s\n", t->id, t->date, (int)t->type, t->amount, cname, t->note);
    }
    fclose(f);
    printf("Exported to %s\n", path);
}

/* Basic CSV import: expects header date,type,amount,category,note or id included */
void import_csv(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) { printf("Open failed.\n"); return; }
    char line[1024];
    int lineno = 0;
    while (fgets(line, sizeof(line), f)) {
        lineno++;
        if (lineno == 1) continue; /* skip header */
        char *p = line;
        /* parse by comma (simple) */
        char *token;
        char date[DATE_STRLEN] = {0};
        int type = 0;
        double amount = 0;
        char category[64] = {0};
        char note[MAX_NOTE] = {0};
        /* date */
        token = strtok(p, ",");
        if (!token) continue;
        strncpy(date, token, DATE_STRLEN-1);
        /* type */
        token = strtok(NULL, ",");
        if (!token) continue;
        type = atoi(token);
        token = strtok(NULL, ",");
        if (!token) continue;
        amount = atof(token);
        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(category, token, sizeof(category)-1);
        token = strtok(NULL, "\n");
        if (token) strncpy(note, token, sizeof(note)-1);
        /* trim whitespace/newline */
        for (char *q=date; *q; ++q) if (*q=='\r' || *q=='\n') *q=0;
        if (!parse_date(date, NULL)) {
            printf("Skipping invalid date on line %d\n", lineno); continue;
        }
        /* find or create category */
        int cid = -1;
        for (size_t i = 0; i < cats.size; ++i) {
            if (strcasecmp(cats.data[i].name, category) == 0) { cid = cats.data[i].id; break; }
        }
        if (cid < 0) {
            ensure_cat_capacity();
            Category c;
            c.id = cats.next_id++;
            strncpy(c.name, category, sizeof(c.name)-1);
            cats.data[cats.size++] = c;
            cid = c.id;
            printf("Created category '%s' id=%d\n", c.name, c.id);
        }
        ensure_txn_capacity();
        Transaction t;
        t.id = txns.next_id++;
        strncpy(t.date, date, DATE_STRLEN-1);
        t.type = (type==1)?TYPE_INCOME:TYPE_EXPENSE;
        t.amount = amount;
        t.category_id = cid;
        strncpy(t.note, note, sizeof(t.note)-1);
        txns.data[txns.size++] = t;
    }
    fclose(f);
    printf("Import complete.\n");
}

/* -------------------- Search -------------------- */
void search_transactions() {
    printf("Search: leave fields blank to ignore.\n");
    printf("Start date (YYYY-MM-DD): ");
    char sdate[DATE_STRLEN]; read_line(sdate, sizeof(sdate));
    if (strlen(sdate) && !parse_date(sdate, NULL)) { printf("Invalid date.\n"); return; }
    printf("End date (YYYY-MM-DD): ");
    char edate[DATE_STRLEN]; read_line(edate, sizeof(edate));
    if (strlen(edate) && !parse_date(edate, NULL)) { printf("Invalid date.\n"); return; }
    printf("Category name (partial): "); char cname[64]; read_line(cname, sizeof(cname));
    printf("Min amount (0 to ignore): "); double minamt = read_double();
    printf("Max amount (0 to ignore): "); double maxamt = read_double();
    printf("Text in note (partial): "); char text[64]; read_line(text, sizeof(text));
    printf("Search results:\n");
    for (size_t i = 0; i < txns.size; ++i) {
        Transaction *t = &txns.data[i];
        if (strlen(sdate) && compare_dates(t->date, sdate) < 0) continue;
        if (strlen(edate) && compare_dates(t->date, edate) > 0) continue;
        if (minamt > 0 && t->amount < minamt) continue;
        if (maxamt > 0 && t->amount > maxamt) continue;
        int idx = find_category_index_by_id(t->category_id);
        const char *catn = (idx>=0)?cats.data[idx].name:"UNKNOWN";
        if (strlen(cname) && strcasestr(catn, cname) == NULL) continue;
        if (strlen(text) && strcasestr(t->note, text) == NULL) continue;
        printf("  id=%d %s %s %.2f [%s] %s\n", t->id, t->date, (t->type==TYPE_INCOME?"IN":"EX"), t->amount, catn, t->note);
    }
}

/* -------------------- Input helpers -------------------- */

void prompt_press_enter() {
    printf("Press Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int read_int() {
    char buf[64];
    read_line(buf, sizeof(buf));
    if (strlen(buf) == 0) return 0;
    return atoi(buf);
}

double read_double() {
    char buf[64];
    read_line(buf, sizeof(buf));
    if (strlen(buf) == 0) return 0.0;
    return atof(buf);
}

void read_line(char *buf, size_t sz) {
    if (!fgets(buf, (int)sz, stdin)) { buf[0] = 0; return; }
    size_t ln = strlen(buf);
    while (ln && (buf[ln-1] == '\n' || buf[ln-1] == '\r')) { buf[--ln] = 0; }
}

/* -------------------- Date helpers -------------------- */

int parse_date(const char *s, struct tm *out) {
    if (!s) return 0;
    if (strlen(s) != 10) return 0;
    int y,m,d;
    if (sscanf(s, "%4d-%2d-%2d", &y,&m,&d) != 3) return 0;
    if (m < 1 || m > 12 || d < 1 || d > 31) return 0;
    if (out) {
        memset(out,0,sizeof(struct tm));
        out->tm_year = y - 1900;
        out->tm_mon = m - 1;
        out->tm_mday = d;
    }
    return 1;
}

/* lexicographic compare works for YYYY-MM-DD */
int compare_dates(const char *a, const char *b) {
    return strcmp(a, b);
}

/* -------------------- Menu -------------------- */

void toggle_obfuscation() {
    if (!obfuscate_enabled) {
        printf("Enable simple XOR obfuscation? (y/n): ");
        char ans[8]; read_line(ans, sizeof(ans));
        if (ans[0]=='y' || ans[0]=='Y') {
            printf("Enter single-character key (not secure): ");
            char k[8]; read_line(k, sizeof(k));
            if (strlen(k)==0) { printf("No key — aborted.\n"); return; }
            obf_key = (unsigned char)k[0];
            obfuscate_enabled = 1;
            printf("Obfuscation enabled.\n");
        }
    } else {
        obfuscate_enabled = 0;
        obf_key = 0;
        printf("Obfuscation disabled.\n");
    }
}

void interactive_menu() {
    for (;;) {
        printf("\n=== Menu ===\n");
        printf("1) Add transaction\n");
        printf("2) List transactions\n");
        printf("3) Edit transaction\n");
        printf("4) Delete transaction\n");
        printf("5) Add category\n");
        printf("6) List/Edit/Delete categories\n");
        printf("7) Set/List budgets\n");
        printf("8) Reports (monthly/category/budget)\n");
        printf("9) Export CSV\n");
        printf("10) Import CSV\n");
        printf("11) Search transactions\n");
        printf("12) Toggle file obfuscation (current: %s)\n", obfuscate_enabled ? "ON" : "OFF");
        printf("0) Save & Exit\n");
        printf("Choice: ");
        int c = read_int();
        switch (c) {
            case 1: add_transaction(); break;
            case 2: {
                printf("List all or range? (a/r) ");
                char r[8]; read_line(r,sizeof(r));
                if (r[0]=='r' || r[0]=='R') {
                    printf("Start date: "); char s[DATE_STRLEN]; read_line(s,sizeof(s));
                    printf("End date: "); char e[DATE_STRLEN]; read_line(e,sizeof(e));
                    list_transactions((strlen(s)?s:NULL), (strlen(e)?e:NULL));
                } else list_transactions(NULL,NULL);
                break;
            }
            case 3: edit_transaction(); break;
            case 4: remove_transaction(); break;
            case 5: add_category(); break;
            case 6: {
                list_categories();
                printf("e=edit, d=delete, anything else to return: ");
                char a[8]; read_line(a,sizeof(a));
                if (a[0]=='e') edit_category();
                else if (a[0]=='d') remove_category();
                break;
            }
            case 7: {
                printf("1=set budget 2=list budgets : ");
                int b = read_int();
                if (b==1) set_budget(); else list_budgets();
                break;
            }
            case 8: {
                printf("Year: "); int y = read_int();
                printf("Month: "); int m = read_int();
                monthly_summary(y,m);
                category_summary(y,m);
                budget_report(y,m);
                break;
            }
            case 9: {
                printf("Export path (e.g., out.csv): ");
                char path[256]; read_line(path,sizeof(path));
                if (strlen(path)==0) strcpy(path,"export.csv");
                export_csv(path);
                break;
            }
            case 10: {
                printf("CSV path to import: ");
                char path[256]; read_line(path,sizeof(path));
                if (strlen(path) == 0) { printf("Aborted.\n"); break; }
                import_csv(path);
                break;
            }
            case 11: search_transactions(); break;
            case 12: toggle_obfuscation(); break;
            case 0:
                save_all();
                return;
            default: printf("Invalid.\n"); break;
        }
    }
}
