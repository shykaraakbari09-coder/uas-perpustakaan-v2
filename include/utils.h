#ifndef UTILS_H
#define UTILS_H

#include "types.h"

int  readLine(char *buffer, int bufferSize);
int  readInt(const char *prompt, int minimum, int maximum, int *result);
int  readString(const char *prompt, char *buffer, int bufferSize);
int  readOptional(const char *prompt, char *buffer, int bufferSize);
int  compareIgnoreCase(const char *first, const char *second);
int  containsIgnoreCase(const char *text, const char *pattern);
void getCurrentDate(char *destination, int destinationSize);
void truncateForDisplay(const char *source, char *destination, int maxLength);

#endif
