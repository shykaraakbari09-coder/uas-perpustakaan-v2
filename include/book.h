#ifndef BOOK_H
#define BOOK_H

#include "types.h"

int  findBookById(const Library *library, const char *ocaid);
int  addBook(Library *library);
int  editBook(Library *library);
int  deleteBook(Library *library);
void searchBooks(const Library *library);
void showSortedCatalog(const Library *library, int availableOnly);

#endif
