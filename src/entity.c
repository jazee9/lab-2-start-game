// Импорты
#include "entity.h"

// Константы
#define MAX_ENTITIES 100              // Максимальное кол-во сущностей на этаже
#define MAX_HEALS_ON_FLOOR 2          // Макс. кол-во аптечек на этаже
#define MAX_MONSTERS_ON_FLOOR 3       // Макс. кол-во монстров на этаже
#define WOOD_STRENGTH 5               // Прочность дерева (на первом этаже)
#define STONE_STRENGTH 20             // Прочность камня (на первом этаже)
#define REWARD_FOR_MONSTER_KILL 20    // Базовая награда золотом за убийство монстра
#define DIFFICULTY_MULTIPLIER 0.7     // Множитель сложности
#define HOW_MUCH_HEAL_PLUS_STAMINA 40 // На сколько увеличив. аптечка выносливость
#define TROPHY_DROP_CHANCE 10         // Шанс выпадения трофея с убийства монстра в %
// ID
#define TROPHY_ITEM_ID 9              // ID трофейного предмета
#define ID_WOOD 1                     // ID дерева
#define ID_STONE 2                    // ID камня

// Функция для генерации сущностей
int generate_entities(ForestMap *m, Entity **entities)
{
    *entities = NULL;

    // Считаем сколько [T] и [R]
    int obstacle_count = 0;
    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            if (m->cells[y][x] == 'T' || m->cells[y][x] == 'R')
                obstacle_count++;
        }
    }

    // Выделяем память
    int capacity = obstacle_count + MAX_MONSTERS_ON_FLOOR + MAX_HEALS_ON_FLOOR + m -> heal_bonus_next;
    if (capacity < 1) capacity = 1;

    *entities = (Entity *)malloc((size_t)capacity * sizeof(Entity));
    if (*entities == NULL)
        return -1;

    // Создаем сущности для деревьев и камней
    int total_res = 0; // Счётчик ресурсов
    int count = 0;

    for (int y = 0; y < m->height; y++)
    {
        for (int x = 0; x < m->width; x++)
        {
            if (m->cells[y][x] == 'T' || m->cells[y][x] == 'R')
            {
                total_res++;
                Entity e;
                e.x = x;
                e.y = y;
                e.symbol = m->cells[y][x];
                e.is_alive = 1;
                e.defense = 0;
                e.attack = 0;

                // Если это дерево
                if (e.symbol == 'T') {
                    e.type = ENT_WOOD;
                    e.drop_item_id = ID_WOOD;
                    e.max_hp = WOOD_STRENGTH * m->maps_passed;
                } 
                // Если камень
                else {
                    e.type = ENT_STONE;
                    e.drop_item_id = ID_STONE;
                    e.max_hp = STONE_STRENGTH * m->maps_passed;
                }
                e.hp = e.max_hp;
                (*entities)[count++] = e;
            }
        }
    }

    // Доп. аптечки
    int heals_to_spawn = MAX_HEALS_ON_FLOOR + m -> heal_bonus_next;

    // Сброс счетчиков
    m->total_resources = total_res;
    m->destroyed_resources = 0;
    m->angry_ents_spawned = 0;
    m->floor_was_gentle = 0;
    m->heal_bonus_next = 0;   

    // Спавним монстров
    int spawned_monsters = 0;
    int spawned_heals = 0;
    int tries = 0;

    while ((spawned_heals < heals_to_spawn ||
            spawned_monsters < MAX_MONSTERS_ON_FLOOR)
           && tries < 5000) // 5000 попыток на спавн
    {
        tries++;

        int x = randInRange(1, m->width - 2);
        int y = randInRange(1, m->height - 2);

        if (m->cells[y][x] != '.')
            continue;

        if (count >= capacity)
            break;

        if (spawned_monsters < MAX_MONSTERS_ON_FLOOR)
        {
            m->cells[y][x] = 'M';

            Entity e;
            e.type = ENT_MONSTER;
            e.x = x;
            e.y = y;
            e.symbol = 'M';
            e.attack  = (int)(2 + 2 * m->maps_passed * DIFFICULTY_MULTIPLIER);
            e.max_hp  = (int)(8 + 4 * m->maps_passed * DIFFICULTY_MULTIPLIER);
            e.defense = (int)(1 + 1 * m->maps_passed * DIFFICULTY_MULTIPLIER);
            e.hp = e.max_hp;
            e.drop_item_id = 0;
            e.is_alive = 1;

            (*entities)[count++] = e;
            spawned_monsters++;
        }

        // Спавним аптечку
        else if (spawned_heals < heals_to_spawn)
        {
            m->cells[y][x] = '+';

            Entity e;
            e.type = ENT_HEAL;
            e.x = x;
            e.y = y;
            e.symbol = '+';
            e.attack = 0;
            e.defense = 0;
            e.max_hp = 1;
            e.hp = 1;
            e.drop_item_id = 0;
            e.is_alive = 1;

            (*entities)[count++] = e;
            spawned_heals++;
        }
    }

    // Если не получилось, что то за спавнить return -1
    if (spawned_monsters < MAX_MONSTERS_ON_FLOOR ||
        spawned_heals < heals_to_spawn)
    {
        printf("\nОшибка генерации, попробуйте снова!\n");
        free(*entities);
        *entities = NULL;
        return -1;
    }
    
    return count;
}

// Функция для очистки памяти
void free_entities(Entity *entities)
{
    if (entities != NULL)
    {
        printf("\nПамять массива сущностей по адресу %p освобождена\n", (void *)entities);
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

                // Если закончилось хп у объекта
                if (entities[i].hp <= 0)
                {
                    m -> destroyed_resources++;
                    entities[i].is_alive = 0;
                    m->cells[target_y][target_x] = '.';
                    p->resources_gathered++;

                    // Уведомляем игрока, что он разрушил объект и предмет добавлен в рюкзак
                    if (add_to_inventory(p, entities[i].drop_item_id))
                    {
                        sprintf(m->status_message, "Вы разрушили объект и получили: %s!", item_names[entities[i].drop_item_id]);
                    }
                    // Если у него забит рюкзак
                    else
                    {
                        sprintf(m->status_message, "Ваш рюкзак переполнен! Предмет пропал.");
                    }
                }
                // Если хп еще не закончилось у объекта
                else
                {
                    sprintf(m->status_message, "Вы ударили %s на %d урона (Осталось хп: %d/%d)", item_names[entities[i].drop_item_id], damage, entities[i].hp, entities[i].max_hp);
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

                // Если он сдох
                if (entities[i].hp <= 0)
                {
                    entities[i].is_alive = 0;
                    m -> cells[target_y][target_x] = '.';
                    int reward = REWARD_FOR_MONSTER_KILL * m -> maps_passed;
                    if (entities[i].symbol == 'E') reward += 50; // C Энта на 50 больше золота
                    p -> gold += reward;
                    p -> monsters_killed++;

                    // Счетчик для гнева природы увеличиваться если был убит не Энт
                    if (entities[i].symbol != 'E') m -> destroyed_resources++;

                    // Трофей
                    int trophy_found = 0;

                    if (rand() % 100 <= TROPHY_DROP_CHANCE) 
                    {
                        if (add_to_inventory(p, TROPHY_ITEM_ID)) 
                        {
                            trophy_found = 1;
                        }
                    }

                    // Если выпал
                    if (trophy_found) 
                    {
                        // Если Энт
                        if (entities[i].symbol == 'E') 
                            sprintf(m->status_message, "Вы победили Энта! Получено %d золота. Трофей: %s!", 
                                reward, item_names[TROPHY_ITEM_ID]);

                        else sprintf(m->status_message, "Вы победили монстра! Получено %d золота. Трофей: %s!", 
                                reward, item_names[TROPHY_ITEM_ID]);
                    } 
                    // Если нет(
                    else 
                    {
                        if (entities[i].symbol == 'E')
                            sprintf(m->status_message, "Вы победили Энта и получили %d золота.", 
                                reward);
                        else sprintf(m->status_message, "Вы победили монстра и получили %d золота.", 
                                reward);
                    }
                }

                // Если он жив, то бьет в ответ
                else
                {
                    int monster_damage = entities[i].attack - (p->defense / 2);
                    if (monster_damage < 1)
                        monster_damage = 1;
                    p->hp -= monster_damage;

                    sprintf(m->status_message, "Вы ударили монстра на %d. Монстр ударил вас на %d (Ваше HP: %d, HP монстра: %d)",
                            player_damage, monster_damage, p->hp, entities[i].hp);
                }
                return 1;
            }

            // Сбор аптечки [+]
            if (entities[i].type == ENT_HEAL)
            {
                entities[i].is_alive = 0;
                m->cells[target_y][target_x] = '.';
                p->hp += 40;
                if (p->hp > p->max_hp)
                    p->hp = p->max_hp;
                p->stamina += HOW_MUCH_HEAL_PLUS_STAMINA;
                if (p->stamina > p->max_stamina)
                    p->stamina = p->max_stamina;

                strcpy(m->status_message, "Вы съели ягоды и восстановили HP и выносливость!");
                return 2;
            }
        }
    }
    return 0; // Сущностей нет
}

// Функция для проверки гнева природы
void check_nature_wrath(ForestMap *m, Entity **entities, int *entity_count) 
{
    if (m -> angry_ents_spawned || m -> total_resources == 0) return;

    // Если зачищенно более 50% ресурсов на этаже
    // Убито больше чем было/2
    if (!m->angry_ents_spawned &&
        m -> destroyed_resources > (m->total_resources / 2))
    {
        m -> angry_ents_spawned = 1;

        for (int i = 0; i < 2; i++) 
        {
            for (int tries = 0; tries < 100; tries++) 
            {
                int x = randInRange(1, m->width - 2);
                int y = randInRange(1, m->height - 2);

                if (m -> cells[y][x] == '.') 
                {
                    // Увеличиваем массив сущностей перед добавлением Энтов
                    Entity *new_entities = (Entity *)realloc(*entities, 
                        (size_t)(*entity_count + 1) * sizeof(Entity));
                    if (new_entities == NULL) break;
                    *entities = new_entities;

                    m -> cells[y][x] = 'E'; // Спавним энта
                    Entity e;
                    e.type = ENT_MONSTER;
                    e.symbol = 'E';
                    e.x = x;
                    e.y = y;
                    e.attack  = 8 * m->maps_passed;
                    e.max_hp  = 10 * m->maps_passed;
                    e.hp = e.max_hp;
                    e.defense = 2 * m->maps_passed;
                    e.drop_item_id = 0;
                    e.is_alive = 1;
                    (*entities)[(*entity_count)++] = e;                    
                    break;
                }
            }
        }
    }
}