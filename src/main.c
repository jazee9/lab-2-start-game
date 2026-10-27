// ======== ИМПОРТ ========⁡ //

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
void timeMenu(Player *p, int *day, int *hour);

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
    int choice; 

    Player *player = NULL; // Cоздаем указатель на игрока

    loadItemNames(item_names, "items.txt"); // Читаем названия из items.txt

    do  
    {
        printMenu(player ? player->name : "Не создан"); 
        choice = readInt(); // Выбор варианта пользователем

        switch (choice) 
        {

        // ⁡⁢⁣Если введена неверная цифра для выбора пункта меню
        default: printf("Нет такого пункта меню.\n"); break;

        // ⁡⁢⁣========= CASES =========⁡ //

        // ВЫХОД 
        case 0: printf("Выход...\n");  
                free_player(player); break;  

        // СОЗДАТЬ / ПЕРЕСОЗДАТЬ        
        case 1: if (player != NULL) 
                    {
                        printf("Персонаж уже существует! Удалить его? (1 - Да, 0 - Нет): ");
                        if (readInt() == 1) 
                        {
                            free_player(player);
                            player = NULL;
                        } else break;
                    }
                player = create_player(); break;
 
        // КАРТОЧКА ПЕРСОНАЖА 
        case 2: if (player == 0) {printf("Персонаж не создан!\n"); break;}
                print_player_card(player, item_names); break;
        
        // УПРАВЛЕНИЕ ВРЕМЕНЕМ
        case 3: timeMenu(player, &current_day, &current_hour); break; // Вызов подменню
        
        // ПОСМОТРЕТЬ ИНВЕНТАРЬ
        case 4: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                printf("\n> Инвентарь:\n");
                showInventory(player -> inventory, item_names); break;

        // ПОЛОЖИТЬ ПРЕДМЕТ В СЛОТ
        case 5: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                putItem(player -> inventory, item_names); break; 

        // ВЫБРОСИТЬ ПРЕДМЕТ
        case 6: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                deleteItem(player -> inventory); break; 

        // ОЧИСТИТЬ МУСОР ИНВЕНТАРЯ
        case 7: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                clearTrash(player -> inventory); break;

        // НАЙТИ ПРЕДМЕТ В ИНВЕНТАРЕ
        case 8: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                findItem(player -> inventory, item_names); break; 
                
        // ЗАПИСАТЬ СОСТОЯНИЕ В ДНЕВНИК ФЕРМЕРА
        case 9: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                write_diary(player -> name, current_day, current_hour, player -> inventory, item_names); break;

        // АНАЛИЗ ЛОГА
        case 10: analyzeLog(); break; 

        // ПРИКЛЮЧЕНИЕ
        case 11: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                forestRingAdventure(player); break;
        } 
    } while (choice != 0); // <--- А ЭТА ЗАКРЫВАЕТ DO
    return 0;
}

// ⁡⁢⁣====== ВЫВОД МЕНЮ ========⁡ //

void printMenu(const char *farmer) 
{
    printf("\n====== МЕНЮ | Фермер: %s ======\n", farmer);
    printf("\n1.  - Создать / пересоздать персонажа\n");
    printf("2.  - Карточка персонажа\n");
    printf("3.  - Управление временем\n"); 
    printf("4.  - Посмотреть инвентарь\n");
    printf("5.  - Положить предмет в слот\n");
    printf("6.  - Выбросить предмет\n");
    printf("7.  - Очистить мусор из инвентаря\n");
    printf("8.  - Найти предмет в инвентаре\n");
    printf("9.  - Записать состояние в дневник фермера\n");
    printf("10. - Анализ лога\n");
    printf("11. - Отправится в приключение (Лес-кольцо)\n");
    printf("0.  - Выход\n");
    printf("\nВыбор: ");
}

// ================ ФУНКЦИИ ================ //

// Для [case: 3]
void timeMenu(Player *p, int *day, int *hour) 
{
    int choice;
    do {
        // Менюшка
        printf("\n--- УПРАВЛЕНИЕ ВРЕМЕНЕМ ---\n");
        printf("1. - Посмотреть на часы\n");
        printf("2. - Пропустить время (Поработать)\n");
        printf("3. - Отдых (Лечь спать)\n");
        printf("0. - Назад в главное меню\n");
        printf("Выбор: ");

        choice = readInt();

        switch (choice) 
        {
            case 0: 
                break; // Выход в главное меню
                
            case 1: 
                showTime(*day, *hour); 
                break;
                
            case 2: 
                workTime(day, hour); 
                break;
                
            case 3: 
                if (p == NULL) 
                {
                    printf("Персонаж не создан! Сначала создайте его в главном меню.\n");
                } 
                else 
                {
                    printf("Вы ложитесь спать...\n");
                    *day += 1;
                    *hour = 8;
                    p -> hp = p -> max_hp;
                    p -> stamina = p -> max_stamina;
                    printf("Наступило утро! День %d, время: %d:00\n", *day, *hour);
                    printf("Здоровье и выносливость полностью восстановлены!\n");
                } 
                break;
                
            default: 
                printf("Нет такого пункта.\n");
        }
    } while (choice != 0);
}

// ПОСМОТРЕТЬ ВРЕМЯ
void showTime(int day,int hour) {printf("Текущее время: День %d, время: %d:00\n", day, hour);}

// ПРОМАТАТЬ ВРЕМЯ (ПОРАБОТАТЬ)
void workTime(int *day, int *hour)
{ 
    printf("Сколько часов поработать?: "); int h = readInt(); // Спрашиваем у пользователя
    if (h < 0) {printf("Кол-во часов не могут быть отрицательными!\n"); return;} // Неверный ввод

    *hour += h; // Прибавляем часы, введенные пользователем
    *day += *hour / HOURS_IN_DAY; // Считаем день
    *hour %= HOURS_IN_DAY; // Сколько часов по итогу

    showTime(*day, *hour); // Выводим время после перемотки
}

// ЗАПИСАТЬ СОСТОЯНИЕ В ДНЕВНИК ФЕРМЕРА (diary.txt)
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