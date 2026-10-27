/* Вспомогательные функции, которые нужны везде, 
но не относятся к игровой логике. */ 

// ==== Подключение файлов ==== //
#include "utils.h"

// ==== Константы ==== //
const char *names[10] = {"Пусто", "Дерево", "Камень", "Семена пшеницы", "Железная руда", 
    "Золотая руда", "Алмазная руда", 
    "Вода", "Ягоды", "Веревка"};

// ============ ФУНКЦИИ ============ //

// ============ НЕККОРЕКТНЫЕ ВВОДЫ ============ //

// Если ввели не число (программа не падает)
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

// ============= РАБОТА С ИНВЕНТАРЕМ и items.txt ============= //

// ЗАПОЛНЯЕТ НАЗВАНИЯМИ ПО УМОЛЧАНИЮ (из names)
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

// ============ АНАЛИЗАТОР ЛОГОВ ============ //

// [case: 9] АНАЛИЗАТОР ЛОГОВ
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

// ГЕНЕРАЦИЯ СЛУЧАЙНОГО ЧИСЛА В ДИАПАЗОНЕ
int randInRange(int lo, int hi) 
{
    return lo + rand() % (hi - lo + 1); // возвращает число в диапазоне [lo, hi]
}