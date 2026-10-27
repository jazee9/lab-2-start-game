#ifndef PLAYER_H 
#define PLAYER_H 

// ==== Импорт библиотек ==== //
#include <stdio.h>
#include "utils.h"

// ==== Константы ==== //

// Инвентарь
#define SIZE         10  // Кол-во слотов в инвентаре 
#define MAX_ITEMS    10  // Сколько всего видов предметов (ID)
#define MAX_NAME_LEN 32  // Название предмета: 31 символ + '\0'

// ==== Структуры ==== //

// Данные о персонаже
typedef struct player
{
    char name[32];
    int inventory[10];   // Массив слотов инвентаря (ID предметов)
    int hp;              // Текущее здоровье
    int max_hp;          // Максимальное здоровье
    int stamina;         // Текущая выносливость
    int max_stamina;     // Максимальная выносливость
    int gold;            // Золото
    
    // --- Поля для 6-го варианта (Картография) ---
    int maps_drawn;      // Сколько карт зарисовано (на продажу)
} Player;

// ==== Прототипы функций ==== //
void showInventory(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void putItem(int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
void deleteItem(int inventory[]);
void clearTrash(int inventory[]);
void findItem(const int inventory[], char item_names[MAX_ITEMS][MAX_NAME_LEN]);
Player* create_player(void);
void free_player(Player *p);
void print_player_card(const Player *p, char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif