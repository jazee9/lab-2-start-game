// Импорты
#include "entity.h"

// Константы
#define MAX_ENTITIES 100              // Максимальное кол-во сущностей на этаже
#define WOOD_STRENGTH 5              // Прочность дерева (на первом этаже)
#define STONE_STRENGTH 20             // Прочность камня (на первом этаже)
#define REWARD_FOR_MONSTER_KILL 20    // Базовая награда золотом за убийство монстра
#define MAX_MONSTERS_ON_FLOOR 3       // Макс. кол-во монстров на этаже
#define MAX_HEALS_ON_FLOOR 2          // Макс. кол-во аптечек на этаже
#define DIFFICULTY_MULTIPLIER 1.5     // Множитель сложности
#define HOW_MUCH_HEAL_PLUS_STAMINA 25 // На сколько увеличив. аптечка выносливость

// Функция для генерации сущностей
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
        if (m->cells[y][x] == '.' && count < MAX_ENTITIES)
        {
            // Спавним монстра
            if (spawned_monsters < MAX_MONSTERS_ON_FLOOR)
            {
                m->cells[y][x] = 'M';
                Entity e;
                e.type = ENT_MONSTER;
                e.x = x, e.y = y;
                e.symbol = 'M';
                e.attack = (int)(5 * m->maps_passed * DIFFICULTY_MULTIPLIER);
                e.max_hp = (int)(15 * m->maps_passed * DIFFICULTY_MULTIPLIER);
                e.defense = 2 * m->maps_passed;
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
                e.x = x, e.y = y;
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

// Функция для очистки памяти
void free_entities(Entity *entities, int count)
{
    if (entities != NULL)
    {
        printf("Память массива сущностей по адресу %p освобождена\n", (void *)entities);
        free(entities);
    }
}

// Функция для добавления выпадшего предмета в рюкзак
// Кладет в первый пустой слот
static int add_to_inventory(Player *p, int item_id)
{
    if (item_id < 0)
        return 1; // Если предмет не должен выпадать

    for (int i = 0; i < SIZE; i++)
    {
        if (p->inventory[i] == 0)
        {
            p->inventory[i] = item_id;
            return 1;
        }
    }
    return 0; // Если инвентарь переполнен
}

// Функция для взаимодействия игрока с сущностями
int interact_with_entity(Player *p, Entity *entities, int entity_count, int target_x, int target_y, ForestMap *m, char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    for (int i = 0; i < entity_count; i++)
    {
        // Если нашли сущность и она жива
        if (entities[i].is_alive && entities[i].x == target_x && entities[i].y == target_y)
        {
            // Рубка ресурсов (Дерево, камень)
            if (entities[i].type == ENT_WOOD || entities[i].type == ENT_STONE)
            {
                p->stamina -= 2; // Тратиться у игрока на удар
                int damage = p->attack;
                entities[i].hp -= damage; // Тратится хп у объекта

                char msg[256];

                // Если закончилось хп у объекта
                if (entities[i].hp <= 0)
                {
                    entities[i].is_alive = 0;
                    m->cells[target_y][target_x] = '.';

                    // Уведомляем игрока, что он разрушил объект и предмет добавлен в рюкзак
                    if (add_to_inventory(p, entities[i].drop_item_id))
                    {
                        sprintf(status_message, "Вы разрушили объект и получили: %s!", item_names[entities[i].drop_item_id]);
                    }
                    // Если у него забит рюкзак
                    else
                    {
                        sprintf(status_message, "Ваш рюкзак переполон! Предмет пропал.");
                    }
                }
                // Если хп еще не закончилось у объекта
                else
                {
                    sprintf(status_message, "Вы ударили %s на %d урона (Осталось хп: %d/%d)", item_names[entities[i].drop_item_id], damage, entities[i].hp, entities[i].max_hp);
                }

                return 1;
            }

            // Драка с монстром
            if (entities[i].type == ENT_MONSTER)
            {
                // Бъет игрок
                int player_damage = p->attack - (entities[i].defense / 2); // Формула
                if (player_damage < 1)
                    player_damage = 1;
                entities[i].hp -= player_damage;
                p->stamina -= 2;

                char msg[256];

                // Если он сдох
                if (entities[i].hp <= 0)
                {
                    entities[i].is_alive = 0;
                    m->cells[target_y][target_x] = '.';
                    int reward = REWARD_FOR_MONSTER_KILL * m->maps_passed;
                    p->gold += reward;

                    sprintf(status_message, "Вы победили монстра! Нанесли %d урона и получили %d золота.", player_damage, reward);
                }
                // Если он жив, то бьет в ответ
                else
                {
                    int monster_damage = entities[i].attack - (p->defense / 2);
                    if (monster_damage < 1)
                        monster_damage = 1;
                    p->hp -= monster_damage;

                    sprintf(status_message, "Вы ударили монстра на %d. Монстр ударил вас на %d (Ваше HP: %d, HP монстра: %d)",
                            player_damage, monster_damage, p->hp, entities[i].hp);
                }
                return 1;
            }

            // Сбор аптечки [+]
            if (entities[i].type == ENT_HEAL)
            {
                entities[i].is_alive = 0;
                m->cells[target_y][target_x] = '.';
                p->hp += 20;
                if (p->hp > p->max_hp)
                    p->hp = p->max_hp;
                p->stamina += 20;
                if (p->stamina > p->max_stamina)
                    p->stamina = p->max_stamina;

                strcpy(status_message, "Вы съели ягоды и восстановили HP и выносливость!");
                return 0;
            }
        }
    }
    return 0; // Сущностей нет
}