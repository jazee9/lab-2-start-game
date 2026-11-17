#ifndef ENITY_H
#define ENITY_H

// Импорты
#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "player.h"

// Перечисление типов объектов
typedef enum
{
    ENT_WOOD,   // Дерево
    ENT_STONE,  // Камень
    ENT_HEAL,   // Аптечка
    ENT_MONSTER // Монстр
} EntityType;

// Структура для объектов на карте
typedef struct Entity
{
    EntityType type;  // Тип
    char symbol;      // Символ на карте
    int x, y;         // Координаты
    int hp;           // Текущее хп
    int max_hp;       // Максимальное хп
    int attack;       // Урон
    int drop_item_id; // ID предмета, который выпадет
    int is_alive;     // 1 - живой/целый, 0 - уничтожен
} Entity;

// Прототипы функций
int generate_entities(ForestMap *m, Entity **entities); // Генерация сущностей
void free_entities(Entity *entities, int count);        // Очистка памяти
// Фунция, которая будет срабатывать, когда игрок шагает на + или M
int interact_with_entity(Player *p, Entity *entities, int entity_count, int target_x, int target_y, ForestMap *m, char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif