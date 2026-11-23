#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"

/*
 * readLine: read one line from stdin using fgets, strip the trailing
 * newline, and if the line did not fit in the buffer, flush the rest
 * so the next call starts clean. Returns SUCCESS or FAILURE (EOF/error).
 */
int readLine(char *buffer, int bufferSize)
{
    int length;
    int ch;

    if (buffer == NULL || bufferSize < 2) {
        return FAILURE;
    }
    if (fgets(buffer, bufferSize, stdin) == NULL) {
        return FAILURE;
    }
    length = (int)strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        /* line did not fit: drain the rest */
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* discard */
        }
    }
    return SUCCESS;
}

/*
 * readInt: print the prompt, read a line, parse it as an integer with
 * strtol, and loop until the value lies in [minimum, maximum].
 * No raw scanf is used anywhere. Returns SUCCESS on success.
 */
int readInt(const char *prompt, int minimum, int maximum, int *result)
{
    char buffer[64];
    char *end;
    long value;

    if (result == NULL) {
        return FAILURE;
    }
    for (;;) {
        printf("%s", prompt);
        if (readLine(buffer, (int)sizeof(buffer)) != SUCCESS) {
            return FAILURE;
        }
        value = strtol(buffer, &end, 10);
        if (end == buffer || *end != '\0') {
            printf("Input tidak valid. Masukkan angka.\n");
            continue;
        }
        if (value < (long)minimum || value > (long)maximum) {
            printf("Harus antara %d dan %d.\n", minimum, maximum);
            continue;
        }
        *result = (int)value;
        return SUCCESS;
    }
}

/*
 * readString: print the prompt and read a line, rejecting empty input.
 * Loops until the user types something non-empty. The buffer is capped
 * at bufferSize - 1 characters by readLine itself.
 */
int readString(const char *prompt, char *buffer, int bufferSize)
{
    if (buffer == NULL || bufferSize < 2) {
        return FAILURE;
    }
    for (;;) {
        printf("%s", prompt);
        if (readLine(buffer, bufferSize) != SUCCESS) {
            return FAILURE;
        }
        if (buffer[0] != '\0') {
            return SUCCESS;
        }
        printf("Input tidak boleh kosong.\n");
    }
}

/*
 * compareIgnoreCase: same sign convention as strcmp (<0, 0, >0) but
 * ASCII case-insensitive. strcasecmp is POSIX and banned by -ansi,
 * so we lowercase one character at a time with tolower.
 */
int compareIgnoreCase(const char *first, const char *second)
{
    int left;
    int right;

    if (first == NULL || second == NULL) {
        return 0;
    }
    while (*first != '\0' && *second != '\0') {
        left  = tolower((unsigned char)*first);
        right = tolower((unsigned char)*second);
        if (left != right) {
            return left - right;
        }
        first++;
        second++;
    }
    return tolower((unsigned char)*first) - tolower((unsigned char)*second);
}

/*
 * containsIgnoreCase: returns nonzero if pattern appears anywhere in
 * text (case-insensitive). Returns 0 otherwise. Used by searchBooks.
 */
int containsIgnoreCase(const char *text, const char *pattern)
{
    int textLength;
    int patternLength;
    int i;
    int j;
    int match;

    if (text == NULL || pattern == NULL) {
        return 0;
    }
    textLength    = (int)strlen(text);
    patternLength = (int)strlen(pattern);
    if (patternLength == 0) {
        return 1;
    }
    if (patternLength > textLength) {
        return 0;
    }
    for (i = 0; i <= textLength - patternLength; i++) {
        match = 1;
        for (j = 0; j < patternLength; j++) {
            if (tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)pattern[j])) {
                match = 0;
                break;
            }
        }
        if (match) {
            return 1;
        }
    }
    return 0;
}

/*
 * getCurrentDate: fill destination with today's date in DD-MM-YYYY.
 * strftime is C89; snprintf is not, so we use strftime directly.
 * destinationSize must be at least MAX_DATE (20) bytes.
 */
void getCurrentDate(char *destination, int destinationSize)
{
    time_t now;
    struct tm *local;
    int written;

    if (destination == NULL || destinationSize < 11) {
        if (destination != NULL && destinationSize > 0) {
            destination[0] = '\0';
        }
        return;
    }
    now = time(NULL);
    if (now == (time_t)-1) {
        destination[0] = '\0';
        return;
    }
    local = localtime(&now);
    if (local == NULL) {
        destination[0] = '\0';
        return;
    }
    written = (int)strftime(destination, (size_t)destinationSize, "%d-%m-%Y", local);
    if (written == 0) {
        destination[0] = '\0';
    }
}

/*
 * truncateForDisplay: copy source into destination but keep at most
 * maxLength characters. If the source is longer, the last three
 * characters of destination become "..." so the user knows it was cut.
 * Display only: the full string stays in the Book struct and on disk.
 */
void truncateForDisplay(const char *source, char *destination, int maxLength)
{
    int length;
    int copy;

    if (destination == NULL || maxLength < 1) {
        return;
    }
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    length = (int)strlen(source);
    if (length <= maxLength) {
        strcpy(destination, source);
        return;
    }
    if (maxLength <= 3) {
        copy = maxLength;
        memcpy(destination, source, (size_t)copy);
        destination[copy] = '\0';
        return;
    }
    copy = maxLength - 3;
    memcpy(destination, source, (size_t)copy);
    destination[copy]     = '.';
    destination[copy + 1] = '.';
    destination[copy + 2] = '.';
    destination[copy + 3] = '\0';
}

int readOptional(const char *prompt, char *buffer, int bufferSize)
{
    int length;

    printf("%s", prompt);
    fflush(stdout);
    if (readLine(buffer, bufferSize) != SUCCESS) return FAILURE;
    length = (int)strlen(buffer);
    while (length > 0 && (buffer[length - 1] == '\n' || buffer[length - 1] == '\r')) {
        buffer[--length] = '\0';
    }
    return SUCCESS;
}
