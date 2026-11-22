#ifndef MAP_H
#define MAP_H

// ==== Импорт ==== //
#include "player.h" // Понадобятся константы оттуда

// ==== Константы ==== //

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
    int height;               // Высота карты
    int width;                // Ширина карты
    int hero_x, hero_y;       // Cтолбец и строка героя
    char **cells;             // Карта в динамической памяти
    int edges_crossed;        // Сколько раз герой вышел за край
    int portal_active;        // Активация портала. 0 - портала нет, 1 - есть
    int on_camp;              // Персонаж стоит на костре. 1 - стоит, 0 - стоит на обычной клетке (не костер)
    int maps_passed;          // Счетчик дальности от дома (сколько пройденно карт)
    Biome biome;
    int is_mapped;            // 0 - не зарисован этаж, 1 - зарисован
    int total_resources;      // Всего деревьев/камней на этаже
    int destroyed_resources;  // Сколько срублено/убито
    int angry_ents_spawned;   // 0 - энтов нет, 1 - уже пришли
    int heal_bonus_next;      // Бонус к аптечкам на след. этаже
    int floor_was_gentle;     // 1 — текущий этаж пройден бережно (<10%), 0 — нет
    char status_message[256]; // Сообщение о последнем событии
} ForestMap;

// ==== Прототипы функций ==== //
char **allocGrid(int h, int w);
void freeGrid(char **g, int h);
void freeForest(ForestMap *m);
int generateForest(ForestMap *m);
void printForest(const ForestMap *m);
void maybeSpawnPortal(ForestMap *m);
int tryMoveForest(ForestMap *m, const char *dir);
void forestRingAdventure(Player *p, GameTime *t, char item_names[MAX_ITEMS][MAX_NAME_LEN]);

#endif