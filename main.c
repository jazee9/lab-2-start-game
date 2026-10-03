// ⁡⁢⁣⁢===⁡ ⁡⁢⁣⁢ИМПОРТ⁡ ⁡⁢⁣⁢===⁡ //
#include <stdio.h> // Для ввода и вывода 

// ⁡⁢⁣⁢===⁡ ⁡⁢⁣⁢КОНСТАНТЫ⁡ ⁡⁢⁣⁢===⁡ //
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

// ⁡⁢⁣⁢===========⁡ ⁡⁢⁣⁢MAIN⁡ ⁡⁢⁣⁢=========⁡ //

int main(void) 
{// Инициализация основных переменных 
    int current_day = 1;
    int current_hour = 8;
    char item_names[MAX_ITEMS][MAX_NAME_LEN]; // Таблица названий предметов 10/32 - 1 ('\0')
    int inventory[SIZE] = {1, 1, 4, 5, 6, 7, 8, 3, 0, 0}; // Разложил инвентарь 
    int choice;

    
    do {// ====== МЕНЮ ====== //
        printf("====== МЕНЮ ======");
        printf("\n1. - Посмотреть на часы\n");
        printf("2. - Промотать время (Поработать)\n");
        printf("3. - Посмотреть инвентарь\n");
        printf("4. - Положить предмет в слот\n");
        printf("5. - Выбросить предмет\n");
        printf("6. - Очистить мусор\n"); // Мой 5-ый вариант 
        printf("0. - Выход\n");

        printf("Выбор: ");
        choice = readInt(); // Выбор варианта пользователем 

        // РАБОТА ЭТОГО МЕНЮ //
        switch (choice) 
            {// Если пользователь дурак, default - срабатывает, когда не сработал не один case 
                default:
                printf("Нет такого пункта меню.\n");
                break;

                case 0: printf("Выход...\n"); break; // ВЫХОД 
                case 1: showTime(current_day, current_hour); break; // ПОСМОТРЕТЬ ВРЕМЯ 
                case 2: workTime(&current_day, &current_hour); break; // ПРОМАТАТЬ ВРЕМЯ (ПОРАБОТАТЬ)
                case 3: showInventory(inventory, item_names); break;
            
                        
                case 4: // ПОЛОЖИТЬ ПРЕДМЕТ В СЛОТ
                    {printf("Введите индекс слота (0-9): "); // Запрашиваем индекс у пользователя
                    int index = readInt();

                    // Если пользватель не адекватный (неверный слот)
                    if (index < 0 || index > 9) {printf("Ошибка! Нет такого слота\n"); break;}

                    // Если адекватный - дальше 
                    else 
                    {printf("Введите ID предмета: "); // Запрашиваем ID предмета
                        int ID = readInt();
                        
                        // Если введен не правльный ID
                        if (ID < 0 || ID > SIZE) {printf("Введен не правильный ID!\n"); break;}

                        // Если все нормально - дальше
                        else {inventory[index] = ID;} // Кладем в слот
                        printf("Предмет: %s - положен в %d слот", names[inventory[ID]], index);} break;} // Выводим

                case 5: // ВЫБРОСИТЬ ПРЕДМЕТ
                    // Запрашиваем индекс у пользователя
                    {printf("Введите индекс слота (0-9): "); 
                    int index = readInt();
                    
                    // Введен не верный индекс
                    if (index < 0 || index > 9) {printf("Ошибка! Нет такого слота\n"); break;}

                    // Если все нормально - дальше
                    // Удаляем и выводим какой слот отчистили
                    else {inventory[index] = 0; printf("Слот: %d - отчищен!\n", index);} break;} 


                    case 6: // ОЧИСТКА ОТ МУСОРА (5 ВАРИАНТ)
                        // Запрашиваем ID предмета
                        {printf("Введите ID предмета, который хотите удалить (0-9): ");
                            int ID = readInt();
                            int count_slot = 0; // Чтоб считать кол-во удаленных

                            // Если введен не правльный ID
                            if (ID < 0 || ID > SIZE) {printf("Введен не правильный ID!\n"); break;}

                            // Если все нормально - дальше
                                // Перебор каждого слота
                            else for (int num_slot = 0; num_slot < SIZE; num_slot++) 
                                // Отчистка, если слот содержит ID, введеный пользователем
                                {if (inventory[num_slot] == ID) {inventory[num_slot] = 0; count_slot++;}} 
                            printf("Очищено слотов: %d\n", count_slot); break;} // Выводим
                } 
    } while (choice != 0); // Работает пока пользователь выбрал     
return 0;}

// ======== ФУНКЦИИ ДЛЯ CASES ======== //

// [1] Посмотреть время
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
        {if (inventory[num] >= 0 && inventory[num] <= 9) 
        {printf("Слот %d: [%d] (%s)\n", num, inventory[num], names[inventory[num]]);}} return;
}




                
