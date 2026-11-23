#ifndef TYPES_H
#define TYPES_H

/* Field sizes from the PDF spec and the Books.txt dataset stats:
   longest title 1121, author 460, line 1298.
   Text fields sized generously to avoid truncation on parse. */
#define MAX_OCAID     64
#define MAX_TITLE     2048
#define MAX_AUTHOR    1024
#define MAX_EDITION   128
#define MAX_WORK      128
#define MAX_URL       512
#define MAX_USERNAME  32
#define MAX_PASSWORD  32
#define MAX_ROLE      12
#define MAX_DATE      20
#define MAX_STATUS    20

#define DEFAULT_STOCK   3
#define PAGE_SIZE       20
#define LOGIN_ATTEMPTS  3

#define BOOKS_MASTER_PATH "data/Books.txt"
#define INVENTORY_PATH    "data/inventory.txt"
#define ACCOUNTS_PATH     "data/user.txt"
#define LOANS_PATH        "data/peminjaman.txt"

#define SUCCESS  0
#define FAILURE  (-1)

typedef struct {
    char ocaid[MAX_OCAID];
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int  year;
    int  pageCount;
    char edition[MAX_EDITION];
    char work[MAX_WORK];
    char url[MAX_URL];
    int  stock;
} Book;

typedef struct {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    char role[MAX_ROLE];
} Account;

typedef struct {
    unsigned int loanId;
    char username[MAX_USERNAME];
    char ocaid[MAX_OCAID];
    char loanDate[MAX_DATE];
    char status[MAX_STATUS];
} Loan;

typedef struct {
    Book    *books;
    int      bookCount;
    int      bookCapacity;
    Account *accounts;
    int      accountCount;
    int      accountCapacity;
    Loan    *loans;
    int      loanCount;
    int      loanCapacity;
    unsigned int nextLoanId;
} Library;

#endif
