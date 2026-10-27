#ifndef UTILS_H
#define UTILS_H

// ==== Импорт библиотек ==== //
#include <stdio.h> // Для ввода и вывода 
#include <string.h> // Для работы со строками (strlen, strcspn, strncpy)
#include <stdlib.h> // Для работы с динамич. памятью и случайных чисел

// ==== Константы ==== //
#define LINE_BUF_LEN 128 // Буфер для одной строки файла
#define MAX_ITEMS    10  // Сколько всего видов предметов (ID)
#define MAX_NAME_LEN 32  // Название предмета: 31 символ + '\0'

// ==== Прототипы функций ==== //
int readInt(void);
void readLine(char *buf, size_t size);
int randInRange(int lo, int hi);
int loadItemNames(char item_names[10][32], const char *filename);
void analyzeLog(void);
extern const char *names[10];

#endif