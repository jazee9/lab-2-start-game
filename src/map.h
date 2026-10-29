#ifndef MAP_H
#define MAP_H

// ==== Импорт ==== //
#include <stdio.h> // Для ввода и вывода 
#include <string.h> // Для работы со строками (strlen, strcspn, strncpy)
#include <stdlib.h> // Для работы с динамич. памятью и случайных чисел
#include "player.h" // Понадобятся константы оттуда

// ==== Константы ==== //

// Глобальный буфер для сообщений
extern char status_message[256];

// == Карта == //
// Размер
#define MIN_MAP 5
#define MAX_MAP 30
// Вероятности спавна припятствия
#define OBSTACLE_MIN_PERCENT 15
#define OBSTACLE_MAX_PERCENT 30
// Сколько должен пройти игрок, что бы появился портал
#define RING_TRIGGER_COUNT 3
// Базовый множитель
#define BASE_MULTIPLIER 10
// Цена за одну карту в золоте
#define PRICE_ONE_MAP 20

// ==== Структуры ==== //

// Структура для биомов
typedef enum 
{
    BIOME_WOODS = 0, // Редколесье
    BIOME_SWAMP,     // Топь
    BIOME_RUINS,     // Каменные руины
    BIOME_CRYSTAL    // Кристальная пещера
} Biome;

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
    Biome biome;
    int is_mapped; // 0 - не зарисован этаж, 1 - зарисован
} ForestMap;

// ==== Прототипы функций ==== //
char **allocGrid(int h, int w);
void freeGrid(char **g, int h);
void freeForest(ForestMap *m);
int generateForest(ForestMap *m);
void printForest(const ForestMap *m);
void maybeSpawnPortal(ForestMap *m);
int tryMoveForest(ForestMap *m, const char *dir);
void forestRingAdventure(Player *p, GameTime *t);

#endif