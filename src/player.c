/* Файл для всего, что связано с фермером 
(его характеристики и рюкзак). */ 

// ==== Подключение файлов ==== //
#include "player.h"
#include "map.h"

// ==== Константы ==== //
#define HOURS_IN_DAY 24 // Кол-во часов в сутках

// ================ ФУНКЦИИ ================ //

// ======== ДИНАМИЧЕСКИЙ ИГРОК ======== //

// [case: 1] СОЗДАНИЕ ПЕРСОНАЖА
Player* create_player(void) 
{
    Player *p = (Player*)malloc(sizeof(Player)); // Память
    if (!p) return NULL; // Проверка, что память выделилась

    printf("Как зовут фермера? ");
    readLine(p -> name, MAX_NAME_LEN); // Записываем имя

    // Характеристики при создании
    p -> max_hp = 100;
    p -> hp = 100;
    p -> max_stamina = 50;
    p -> stamina = 50;
    p -> gold = 50;
    p -> maps_drawn = 0; // Задел под 6-й вариант

    // Стартовый инвентарь
    int start_inv[10] = {2, 3, 4, 4, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < SIZE; i++) p->inventory[i] = start_inv[i];

    printf("Персонаж %s успешно создан!\n", p->name);
    return p;
}

// [case: 0] УДАЛЕНИЕ ПЕРСОНАЖА (Освобождение памяти)
void free_player(Player *p) 
{
    if (p != NULL) 
    {
        printf("\nПамять структуры по адресу %p освобождена\n\n", (void*)p);
        free(p);
    }
}

// [case: 2] ВЫВОД КАРТОЧКИ ПЕРСОНАЖА
void print_player_card(const Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]) 
{
    // Заголовки
    printf("\n--- Карточка: %s ---\n", p -> name);
    printf("\n> HP: %d/%d | Выносливость: %d/%d | Золото: %d\n", p -> hp, p -> max_hp, p -> stamina, 
        p -> max_stamina, p -> gold);

    if (p -> maps_drawn > 0) printf("Зарисовано карт: %d\n", p -> maps_drawn);

    printf("\n> Инвентарь:\n");
    showInventory(p -> inventory, item_names);
}

// ======== РАБОТА С ИНВЕНТАРЕМ ======== //

// [case: 3] ПОСМОТРЕТЬ ИНВЕНТАРЬ
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) 
{
    for (int num = 0; num < SIZE; num++) 
        {if (inventory[num] >= 0 && inventory[num] < MAX_ITEMS) // Перебор слотов
        printf("Слот %d: [%d] - %s\n", num, inventory[num], item_names[inventory[num]]);}     
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

// [case: 4] ПОЛОЖИТЬ ПРЕДМЕТ В ИНВЕНТАРЬ
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

// [case: 5] ВЫБРОСИТЬ ПРЕДМЕТ ИЗ ИНВЕНТАРЯ
void deleteItem(int inventory[])
{
    printf("Введите индекс слота (0-%d): ", SIZE - 1); // Спрашиваем в какой слот ложить
    int index = readInt(); // Записываем

    if (index < 0 || index >= SIZE) // Неверный ввод
    {printf("Ошибка! Нет такого слота\n"); return;} 

    inventory[index] = 0; // Чистим слот
    printf("Слот %d - очищен!\n", index); // Выводим
}

// [case: 6] ОТЧИСТКА ИНВЕНТАРЯ ОТ МУСОРА
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

// [case: 7] ПОИСК ПРЕДМЕТА В ИНВЕНТАРЕ ПО НАЗВАНИЮ
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

// Для [case: 11] Продать карты
void sell_maps(Player *p) 
{
    if (p -> maps_drawn == 0) {printf("У вас нет зарисованных карт!\n"); return;}

    printf("\nКурс за 1 карту - %d золота\n", PRICE_ONE_MAP);

    if (p -> maps_drawn == 1) 
    {
        p -> gold += PRICE_ONE_MAP;
        p -> maps_drawn -= 1;
        printf("\nУ вас была 1 карта, она продана за %d золота\n", PRICE_ONE_MAP);
        printf("Сейчас у вас %d золота!\n", p -> gold);
    }
    else if (p -> maps_drawn > 1) 
    {
        printf("Количество ваших зарисованных карт: %d\n", p -> maps_drawn);
        printf("Сколько карт вы хотите продать? ");
        
        int quantity_sale_map = readInt();

        // Некорректного ввод (меньше 1 или больше, чем есть)
        if (quantity_sale_map < 1 || quantity_sale_map > p -> maps_drawn) 
        {
            printf("Ошибка! Вы не можете продать столько карт!.\n");
            printf("Количество ваших зарисованных карт: %d\n", p -> maps_drawn);
            return; // Выходим, чтобы не начислять золото
        }

        // Если проверка пройдена - продаем
        int sum_gold_sale_map = quantity_sale_map * PRICE_ONE_MAP;

        p -> gold += sum_gold_sale_map;
        p -> maps_drawn -= quantity_sale_map;
        
        printf("Вы продали на %d золота\n", sum_gold_sale_map);
        printf("Осталось карт: %d. Теперь у вас %d золота.\n", p -> maps_drawn, p -> gold);
    }
}