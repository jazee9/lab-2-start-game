// === ИМПОРТ ===⁡ //
#include <stdio.h> // Для ввода и вывода 
#include <string.h> // Для работы со строками (strlen, strcspn, strncpy)

// ⁡⁢⁣========== КОНСТАНТЫ ============⁡ //

#define HOURS_IN_DAY 24 // Константа для кол-ва часов в сутках

// Для инвентаря
#define SIZE         10  // Кол-во слотов в инвентаре
#define MAX_ITEMS    10  // Сколько всего видов предметов (ID)
#define MAX_NAME_LEN 32  // Название предмета: 31 символ + '\0'
#define LINE_BUF_LEN 128 // Буфер для одной строки файла

// Для имени фермера
#define MAX_FARMER_NAME 32 // Имя фермера: 31 символ + '\0'

// Стандартные названия предметов по ID (страховка, если будут проблемы с items.txt)
const char *names[10] = {"Пусто", "Дерево", "Камень", "Семена пшеницы", "Железная руда", 
    "Золотая руда", "Алмазная руда", 
    "Вода", "Ягоды", "Веревка"};

// ⁡⁢⁣========= ДЛЯ НЕКОРЕКТНЫХ ВВОДОВ ================⁡ //

/* Не дает программе упасть если ввели не число (единственная строчка, которую я не смогу объяснить) */
int readInt(void)
{
    int x, c;
    while (scanf("%d", &x) != 1)
    {
        if (feof(stdin)) return 0; // конец ввода - иначе цикл был бы бесконечным
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Ошибка! Введите число: ");
    }
    while ((c = getchar()) != '\n' && c != EOF); // убираем '\n' после числа, иначе он сломает readLine
    return x;
}

// Читает строку с пробелами (максимум size-1 символов) и убирает '\n'
void readLine(char *buf, size_t size) 
{
    if (fgets(buf, (int)size, stdin) == NULL) // конец ввода или ошибка
    {
        buf[0] = '\0';
        return;
    }

    size_t len = strcspn(buf, "\n"); // позиция первого '\n' (или конец строки)
    if (buf[len] == '\n')
        buf[len] = '\0'; // заменяем '\n' на конец строки

    else // строка не влезла в буфер - выкидываем остаток из потока
    { 
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

// ============ ДЛЯ ИМЕНИ ФЕРМЕРА ============ //

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

// ============= РАБОТА С ИНВЕНТАРЕМ и items.txt ============= //

// Заполняет массив названиями по умолчанию из names
void fillDefaultNames(char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    for (int i = 0; i < MAX_ITEMS; i++) // Перебор строк
    {
        strncpy(item_names[i], names[i], MAX_NAME_LEN - 1);
        item_names[i][MAX_NAME_LEN - 1] = '\0'; // В конце добавляем '\0'
    }
}

// Загружает названия из файла в формате "ID Название" (по одной строке на предмет).
// Возвращает 1, если файл открылся, и 0, если нет (тогда остаются названия по умолчанию).
int loadItemNames(char item_names[MAX_ITEMS][MAX_NAME_LEN], const char *filename)
{
    // Сначала заполняем стандартными (names) - так программа не упадет ни при каких проблемах с файлом
    fillDefaultNames(item_names);

    FILE *f = fopen(filename, "r");
    if (f == NULL) // Если файл не открывается + тогда остаются названия стандартные (из names)
    {
        printf("Предупреждение: файл %s не найден. Используются названия по умолчанию.\n", filename);
        return 0;
    }

    char line[LINE_BUF_LEN];
    int line_no = 0; // Для ошибок

    while (fgets(line, sizeof(line), f) != NULL) // Читает до последней строки, потом NULL
    {
        line_no++;
        line[strcspn(line, "\r\n")] = '\0'; // убираем '\n' и '\r' (если файл с Windows) 

        if (line[0] == '\0') continue; // пропускаем пустые строки

        int id;
        char name[MAX_NAME_LEN];

        // %d - ID, потом пробел, потом всё до конца строки (максимум 31 символ)
        if (sscanf(line, "%d %31[^\n]", &id, name) != 2 || id < 0 || id >= MAX_ITEMS)
        // != 2 - пока считываются 2 значения (id и names). id < 0 || id >= MAX_ITEMS - пока id (0-9)
        {
            printf("Ошибка: строка %d в %s не считана - пропущена.\n", line_no, filename);
            continue;
        }

        strncpy(item_names[id], name, MAX_NAME_LEN - 1); // Закидываем
        item_names[id][MAX_NAME_LEN - 1] = '\0'; // В конце добавляем '\0'
    }

    fclose(f); // Закрываем файл
    return 1;
}


// ========= ПРОТОТИПЫ (нейронка сказала сделать, без этого не работало) ====== //

void printMenu(const char *farmer);
void showTime(int day, int hour);
void workTime(int *day, int *hour);
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void deleteItem(int inventory[]);
void clearTrash(int inventory[]);
int loadItemNames(char item_names[MAX_ITEMS][MAX_NAME_LEN], const char *filename);


// ====================== MAIN ====================⁡ //

int main(void) 
{
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
        }
    } while (choice != 0);

    return 0;
}

// ⁡⁢⁣====== МЕНЮ ========⁡ //
void printMenu(const char *farmer) 
{
    printf("\n====== МЕНЮ ====== | Фермер: %s", farmer);
    printf("\n1. - Посмотреть на часы\n");
    printf("2. - Промотать время (Поработать)\n");
    printf("3. - Посмотреть инвентарь\n");
    printf("4. - Положить предмет в слот\n");
    printf("5. - Выбросить предмет\n");
    printf("6. - Очистить мусор\n"); // Мой 5-ый вариант 
    printf("0. - Выход\n");
    printf("Выбор: ");
}
    


// ⁡⁢⁣========⁣======== ФУНКЦИИ ДЛЯ CASES ⁣================⁡ //

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
        printf("Слот %d: [%d] - (%s)\n", num, inventory[num], item_names[inventory[num]]);};
        
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
    printf("Слот %d - очищен!\n", index); // Выводим
}

// [6-МОЙ 5-ЫЙ ВАРИАНТ] ОТЧИСТКА ОТ МУСОРА
void clearTrash(int inventory[])
{
    printf("Введите ID предмета, который хотите удалить (1-%d): ", MAX_ITEMS - 1);
    int id = readInt(); // Запрашиваем ID

    if (id <= 0 || id >= MAX_ITEMS) // Неверный ввод
    {printf("Введен неправильный ID!\n"); return;} 

    int count = 0; // Для подсчета кол-ва слотов
    for (int num = 0; num < SIZE; num++) // Перебор слотов
        if (inventory[num] == id) {inventory[num] = 0; count++;} // Чистим, если есть

    printf("Очищено слотов: %d\n", count); // Выводиим
}

// ================ Лаба 3 ================ //

// [7] 

