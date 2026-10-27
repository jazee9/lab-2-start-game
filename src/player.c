/* Файл для всего, что связано с фермером 
(его характеристики и рюкзак). */ 

// ==== Подключение файлов ==== //
#include "player.h"

// ================ ФУНКЦИИ ================ //

// ======== РАБОТА С ИНВЕНТАРЕМ ======== //

// [case: 3] ПОСМОТРЕТЬ ИНВЕНТАРЬ
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]) 
{
    for (int num = 0; num < SIZE; num++) 
        {if (inventory[num] >= 0 && inventory[num] < MAX_ITEMS) // Перебор слотов
        printf("Слот %d: [%d] - %s\n", num, inventory[num], item_names[inventory[num]]);}     
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