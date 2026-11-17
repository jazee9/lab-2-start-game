// Импорты
#include "entity.h"

// Константы
#define MAX_ENTITIES 100          // Максимальное кол-во сущностей на этаже
#define WOOD_STRENGTH 10          // Прочность дерева
#define STONE_STRENGTH 20         // Прочность камня
#define MAX_MONSTERS_ON_FLOOR 3   // Макс. кол-во монстров на этаже
#define MAX_HEALS_ON_FLOOR 2      // Макс. кол-во аптечек на этаже
#define DIFFICULTY_MULTIPLIER 1.5 // Множитель сложности

int generate_entities(ForestMap *m, Entity **entities)
{
    *entities = (Entity *)malloc(MAX_ENTITIES * sizeof(Entity));
    if (*entities == NULL)
        return 0;

    int count = 0;

    // Сканируем карту и создаем сущности для деревьев и камней
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            // Если [T] или [R] (дерево, камень), то создаем
            if (m->cells[y][x] == 'T' || m->cells[y][x] == 'R')
            {
                // Задаем значения
                Entity e;
                e.x = x, e.y = y;
                e.symbol = m->cells[y][x];
                e.is_alive = 1;

                // Если это дерево
                if (e.symbol == 'T')
                {
                    e.type = ENT_WOOD;
                    e.drop_item_id = 1;
                    e.max_hp = WOOD_STRENGTH * m->maps_passed; // Растет с каждым этажом
                    e.attack = 0;
                }
                // Если это камень
                else
                {
                    e.type = ENT_STONE;
                    e.drop_item_id = 2;
                    e.max_hp = STONE_STRENGTH * m->maps_passed; // Растет с каждым этажом
                    e.attack = 0;
                }
                e.hp = e.max_hp;
                (*entities)[count++] = e;
            }
        }
    }

    // Спавним монстров и аптечки
    int spawned_monsters = 0;
    int spawned_heals = 0;
    int tries = 0;

    while ((spawned_heals < MAX_HEALS_ON_FLOOR || spawned_monsters < MAX_MONSTERS_ON_FLOOR) && tries < 1000)
    {
        int x = randInRange(1, m->width - 2);
        int y = randInRange(1, m->height - 2);
        // -2 что бы на краях не спавнились

        // Точка ли там?
        if (m->cells[y][x] == "." && count < MAX_ENTITIES)
        {
            // Спавним монстра
            if (spawned_monsters < MAX_MONSTERS_ON_FLOOR)
            {
                m->cells[y][x] = 'M';
                Entity e;
                e.type = ENT_MONSTER;
                e.x = m->cells[y][x], e.x = m->cells[y][x];
                e.symbol = 'M';
                e.attack = (int)(5 * m->maps_passed * DIFFICULTY_MULTIPLIER);
                e.max_hp = (int)(15 * m->maps_passed * DIFFICULTY_MULTIPLIER);
                e.hp = e.max_hp;
                e.drop_item_id = 0;
                e.is_alive = 1;
                (*entities)[count++] = e;
                spawned_monsters++;
            }
            // Спавним аптечки
            else if (spawned_heals < MAX_HEALS_ON_FLOOR)
            {
                m->cells[y][x] = '+';
                Entity e;
                e.type = ENT_HEAL;
                e.x = m->cells[y][x], e.x = m->cells[y][x];
                e.symbol = '+';
                e.attack = 0;
                e.max_hp = 1, e.hp = 1;
                e.drop_item_id = 0; // Восстанавливает сразу
                e.is_alive = 1;
                (*entities)[count++] = e;
                spawned_heals++;  
            }
        }
    }
    return count; // Возвращаем кол-во заспаненных сущностей
}
