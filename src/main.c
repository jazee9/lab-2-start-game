// === ИМПОРТ ===⁡ //

// Библиотеки
#include <time.h> // Для рандомных чисел
#include <stdio.h> // Для ввода и вывода 

// Подключение файлов
#include "utils.h"
#include "player.h"
#include "map.h"

// ⁡⁢⁣========== КОНСТАНТЫ ============⁡ //

#define HOURS_IN_DAY 24 // Кол-во часов в сутках

// Для имени фермера
#define MAX_FARMER_NAME 32 // Имя фермера: 31 символ + '\0'

// ============ ФУНКЦИЯ ДЛЯ ИМЕНИ ФЕРМЕРА ============ //

// Спрашиваем имя для фермера, пока не введут
void readFarmerName(char *name)
{
    do
    {
        printf("Как зовут фермера? ");
        readLine(name, MAX_FARMER_NAME);
        if (strlen(name) == 0)
            printf("Имя не может быть пустым!\n");
    } while (strlen(name) == 0 && !feof(stdin));
}


// ========= ПРОТОТИПЫ ======== //
void printMenu(const char *farmer);
void showTime(int day, int hour);
void workTime(int *day, int *hour);
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void deleteItem(int inventory[]);
void clearTrash(int inventory[]);
void findItem(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void write_diary(const char *farmer, int day, int hour, const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);


// ====================== MAIN ====================⁡ //

int main(void) 
{
    // Что бы rand() при каждом запуске программы не выдавал одну и ту же последовательность 
    srand((unsigned)time(NULL)); 
    // time(NULL) - Выдает текущее время #include <time.h>
    // unsigned - Приводит в целое
    // srand() - Задаёт начальное значение для генератора псевдослучайных чисел

    // Инициализация основных переменных⁡ 
    int current_day = 1;
    int current_hour = 8;
    char item_names[MAX_ITEMS][MAX_NAME_LEN]; // Создаем таблицу для названий предметов 10/32 - 1 ('\0')
    int inventory[SIZE] = {2, 3, 4, 4, 0, 0, 0, 0, 0, 0}; // Изначальный инвентарь 
    int choice; 

    char farmer[MAX_FARMER_NAME]; // Для имени фермера
    readFarmerName(farmer); // Запрашиваем имя фермера

    loadItemNames(item_names, "items.txt"); // Читаем названия из items.txt

    do  
    {
        printMenu(farmer); // Запуск меню
        choice = readInt(); // Выбор варианта пользователем

        switch (choice) 
        {

        // ⁡⁢⁣Если введена неверная цифра для выбора пункта меню
        default: printf("Нет такого пункта меню.\n"); break;

        // ⁡⁢⁣=== CASES ===⁡ //
        case 0: printf("Выход...\n"); break;                  // ВЫХОД 
        case 1: showTime(current_day, current_hour); break;   // ПОСМОТРЕТЬ ВРЕМЯ 
        case 2: workTime(&current_day, &current_hour); break; // ПРОМАТАТЬ ВРЕМЯ (ПОРАБОТАТЬ)
        case 3: showInventory(inventory, item_names); break;  // ПОСМОТРЕТЬ ИНВЕНТАРЬ        
        case 4: putItem(inventory, item_names); break;        // ПОЛОЖИТЬ ПРЕДМЕТ В СЛОТ
        case 5: deleteItem(inventory); break;                 // ВЫБРОСИТЬ ПРЕДМЕТ
        case 6: clearTrash(inventory); break;                 // ОЧИСТКА ОТ МУСОРА (5 ВАРИАНТ)
        case 7: findItem(inventory, item_names); break;       // ПОИСК ПРЕДМЕТА В РЮКЗАКЕ ПО НАЗВАНИЮ
        case 8: write_diary(farmer, current_day, current_hour, inventory, item_names); break; // ЗАПИСЬ В ДНЕВНИК 
        case 9: analyzeLog(); break; 
        case 10: forestRingAdventure(); break;                // В ПРИКЛЮЧЕНИЕ (5-ЫЙ ВАРИАНТ)
        }
    } while (choice != 0);
    return 0;
}

// ⁡⁢⁣====== МЕНЮ ========⁡ //

void printMenu(const char *farmer) 
{
    printf("\n====== МЕНЮ | Фермер: %s ======", farmer);
    printf("\n1.  - Посмотреть на часы\n");
    printf("2.  - Промотать время (Поработать)\n");
    printf("3.  - Посмотреть инвентарь\n");
    printf("4.  - Положить предмет в слот\n");
    printf("5.  - Выбросить предмет\n");
    printf("6.  - Очистить мусор\n"); // Мой 5-ый вариант 
    printf("7.  - Найти предмет в инвентаре\n"); // От сюда лаба 3
    printf("8.  - Записать состояние в дневник фермера\n");
    printf("9.  - Анализ лога\n");
    printf("10. - Отправится в приключение (Лес-кольцо)\n");  // От сюда лаба 4 (5-ый вариант)
    printf("0.  - Выход\n");
    printf("Выбор: ");
}

// ================ ФУНКЦИИ ================ //

// [case: 1] ПОСМОТРЕТЬ ВРЕМЯ
void showTime(int day,int hour) {printf("Текущее время: День %d, время: %d:00\n", day, hour);}

// [case: 2] ПРОМАТАТЬ ВРЕМЯ (ПОРАБОТАТЬ)
void workTime(int *day, int *hour)
{ 
    printf("Сколько часов поработать?: "); int h = readInt(); // Спрашиваем у пользователя
    if (h < 0) {printf("Кол-во часов не могут быть отрицательными!\n"); return;} // Неверный ввод

    *hour += h; // Прибавляем часы, введенные пользователем
    *day += *hour / HOURS_IN_DAY; // Считаем день
    *hour %= HOURS_IN_DAY; // Сколько часов по итогу

    showTime(*day, *hour); // Выводим время после перемотки
}

// [case: 8] ЗАПИСАТЬ СОСТОЯНИЕ В ДНЕВНИК ФЕРМЕРА (diary.txt)
void write_diary(const char *farmer, int day, int hour, const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) 
{
    FILE *f = fopen("diary.txt", "a");
    // Если не откроется
    if (f == NULL) {printf("Ошибка! Не удалось открыть файл diary.txt для записи.\n"); return;}

    fprintf(f, "==== Фермер: %s ====\n", farmer);
    fprintf(f, "Время: День %d, %d:00\n", day, hour);
    fprintf(f, "Инвентарь:\n");

    // Выводим инвентарь
    for (int num = 0; num < SIZE; num++) // Перебор слотов
    {
        int id = inventory[num];
        if (id >= 0 && id < MAX_ITEMS) 
            fprintf(f, "  Слот %d: %s\n", num, item_names[id]);
    }

    fprintf(f, "\n"); // Пустая строка между записями
    fclose(f);
    printf("Запись добавлена\n");
}