// === ИМПОРТ ===⁡ //
#include <stdio.h> // Для ввода и вывода 
#include <string.h> 

// ⁡⁢⁣=== КОНСТАНТЫ ===⁡ //
#define HOURS_IN_DAY 24 // Константа для кол-ва часов в сутках

// Для инвентаря
#define SIZE         10 // Кол-во слотов в инвентаре
#define MAX_ITEMS    10 // Сколько всего видов предметов (ID)
#define MAX_NAME_LEN 32 // Название предмета: 31 символ + '\0'


// Названия предметов по ID (индекс = ID) 
const char *names[10] = {"Пусто", "Дерево", "Камень", "Семена", "Железная руда", 
    "Золотая руда", "Алмазная руда", 
    "Вода", "Ягоды", "Веревка"};

/* Не дает программе упасть если ввели не число (единственная строчка, которую я не смогу объяснить) */
int readInt() {
    int x; 
    while (scanf("%d", &x) != 1) {printf("Ошибка! Введите число: "); 
        while (getchar() != '\n');} // точнее вот это
        return x;
}

// === ПРОТОТИПЫ (нейронка сказала сделать, без этого не работало) === //
void printMenu(void);
void showTime(int day, int hour);
void workTime(int *day, int *hour);
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void deleteItem(int inventory[]);
void clearTrash(int inventory[]);



// =========== MAIN =========⁡ //

int main(void) 
{
    // Инициализация основных переменных⁡ 
    int current_day = 1;
    int current_hour = 8;
    char item_names[MAX_ITEMS][MAX_NAME_LEN]; // Таблица названий предметов 10/32 - 1 ('\0')
    int inventory[SIZE] = {2, 3, 4, 4, 0, 0, 0, 0, 0, 0}; // Изначальный инвентарь 
    int choice; 

    do  
    {
        printMenu(); // Запуск меню
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
        }
    } while (choice != 0);

    return 0;
}

// ⁡⁢⁣====== МЕНЮ ========⁡ //
void printMenu(void) 
{
    printf("\n====== МЕНЮ ======");
    printf("\n1. - Посмотреть на часы\n");
    printf("2. - Промотать время (Поработать)\n");
    printf("3. - Посмотреть инвентарь\n");
    printf("4. - Положить предмет в слот\n");
    printf("5. - Выбросить предмет\n");
    printf("6. - Очистить мусор\n"); // Мой 5-ый вариант 
    printf("0. - Выход\n");
    printf("Выбор: ");
}
    


// ⁡⁢⁣======== ФУНКЦИИ ДЛЯ CASES ========⁡ //

// ================ Лаба 1 ================ //

// [1] ПОСМОТРЕТЬ ВРЕМЯ
void showTime(int day,int hour) {printf("Текущее время: День %d, время: %d:00\n", day, hour);}

// [2] ПРОМАТАТЬ ВРЕМЯ (ПОРАБОТАТЬ)
void workTime(int *day, int *hour)
{ 
    printf("Сколько часов поработать?: "); int h = readInt(); // Спрашиваем у пользователя
    if (h < 0) {printf("Кол-во часов не могут быть отрицательными!\n"); return;} // Неверный ввод

    *hour += h; // Прибавляем часы, введенные пользователем
    *day += *hour / HOURS_IN_DAY; // Считаем день
    *hour %= HOURS_IN_DAY; // Сколько часов по итогу

    showTime(*day, *hour); // Выводим время после перемотки
}

// [3] ПОСМОТРЕТЬ ИНВЕНТАРЬ
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) 
{
    for (int num = 0; num < SIZE; num++) 
        {if (inventory[num] >= 0 && inventory[num] <= 9) // Перебор слотов
        {printf("Слот %d: [%d] (%s)\n", num, inventory[num], names[inventory[num]]);}} return;
}

// [4] ПОЛОЖИТЬ ПРЕДМЕТ В ИНВЕНТАРЬ
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    printf("Введите индекс слота (0-%d): ", SIZE - 1); // Спрашиваем в какой слот ложить
    int index = readInt(); // Записываем

    if (index < 0 || index >= SIZE) // Неверный ввод
    {printf("Ошибка! Нет такого слота\n"); return;} 

    printf("Введите ID предмета (0-%d): ", MAX_ITEMS - 1); // Спрашиваем ID
    int id = readInt(); // Записываем

    if (id < 0 || id >= MAX_ITEMS) // Неверный ввод
    {printf("Введен неправильный ID!\n"); return;} 

    inventory[index] = id; // Кладем в слот
    printf("Предмет: %s - положен в слот %d\n", item_names[id], index); // Выводим
}

// [5] ВЫБРОСИТЬ ПРЕДМЕТ
void deleteItem(int inventory[])
{
    printf("Введите индекс слота (0-%d): ", SIZE - 1); // Спрашиваем в какой слот ложить
    int index = readInt(); // Записываем

    if (index < 0 || index >= SIZE) // Неверный ввод
    {printf("Ошибка! Нет такого слота\n"); return;} 

    inventory[index] = 0; // Чистим слот
    printf("Слот %d - очищен!", inventory[index]); // Выводим
}

// [6-МОЙ 5-ЫЙ ВАРИАНТ] ОТЧИСТКА ОТ МУСОРА
void clearTrash(int inventory[])
{
    printf("Введите ID предмета, который хотите удалить (0-%d): ", MAX_ITEMS - 1);
    int id = readInt(); // Запрашиваем ID

    if (id < 0 || id >= MAX_ITEMS) // Неверный ввод
    {printf("Введен неправильный ID!\n"); return;} 

    int count = 0; // Для подсчета кол-ва слотов
    for (int num = 0; num < SIZE; num++) // Перебор слотов
        if (inventory[num] == id) {inventory[num] = 0; count++;} // Чистим, если есть

    printf("Очищено слотов 666666: %d\n", count); // Выводиим
}

// ================ Лаба 3 ================ //



