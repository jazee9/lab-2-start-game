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
#define MAX_FARMER_NAME 32 // Имя фермера: 31 символ + '\0'

// ========= ПРОТОТИПЫ ======== //
void printMenu(const char *farmer);
void showTime(int day, int hour);
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void deleteItem(int inventory[]);
void clearTrash(int inventory[]);
void findItem(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void write_diary(const char *farmer, int day, int hour, const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void timeMenu(Player *p, GameTime *t);
void skipTime(GameTime *t);
void sell_maps(Player *p);

// ====================== MAIN ====================⁡ //

int main(void) 
{
    // Что бы rand() не выдавал одну и ту же последовательность 
    srand((unsigned)time(NULL)); 

    // Инициализация времени 
    GameTime time = {1, 8}; // При запуске день 1, 8:00 

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
        // ВЫХОД 
        case 0: printf("Выход...\n"); free_player(player); break;  

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
        case 2: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                print_player_card(player, item_names); break;

        // УПРАВЛЕНИЕ ВРЕМЕНЕМ
        case 3: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                timeMenu(player, &time); break; // Вызов подменню
        
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
                write_diary(player -> name, time.day, time.hour, player -> inventory, item_names); break;

        // АНАЛИЗ ЛОГА
        case 10: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                analyzeLog(); break; 

        // ПРОДАТЬ КАРТЫ
        case 11: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                sell_maps(player); break;
     
        // ПРИКЛЮЧЕНИЕ
        case 12: if (player == NULL) {printf("Персонаж не создан!\n"); break;}
                forestRingAdventure(player, &time); break;

        default: printf("Нет такого пункта меню.\n"); break;
        } 
    } while (choice != 0);
    return 0;
}

// ================ ФУНКЦИИ ================ //

// ⁡⁢Для вывода меню
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
    printf("11. - Продать зарисованные карты\n");
    printf("12. - Отправится в приключение (Лес-кольцо)\n");
    printf("0.  - Выход\n");
    printf("\nВыбор: ");
}

// Для [case: 3] "Подменю"
void timeMenu(Player *p, GameTime *t)
{
    int choice;
    do {
        printf("\n--- УПРАВЛЕНИЕ ВРЕМЕНЕМ ---\n");
        printf("1. - Посмотреть на часы\n");
        printf("2. - Пропустить время (Ждать)\n");
        printf("3. - Отдых (Лечь спать)\n");
        printf("0. - Назад в главное меню\n");
        printf("Выбор: ");

        choice = readInt();

        switch (choice) 
        {
            case 0: break; // Выход в главное меню   
            case 1: showTime(t -> day, t -> hour); break;
            case 2: skipTime(t); break;
            case 3: 
                {
                    printf("Вы ложитесь спать...\n");
                    
                    t -> day += 1;
                    t -> hour = 8;
                    p -> hp = p -> max_hp;
                    p -> stamina = p -> max_stamina;
                    printf("Наступило утро! День %d, время: %d:00\n", t -> day, t -> hour);
                    printf("Здоровье и выносливость полностью восстановлены!\n");
                } break;
            default: printf("Нет такого пункта.\n");
        }
    } while (choice != 0);
}

// ПОСМОТРЕТЬ ВРЕМЯ
void showTime(int day,int hour) {printf("Текущее время: День %d, время: %d:00\n", day, hour);}

// ПРОМАТАТЬ ВРЕМЯ (Ждать)
void skipTime(GameTime *t)
{ 
    printf("Сколько часов промотать?: "); 
    int h = readInt(); 

    if (h < 1) {printf("Кол-во часов должно быть не меньше 1!\n"); return;}

    int maxSkip; // Для подсчета сколько часов осталось до ближайших 2:00

    if (t -> hour < 2) maxSkip = 2 - t -> hour; // Если щас 0:00 или 1:00
    // if (*hour == 2) {printf("Лучше лечь спать");}
    else maxSkip = (24 - t -> hour) + 2; // Если щас 2:00 или позже

    // Проверка
    if (h > maxSkip) 
    {
        printf("Нельзя перемотать дальше 2:00 ночи!\n");
        printf("Максимум можно промотать на %d ч.\n", maxSkip);
        return;
    }

    t -> hour += h; 
    t -> day += t -> hour / HOURS_IN_DAY; // Считаем день
    t -> hour %= HOURS_IN_DAY; // Сколько часов по итогу

    showTime(t -> day, t -> hour); // Выводим время после перемотки
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