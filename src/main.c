// === ИМПОРТ ===⁡ //
#include <stdio.h> // Для ввода и вывода 
#include <string.h> // Для работы со строками (strlen, strcspn, strncpy)
#include <stdlib.h> // Для работы с динамич. памятью и случайных чисел
#include <time.h> // Для рандомных чисел

// ⁡⁢⁣========== КОНСТАНТЫ ============⁡ //
#define HOURS_IN_DAY 24 // Константа для кол-ва часов в сутках

// Для инвентаря
#define SIZE         10  // Кол-во слотов в инвентаре
#define MAX_ITEMS    10  // Сколько всего видов предметов (ID)
#define MAX_NAME_LEN 32  // Название предмета: 31 символ + '\0'
#define LINE_BUF_LEN 128 // Буфер для одной строки файла

// Для имени фермера
#define MAX_FARMER_NAME 32 // Имя фермера: 31 символ + '\0'

// Для карты
#define MIN_MAP 5 // Размеры карты
#define MAX_MAP 30
#define OBSTACLE_MIN_PERCENT 15 // Вероятности припятствия
#define OBSTACLE_MAX_PERCENT 30
#define RING_TRIGGER_COUNT 3

// Структура для карты
typedef struct 
{
    int height; // Высота карты
    int width; // Ширина карты
    int hero_x, hero_y; // Cтолбец и строка героя
    char **cells; // Карта в динамической памяти
    int edges_crossed; // Сколько раз герой вышел за край
    int portal_active; // Активация портала. 0 - портала нет, 1 - есть
    int on_camp; // Персонаж стоит на костре. 1 - стоит, 0 - стоит на обычной клетке (не костер)
    int maps_passed; // Счетчик дальности от дома (сколько пройденно карт)
} ForestMap;

// Стандартные названия (страховка, если будут проблемы с items.txt)
const char *names[10] = {"Пусто", "Дерево", "Камень", "Семена пшеницы", "Железная руда", 
    "Золотая руда", "Алмазная руда", 
    "Вода", "Ягоды", "Веревка"};

char status_message[256] = ""; // Глобальный буфер для сообщений

// ⁡⁢⁣========= ДЛЯ НЕКОРЕКТНЫХ ВВОДОВ ================⁡ //

// Не дает программе упасть если ввели не число 
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
// Возвращает 1, если файл открылся, и 0, если нет (тогда остаются названия по умолчанию (из names)).
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
        line[strcspn(line, "\r\n")] = '\0'; // Убираем '\n' и '\r' (если файл с Windows) 

        if (line[0] == '\0') continue; // Пропускаем пустые строки

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


// ========= ПРОТОТИПЫ ======== //

void printMenu(const char *farmer);
void showTime(int day, int hour);
void workTime(int *day, int *hour);
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void deleteItem(int inventory[]);
void clearTrash(int inventory[]);
int loadItemNames(char item_names[MAX_ITEMS][MAX_NAME_LEN], const char *filename);
void findItem(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void write_diary(const char *farmer, int day, int hour, const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void analyzeLog(void);
void forestRingAdventure(void);


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

// ⁡⁢⁣========⁣======== ФУНКЦИИ ДЛЯ CASES ⁣================⁡ //

// ================ Лаба 2 ================ //

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
        {if (inventory[num] >= 0 && inventory[num] < MAX_ITEMS) // Перебор слотов
        printf("Слот %d: [%d] - %s\n", num, inventory[num], item_names[inventory[num]]);}     
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

// [7] ПОИСК ПРЕДМЕТА В РЮКЗАКЕ ПО НАЗВАНИЮ
void findItem(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) 
{
    char query[MAX_NAME_LEN];
    printf("Введите название предмета: ");
    readLine(query, sizeof(query)); // Если строка будет с пробелами

    // Пустой ввод
    if (query[0] == '\0') {printf("Название не может быть пустым!\n"); return;}

    // Ищем ID в каталоге по названию
    int id = -1; // -1 - пока не нашли
    for (int i = 0; i < MAX_ITEMS; i++) // Перебираем строки
        if (strcmp(item_names[i], query) == 0) {id = i; break;} // strcmp возвращает 0, если совпали

    // Если нет в каталоге (id так и остался -1)
    if (id == -1) {printf("Предмета: \"%s\" - нет в каталоге!\n", query); return;}

    // Считаем, в скольких слотах он лежит
    int count = 0;
    for (int num = 0; num < SIZE; num++)
        if (inventory[num] == id) count++;

    // Если нет в инвентаре
    if (count == 0) {printf("Предмет: \"%s\" - не содержится в инвентаре\n", query); return;}

    // Один слот - "в слоте", несколько - "в слотах"
    if (count == 1) printf("Предмет: \"%s\" - содержится в слоте: ", query);
    else printf("Предмет: \"%s\" - содержится в слотах: ", query);

    // Вывод номеров
    int printed = 0;
    for (int num = 0; num < SIZE; num++)
    {
        if (inventory[num] != id) continue; // Если нет - дальше

        if (printed > 0) printf(", "); // Запятая только между номерами
        printf("№%d", num);
        printed++;
    }
    printf("\n");
}

// [8] ЗАПИСАТЬ СОСТОЯНИЕ В ДНЕВНИК ФЕРМЕРА (diary.txt)
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

// [9-МОй 5-ЫЙ ВАРИАНТ] АНАЛИЗАТОР ЛОГОВ
// В input.txt записан системный журнал событий, где каждая строка начинается с тега важности: 
// [INFO], [WARN] или [ERROR] (например, [ERROR] Здоровье игрока отрицательное, но он жив). 
// Программа должна подсчитать общую статистику по каждому из трёх типов сообщений, а в output.txt 
// выписать только строки с предупреждениями ([WARN]) и ошибками ([ERROR]), предварительно удалив сам тег 
// из начала строки и добавив номер строки из исходного файла.
void analyzeLog(void) 
{
    FILE *input = fopen("input.txt", "r");
    // input.txt файла нет
    if (input == NULL) {printf("Ошибка: не удалось открыть input.txt\n"); return;} 
    
    FILE *output = fopen("output.txt", "w");
    // output.txt файла нет
    if (output == NULL)
        {
            printf("Ошибка: не удалось открыть output.txt для записи.\n");
            fclose(input);
            return;
        }

    char line[LINE_BUF_LEN]; // массив символов line, в который будут считываться строки файла.
    int line_no = 0; // Номер строки в исходном файле
    int info_count = 0, warn_count = 0, error_count = 0; // Подсчет [INFO], [WARN], [ERROR]

    printf("\n--- [WARN] и [ERROR] из input.txt ---\n"); // Заголовок
        
    while (fgets(line, sizeof(line), input) != NULL) // Читаем до последней строки
    {
        line_no++; // Считаем все строки, в том числе пустые

        // ищет символ \n в строке и возвращает NULL, если не находит.
        if (strchr(line, '\n') == NULL) 
        {                          
            int c;
            // Читает и отбрасывает символы из файла до перевода строки или конца файла.
            while ((c = fgetc(input)) != '\n' && c != EOF);
        }
        
        line[strcspn(line, "\r\n")] = '\0'; // Убираем '\n' и '\r'

        const char *text = NULL; // Текст без тега (NULL - строку выводить не надо)

        // Если [INFO]
        if (strncmp(line, "[INFO]", strlen("[INFO]")) == 0)
            info_count++; // +1

        // Если [WARN]
        else if (strncmp(line, "[WARN]", strlen("[WARN]")) == 0)
        {
            warn_count++; // +1
            text = line + strlen("[WARN]"); // Перепрыгиваем тег
        }

        // Если [ERROR]
        else if (strncmp(line, "[ERROR]", strlen("[ERROR]")) == 0)
        {
            error_count++; // +1
            text = line + strlen("[ERROR]");
        }

        if (text != NULL) // Это WARN или ERROR
        {
            while (*text == ' ') text++; // Пропускаем пробелы после тега

            printf("Строка %d: %s\n", line_no, text);          // На экран
            fprintf(output, "Строка %d: %s\n", line_no, text); // В файл
        }
    }

    // Закрываем файлы
    fclose(input);
    fclose(output);

    // Вывод на экран
    printf("\n--- Статистика ---\n");
    printf("INFO:  %d\n", info_count);
    printf("WARN:  %d\n", warn_count);
    printf("ERROR: %d\n", error_count);
    printf("Результат записан в output.txt\n");
}


// ========== ЛАБА 4 (5 ВАРИАНТ) ============== //

// ФУНКЦИЯ ДЛЯ ВЫДЕЛЕНИЯ ДВУМЕРНОГО МАССИВА
char **allocGrid(int h, int w) // Принимает высоту и ширину карты
{
    char **g = (char**)malloc((size_t)h * sizeof(char*)); // выделяем массив из h-указателей

    if (g == NULL) return NULL; // Проверяет выделилась ли память, если нет - сразу выходим NULL

    for (int i = 0; i < h; i++) // Перебор строк
    {
        g[i] = (char*)malloc((size_t)w * sizeof(char)); // Для каждой строки выделяется w

        if (g[i] == NULL) // И опять проверка
        {
            for (int j = 0; j < i; j++) free(g[j]); // Если очередная строка не выделилась — откат всего
            free(g); // Освобождаем массив указателей
            return NULL;
        }
    }
    return g; // вовзращаем указатель на массив, если все выделилось
}

// ФУНКЦИЯ ДЛЯ ОСВОБОЖДЕНИЯ ДВУМЕРНОГО МАССИВА
void freeGrid(char **g, int h)
{
    if (g == NULL) return; // Проверка есть ли, что освобождать

    // Если есть что
    for (int i = 0; i < h; i++) free(g[i]); // Освобождаем каждую строку
    free(g); // Освобождаем массив указателей
}

// ФУНКЦИЯ ДЛЯ ОСВОБОЖДЕНИЯ ВСЕЙ КАРТЫ
void freeForest(ForestMap *m)
{
    if (m == NULL) return; // Защита от NULL структуры

    freeGrid(m -> cells, m -> height); // Освобождает двумерный массив cells через freeGrid
    m -> cells = NULL; // обнуляет указатель после освобождения
}

// ГЕНЕРАЦИЯ СЛУЧАЙНОГО ЧИСЛА В ДИАПАЗОНЕ
static int 
randInRange(int lo, int hi) 
{
    return lo + rand() % (hi - lo + 1); // возвращает число в диапазоне [lo, hi]
}

// ФУНКЦИЯ ДЛЯ ПРОВЕРКИ СВОБОДНОГО СОСЕДА У СПАВНА ИГРОКА, КОСТРА И ПОРТАЛА
// ЧТО БЫ ОНИ НЕ БЫЛИ В ЛОВУШКЕ
static int hasFreeNeighbor(ForestMap *m, int x, int y) 
{
    // 4 стороны в округ
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    // Перебираем каждую сторону и смотрим
    for (int i = 0; i < 4; i++) 
    {
        // Координаты соседов
        int nx = x + dx[i];
        int ny = y + dy[i];

        // Если сосед за пределаи карты - не считать
        if (nx < 0 || nx >= m -> width || ny < 0 || ny >= m -> height) continue;

        // Если есть хотя бы 1 сосед [.] - выходим из hasFreeNeighbor с 1 - успех
        if (m -> cells[ny][nx] == '.') return 1;
    }
    return 0; // Если в округ припятствия, после того как все 4 просмотрели
}

// ФУНКЦИЯ ДЛЯ ЗАПОЛНЕНИЯ ForestMap И СОЗДАНИЯ НОВОГО УЧАСТКА ЛЕСА
int generateForest(ForestMap *m) 
{
    // Размеры карты, может быть не квадрат! Используем функцию randInRange
    // Может выпасть например h = 12, w = 27 (12x27 карта)
    m -> height = randInRange(MIN_MAP, MAX_MAP); // Для высоты
    m -> width = randInRange(MIN_MAP, MAX_MAP); // Для ширины

    // Выделяем память с числами которые выпали для height и width
    m -> cells = allocGrid(m -> height, m -> width); // Закидываем в allocGrid
    if (m -> cells == NULL) return 0; // Если что-то не сработало

    // [.] Заполняем карту точками
    for (int count_h = 0; count_h < m -> height; count_h++)  // Перебор строк до последней
    {
        for (int count_w = 0; count_w < m -> width; count_w++) // Перебор столбцов до последнего
        {
            m -> cells[count_h][count_w] = '.'; // Заполняем точками
        }
    }

    // [T] [R] Заполняем случайно деревьями и камнями. (T) и (R)
    int pct = randInRange(OBSTACLE_MIN_PERCENT, OBSTACLE_MAX_PERCENT); // Cлучайный % (от 15 до 30)

    for (int y = 0; y < m -> height; y++) // Перебор строк до последней
    {
        for (int x = 0; x < m -> width; x++) // Перебор столбцов до последнего
        {
            if (rand() % 100 < pct) // Если выпадает клетке шанс получить припятствие, то ставим
            {
                m -> cells[y][x] = (rand() % 2) ? 'T' : 'R'; // Рандомим в нее T или R
            }
        }
    }

    // [%] Спавним героя
    int placed = 0; // placed = 0 герой не поставлен, 1 - посвлен

    // Пока !tries (то есть не поставлен персонаж) и tries < 2000 (ограничение)
    /* Ограничение, потому что вся карта мб будет в T и R (не ну а чо нет) и  
    тогда ставить некуда будет перса. Поэтому ограничение, что бы не упала программа */ 
    for (int tries = 0; tries < 2000 && !placed; tries++) 
    {
        // Генерируем случайные координаты для спавна игрока
        int y = randInRange(0, m -> height - 1); // x
        int x = randInRange(0, m -> width - 1); // y
        // -1 потому что если например height 12, то x ∈ [0, 11]

        // Проверяем нет ли там припятствия T или R (точка ли там?)
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y)) 
        {
            m -> cells[y][x] = '@'; // Если '.', то ставим героя

            // Записываем координаты игрока в hero_x, hero_y
            m -> hero_x = x;
            m -> hero_y = y; 
            placed = 1; // Поставили
        }
    }

    // Если не хватило 2000 попыток, то возвращаем 0 (не, ну а вдруг)
    if (!placed) return 0; 

    // [^] Создаем костер. Собственно также
    // ==== КОСТЕР СОЗДАЕТСЯ ПОСЛЕ СПАВНА ГЕРОЯ ЧТО БЫ ОН НЕ ПОПАЛ НА ГЕРОЯ СРАЗУ === // 
    int camp_ok = 0;
    for (int tries = 0; tries < 1000 && !camp_ok; tries++) // Также
    {
        // Рандомом генерируем координаты
        int y = randInRange(0, m -> height - 1); // y
        int x = randInRange(0, m -> width - 1); // x  

        // Проверяем нет ли там припятствия T или R (точка ли там?)
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y))
        {
            m -> cells[y][x] = '^'; // Если '.', то ставим костер
            camp_ok = 1; // Поставили
        }
    }
    // Если не хватило 1000 попыток, то возвращаем 0 (не, ну а вдруг)
    if (!camp_ok) return 0;

    // Сброс счетчиков
    m -> edges_crossed = 0;
    m -> portal_active = 0;
    m -> on_camp = 0;
    return 1;
}

// ФУНКЦИЯ ВЫВОДА КАРТЫ ЛЕСА НА ЭКРАН, ИНСТРУКЦИЯ ПО УПРАВЛЕНИЮ И ЗАГОЛОВОК
void printForest(const ForestMap *m) 
{
    // Заголовок
    printf("\n--- Локация Лес-кольцо %dx%d. Пройденно карт: %d | Переходов через край: %d ---\n\n", 
        m -> width, m -> height, m -> maps_passed, m -> edges_crossed);

    // Перебираем
    for (int y = 0; y < m -> height; y++) 
    {
        for (int x = 0; x < m -> width; x++) 
            putchar(m -> cells[y][x]); // Печатаем putchar'ом по одной
        putchar('\n'); // Перевод строки
    }

    // Инструкция по управлению
    printf("\nW/A/S/D - идти. Q - домой (только на ^)\n");

    // Вывод подсказок под картой
    if (status_message[0] != '\0') {
        printf("\n>>> %s <<<\n", status_message);
    }
    printf("\n");
}

// [P] СПАВН ПОРТАЛА
void maybeSpawnPortal(ForestMap *m) 
{
    // Проверки при попадании в эту функцию
    if (m -> portal_active) return; // portal_active = 0? Если 1 - выходим
    if (m -> edges_crossed < RING_TRIGGER_COUNT) return; // Прошел ли игрок 3+ карты? Если нет - выходим

    for (int tries = 0; tries < 2000; tries++)  // 2000 попыток спавна портала
    {
        // Рандомизируем координаты
        int y = randInRange(0, m -> height - 1); // y
        int x = randInRange(0, m -> width - 1); // x  

        // Проверяем точка ли там
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y)) 
        {
            m -> cells[y][x] = 'P'; // Ставим P, ели свободно
            m -> portal_active = 1; // Меняем m -> portal_active на 1

            // Уведомляем игрока о появлении портала
            strcpy(status_message, "Вы заблудились... На карте появился портал (P)");
            return;
        }
    }
}

// ФУНКЦИЯ ДЛЯ ПЕРЕДВИЖЕНИЯ ИГРОКА ПО КАРТЕ
int tryMoveForest(ForestMap *m, const char *dir) 
{
    // Для подсчетов
    int dx = 0;
    int dy = 0;

    // Биндим кнопки
    if (dir[0] == 'W' || dir[0] == 'w' || strncmp(dir, "Ц", 2) == 0 || 
        strncmp(dir, "ц", 2) == 0) dy = -1; // Вверх
    else if (dir[0] == 'S' || dir[0] == 's' || strncmp(dir, "Ы", 2) == 0 || 
        strncmp(dir, "ы", 2) == 0) dy = 1; // Вниз
    else if (dir[0] == 'A' || dir[0] == 'a' || strncmp(dir, "Ф", 2) == 0 || 
        strncmp(dir, "ф", 2) == 0) dx = -1; // Влево
    else if (dir[0] == 'D' || dir[0] == 'd' || strncmp(dir, "В", 2) == 0 || 
        strncmp(dir, "в", 2) == 0) dx = 1; // Вправо
    else return 0; // Если нажал не то, ничего не происходит

    // Новая координата
    int nx = m -> hero_x + dx;
    int ny = m -> hero_y + dy;

    // 0 - не вышел за край, 1 - вышел за край
    int crossed = 0;

    // выходы за края
    if (nx < 0) {nx = m -> width - 1; crossed = 1;} // Если вышел за ЛЕВЫЙ, тепаем у ПРАВОГО 
    if (nx >= m -> width) {nx = 0; crossed = 1;} // Если вышел за ПРАВЫЙ, тепаем у ЛЕВОГО 
    if (ny < 0) {ny = m -> height - 1; crossed = 1;} // Если вышел за ВЕРХНИЙ, тепаем у НИЖНЕГО 
    if (ny >= m -> height) {ny = 0; crossed = 1;} // Если вышел за НИЖНИЙ, тепаем у ВЕРХНЕГО
    
    // Узнаем, что в той клетке, в которую идем
    char c = m -> cells[ny][nx];

    // Если припятствие [T] [R]
    if (c == 'T' || c == 'R') 
    {
        strcpy(status_message, "Туда нельзя, препятствие!");
        return 0;
    } 

    // Переходы через край
    if (crossed) 
    {
        m -> edges_crossed++;
        sprintf(status_message, "Вы пересекли край леса (переходов: %d)", m->edges_crossed);
    }

    // Если перешел, то стираем героя со старой клетки и возвращаем туда [.] или [^]
    m -> cells[m -> hero_y][m -> hero_x] = m -> on_camp ? '^' : '.';
    m -> on_camp = 0; // Если был на костре и ушел с него

    // Если портал [P]
    if (c == 'P') 
    {
        m -> cells[ny][nx] = '@'; // Ставим героя
        m -> hero_x = nx; m -> hero_y = ny; // Обнуляем координаты, тк будет новая карта
        return 1;
    }

    // После проверки припятствий и прочего
    m -> hero_x = nx; m -> hero_y = ny;

    if (c == '^')  // Если там костер
    {
        m -> on_camp = 1;
        m -> cells[ny][nx] = '@';

        strcpy(status_message, "Вы у костра, можно передохнуть (Q - выйти домой)");
    }

    else {m -> cells[ny][nx] = '@';} // Если там ничего

    maybeSpawnPortal(m);
    return 0;
}

// [10, 5-ЫЙ ВАРИАНТ] ОТПРАВИТСЯ В ПРИКЛЮЕНИЕ (ЛЕС-КОЛЬЦО) 
// ГЛАВНАЯ ЗАПУСКАЮЩАЯ ФУНКЦИЯ
void forestRingAdventure(void) 
{
    ForestMap m; // Запускает структуру
    memset(&m, 0, sizeof(m)); // Дает 0 всем в структуре
    m.maps_passed = 1; 
    status_message[0] = '\0'; // Отчистка буфера

    if (!generateForest(&m)) 
    {
        printf("Ошибка генерации леса!\n");
        freeForest(&m); 
        return;
    }

    int running = 1;
    system("clear");
    printForest(&m); // Рисуем первую карту

    while (running) 
    {
        // Читаем ход (карта уже на экране, сообщений нет)
        printf("\nХод: ");
        char ch[10];
        if (fgets(ch, sizeof(ch), stdin) == NULL) break;

        status_message[0] = '\0'; // Очищаем сообщение перед ходом

        // [Q] (выход)
        if (ch[0] == 'Q' || ch[0] == 'q' || (strncmp(ch, "Й", 2) == 0) || (strncmp(ch, "й", 2) == 0))  
        {
            if (m.on_camp == 1) // На костре?
            {
                printf("Вы отдохнули у костра и вернулись домой.\n");
                running = 0; 
            }
            else 
            {
                strcpy(status_message, "Выйти домой можно только стоя на костре (^).");

                system("clear");
                printForest(&m);
            } 
            continue; // Не перерисовываем карту, просто покажем сообщение (или выйдем)
        }

        // Движение
        int res = tryMoveForest(&m, ch);

        if (res) 
        {
            m.maps_passed++; 
            freeForest(&m); 
            if (!generateForest(&m)) {printf("Ошибка генерации.\n"); break;}
        }

        // Очищаем экран и рисуем карту + новое сообщение (если есть)
        system("clear");
        printForest(&m);
    } 
    freeForest(&m); 
}