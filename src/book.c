#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "book.h"
#include "sort.h"
#include "utils.h"
#include "fileio.h"






int findBookById(const Library *library, const char *ocaid)
{
    int i;

    for (i = 0; i < library->bookCount; i++) {
        if (strcmp(library->books[i].ocaid, ocaid) == 0) {
            return i;
        }
    }
    return FAILURE;
}






void searchBooks(const Library *library)
{
    char keyword[128];
    char shortTitle[41];
    char shortAuthor[26];
    char answer[8];
    int i;
    int found = 0;

    if (readString("Kata kunci (ID/judul/penulis): ", keyword, (int)sizeof(keyword)) != SUCCESS) {
        return;
    }
    if (keyword[0] == '\0') {
        printf("Kata kunci tidak boleh kosong.\n");
        return;
    }

    for (i = 0; i < library->bookCount; i++) {
        if (!containsIgnoreCase(library->books[i].ocaid, keyword) &&
            !containsIgnoreCase(library->books[i].title, keyword) &&
            !containsIgnoreCase(library->books[i].author, keyword)) {
            continue;
        }
        if (found % PAGE_SIZE == 0) {
            printf("\n%-28s %-40s %-25s %-5s %s\n", "OCAID", "JUDUL", "PENULIS", "THN", "STOK");
        }
        truncateForDisplay(library->books[i].title, shortTitle, 40);
        truncateForDisplay(library->books[i].author, shortAuthor, 25);
        printf("%-28.28s %-40s %-25s %-5d %d\n",
               library->books[i].ocaid, shortTitle, shortAuthor,
               library->books[i].year, library->books[i].stock);
        found++;
        if (found % PAGE_SIZE == 0) {
            if (readOptional("Enter lanjut, q berhenti: ", answer, (int)sizeof(answer)) != SUCCESS ||
                answer[0] == 'q' || answer[0] == 'Q') {
                break;
            }
        }
    }
    if (found == 0) {
        printf("Tidak ada buku yang cocok.\n");
    } else {
        printf("\n%d buku ditampilkan.\n", found);
    }
}

/* Reads a non-empty text field. Rejects double quotes always, and
 * whitespace when allowSpaces is 0 (bare tokens in the data files). */
static int readField(const char *prompt, char *buffer, int bufferSize, int allowSpaces)
{
    for (;;) {
        if (readString(prompt, buffer, bufferSize) != SUCCESS) {
            return FAILURE;
        }
        if (buffer[0] == '\0') {
            printf("Tidak boleh kosong.\n");
        } else if (strchr(buffer, '"') != NULL) {
            printf("Tanda kutip (\") tidak boleh dipakai.\n");
        } else if (!allowSpaces && (strchr(buffer, ' ') != NULL || strchr(buffer, '\t') != NULL)) {
            printf("Tidak boleh mengandung spasi.\n");
        } else {
            return SUCCESS;
        }
    }
}

int addBook(Library *library)
{
    Book newBook;
    Book *resizedArray;
    int newCapacity;

    memset(&newBook, 0, sizeof(newBook));

    if (readField("OCAID (tanpa spasi): ", newBook.ocaid, MAX_OCAID, 0) != SUCCESS) return FAILURE;
    if (findBookById(library, newBook.ocaid) != FAILURE) {
        printf("OCAID sudah ada. Gunakan Edit buku.\n");
        return FAILURE;
    }
    if (readField("Judul: ", newBook.title, MAX_TITLE, 1) != SUCCESS) return FAILURE;
    if (readField("Penulis: ", newBook.author, MAX_AUTHOR, 1) != SUCCESS) return FAILURE;
    if (readInt("Tahun (1-2100): ", 1, 2100, &newBook.year) != SUCCESS) return FAILURE;
    if (readInt("Jumlah halaman (1-100000): ", 1, 100000, &newBook.pageCount) != SUCCESS) return FAILURE;
    if (readField("Edisi (tanpa spasi): ", newBook.edition, MAX_EDITION, 0) != SUCCESS) return FAILURE;
    if (readField("Work (tanpa spasi): ", newBook.work, MAX_WORK, 0) != SUCCESS) return FAILURE;
    if (readField("URL (tanpa spasi): ", newBook.url, MAX_URL, 0) != SUCCESS) return FAILURE;
    if (readInt("Stok (0-1000): ", 0, 1000, &newBook.stock) != SUCCESS) return FAILURE;

    if (library->bookCount >= library->bookCapacity) {
        newCapacity = (library->bookCapacity == 0) ? 64 : library->bookCapacity * 2;
        resizedArray = (Book *)realloc(library->books, (size_t)newCapacity * sizeof(Book));
        if (resizedArray == NULL) {
            printf("Memori tidak cukup.\n");
            return FAILURE;
        }
        library->books = resizedArray;
        library->bookCapacity = newCapacity;
    }
    library->books[library->bookCount] = newBook;
    library->bookCount++;

    if (saveBooks(library) != SUCCESS) {
        printf("Peringatan: gagal menyimpan ke %s.\n", INVENTORY_PATH);
        return FAILURE;
    }
    printf("Buku '%s' berhasil ditambahkan.\n", newBook.ocaid);
    return SUCCESS;
}

/* Prompts "label [current]: ". Empty input keeps the current text. */
static int editTextField(const char *label, char *target, int bufferSize, int allowSpaces)
{
    char prompt[160];
    char input[MAX_TITLE];

    for (;;) {
        sprintf(prompt, "%s (Enter = tetap): ", label);
        if (readOptional(prompt, input, bufferSize) != SUCCESS) return FAILURE;
        if (input[0] == '\0') return SUCCESS;
        if (strchr(input, '"') != NULL) {
            printf("Tanda kutip (\") tidak boleh dipakai.\n");
        } else if (!allowSpaces && (strchr(input, ' ') != NULL || strchr(input, '\t') != NULL)) {
            printf("Tidak boleh mengandung spasi.\n");
        } else {
            strcpy(target, input);
            return SUCCESS;
        }
    }
}

/* Prompts for a number; empty input keeps the current value. */
static int editIntField(const char *label, int minimum, int maximum, int *target)
{
    char line[32];
    char prompt[96];
    int value;

    for (;;) {
        sprintf(prompt, "%s [%d] (Enter = tetap): ", label, *target);
        if (readOptional(prompt, line, (int)sizeof(line)) != SUCCESS) return FAILURE;
        if (line[0] == '\0') return SUCCESS;
        value = atoi(line);
        if (value < minimum || value > maximum ||
            (value == 0 && strcmp(line, "0") != 0)) {
            printf("Masukkan angka %d-%d.\n", minimum, maximum);
        } else {
            *target = value;
            return SUCCESS;
        }
    }
}

int editBook(Library *library)
{
    char ocaid[MAX_OCAID];
    Book draft;
    int index;

    if (readString("OCAID buku yang diedit: ", ocaid, MAX_OCAID) != SUCCESS) return FAILURE;
    index = findBookById(library, ocaid);
    if (index == FAILURE) {
        printf("Buku tidak ditemukan.\n");
        return FAILURE;
    }

    draft = library->books[index];
    printf("\nJudul   : %s\nPenulis : %s\n", draft.title, draft.author);
    if (editTextField("Judul", draft.title, MAX_TITLE, 1) != SUCCESS) return FAILURE;
    printf("Penulis : %s\n", draft.author);
    if (editTextField("Penulis", draft.author, MAX_AUTHOR, 1) != SUCCESS) return FAILURE;
    if (editIntField("Tahun", 1, 2100, &draft.year) != SUCCESS) return FAILURE;
    if (editIntField("Halaman", 1, 100000, &draft.pageCount) != SUCCESS) return FAILURE;
    if (editIntField("Stok", 0, 1000, &draft.stock) != SUCCESS) return FAILURE;

    library->books[index] = draft;
    if (saveBooks(library) != SUCCESS) {
        printf("Peringatan: gagal menyimpan ke %s.\n", INVENTORY_PATH);
        return FAILURE;
    }
    printf("Buku '%s' diperbarui.\n", draft.ocaid);
    return SUCCESS;
}

int deleteBook(Library *library)
{
    char ocaid[MAX_OCAID];
    char answer[8];
    int index;
    int i;

    if (readString("OCAID buku yang dihapus: ", ocaid, MAX_OCAID) != SUCCESS) return FAILURE;
    index = findBookById(library, ocaid);
    if (index == FAILURE) {
        printf("Buku tidak ditemukan.\n");
        return FAILURE;
    }

    for (i = 0; i < library->loanCount; i++) {
        if (strcmp(library->loans[i].ocaid, ocaid) == 0 &&
            strcmp(library->loans[i].status, "DIPINJAM") == 0) {
            printf("Ditolak: buku masih dipinjam oleh %s.\n", library->loans[i].username);
            return FAILURE;
        }
    }

    printf("Hapus '%s'? (y/n): ", library->books[index].title);
    if (readLine(answer, (int)sizeof(answer)) != SUCCESS ||
        (answer[0] != 'y' && answer[0] != 'Y')) {
        printf("Dibatalkan.\n");
        return FAILURE;
    }

    memmove(&library->books[index], &library->books[index + 1],
            (size_t)(library->bookCount - index - 1) * sizeof(Book));
    library->bookCount--;

    if (saveBooks(library) != SUCCESS) {
        printf("Peringatan: gagal menyimpan ke %s.\n", INVENTORY_PATH);
        return FAILURE;
    }
    printf("Buku '%s' dihapus.\n", ocaid);
    return SUCCESS;
}

void showSortedCatalog(const Library *library, int availableOnly)
{
    int *order;
    int count = 0;
    int i;
    int algorithm;
    int fieldChoice;
    int direction;
    SortField field;
    clock_t startTime;
    double seconds;
    char shortTitle[41];
    char shortAuthor[26];
    char answer[8];

    if (library->bookCount == 0) {
        printf("Katalog kosong.\n");
        return;
    }

    printf("\n=== URUTKAN KATALOG ===\n");
    printf("Algoritma: 1. Bubble  2. Selection  3. Insertion\n");
    if (readInt("Pilih algoritma (1-3): ", 1, 3, &algorithm) != SUCCESS) return;
    printf("Urut berdasarkan: 1. ID  2. Judul  3. Tahun  4. Halaman  5. Stok\n");
    if (readInt("Pilih field (1-5): ", 1, 5, &fieldChoice) != SUCCESS) return;
    printf("Arah: 1. Naik (A-Z, kecil-besar)  2. Turun\n");
    if (readInt("Pilih arah (1-2): ", 1, 2, &direction) != SUCCESS) return;
    field = (SortField)(fieldChoice - 1);

    order = (int *)malloc((size_t)library->bookCount * sizeof(int));
    if (order == NULL) {
        printf("Memori tidak cukup.\n");
        return;
    }
    for (i = 0; i < library->bookCount; i++) {
        if (!availableOnly || library->books[i].stock > 0) {
            order[count++] = i;
        }
    }
    if (count == 0) {
        printf("Tidak ada buku yang tersedia.\n");
        free(order);
        return;
    }

    startTime = clock();
    if (algorithm == 1) {
        bubbleSort(library, order, count, field, direction == 2);
    } else if (algorithm == 2) {
        selectionSort(library, order, count, field, direction == 2);
    } else {
        insertionSort(library, order, count, field, direction == 2);
    }
    seconds = (double)(clock() - startTime) / (double)CLOCKS_PER_SEC;
    printf("\n%d buku diurutkan dalam %.3f detik.\n", count, seconds);

    for (i = 0; i < count; i++) {
        if (i % PAGE_SIZE == 0) {
            printf("\n%-28s %-40s %-25s %-5s %s\n", "OCAID", "JUDUL", "PENULIS", "THN", "STOK");
        }
        truncateForDisplay(library->books[order[i]].title, shortTitle, 40);
        truncateForDisplay(library->books[order[i]].author, shortAuthor, 25);
        printf("%-28.28s %-40s %-25s %-5d %d\n",
               library->books[order[i]].ocaid, shortTitle, shortAuthor,
               library->books[order[i]].year, library->books[order[i]].stock);
        if ((i + 1) % PAGE_SIZE == 0 && i + 1 < count) {
            printf("-- halaman %d dari %d --\n", (i + 1) / PAGE_SIZE,
                   (count + PAGE_SIZE - 1) / PAGE_SIZE);
            if (readOptional("Enter lanjut, q berhenti: ", answer, (int)sizeof(answer)) != SUCCESS ||
                answer[0] == 'q' || answer[0] == 'Q') {
                break;
            }
        }
    }
    free(order);
}





/* Like readString, but empty input is allowed (used for "Enter = keep"). */
