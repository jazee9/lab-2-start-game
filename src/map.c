// ==== Подключение файлов ==== //
#include "map.h"
#include "utils.h"
#include "entity.h"

char status_message[256] = "";

// ================ ФУНКЦИИ ================ //

// ========== РАБОТА С ПАМЯТЬЮ ========== //

// Для выделения двумерного массива
char **allocGrid(int h, int w)
{
    char **g = (char**)malloc((size_t)h * sizeof(char*));
    if (g == NULL) return NULL;

    for (int i = 0; i < h; i++)
    {
         // Для каждой строки выделяется w
        g[i] = (char*)malloc((size_t)w * sizeof(char));
        if (g[i] == NULL)
        {
            for (int j = 0; j < i; j++) free(g[j]);
            free(g);
            return NULL;
        }
    }
    return g; // вовзращаем указатель на массив, если все выделилось
}

// Для освобождения двумерного массива
void freeGrid(char **g, int h)
{
    if (g == NULL) return;

    for (int i = 0; i < h; i++) free(g[i]);
    free(g);
}

// Для освобождения всей карты
void freeForest(ForestMap *m)
{
    if (m == NULL) return;

    // Освобождает двумерный массив cells через freeGrid
    freeGrid(m -> cells, m -> height);
    m -> cells = NULL; // обнуляет указатель после освобождения
}

// ========== РАБОТА С КАРТОЙ ========== //

/* Для проверки хотя бы одного свободного соседа,
что бы игрок и/или костер/портал не оказались замкнутыми припятствиями */
static int hasFreeNeighbor(ForestMap *m, int x, int y) 
{
    // 4 стороны в округ
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    for (int i = 0; i < 4; i++) 
    {
        int nx = x + dx[i];
        int ny = y + dy[i];

        // Если за пределами карты - не считать
        if (nx < 0 || nx >= m -> width || ny < 0 || ny >= m -> height) continue;

        if (m -> cells[ny][nx] == '.') return 1; // Нашли 1 мин.
    }
    return 0;
}

// Функция для заполнения ForestMap и создания нового участка леса
int generateForest(ForestMap *m) 
{
    // Рандомизируем размер карты
    m -> height = randInRange(MIN_MAP, MAX_MAP);
    m -> width = randInRange(MIN_MAP, MAX_MAP);

    // Генерация биома
    m -> biome = randInRange(0, 3);

    // Выделяем память с числами которые выпали для height и width
    m -> cells = allocGrid(m -> height, m -> width);
    if (m -> cells == NULL) return 0;

    // [.] Заполняем карту точками
    for (int count_h = 0; count_h < m -> height; count_h++)
    {
        for (int count_w = 0; count_w < m -> width; count_w++)
        {
            m -> cells[count_h][count_w] = '.';
        }
    }

    // [T] [R] Заполняем случайно деревьями и камнями.
    int pct;

    switch (m -> biome) 
    {
        // На разных биомах - разный % препятствий
        case BIOME_WOODS: pct = randInRange(10, 15); break;
        case BIOME_CRYSTAL: pct = randInRange(15, 20); break;
        case BIOME_RUINS: pct = randInRange(25, 35); break;
        case BIOME_SWAMP: pct = randInRange(20, 25); break;
        default: pct = randInRange(OBSTACLE_MIN_PERCENT, OBSTACLE_MAX_PERCENT);
    }

    for (int y = 0; y < m -> height; y++)
    {
        for (int x = 0; x < m -> width; x++)
        {
            if (rand() % 100 < pct) // Если выпадает клетке шанс получить припятствие, то ставим
            {
                m -> cells[y][x] = (rand() % 2) ? 'T' : 'R'; // Рандом T или R
            }
        }
    }

    // [%] Спавним героя
    int placed = 0; // placed = 0 герой не поставлен, 1 - поставлен

    for (int tries = 0; tries < 2000 && !placed; tries++) 
    {
        // Генерируем случайные координаты для спавна
        int y = randInRange(0, m -> height - 1);
        int x = randInRange(0, m -> width - 1);
        // -1 потому что если например height 12, то x ∈ [0, 11]

        // Проверяем: точка ли там?
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y)) 
        {
            m -> cells[y][x] = '@';

            // Записываем координаты игрока в hero_x, hero_y
            m -> hero_x = x;
            m -> hero_y = y; 
            placed = 1; // Поставили
        }
    }

    // Если не хватило 2000 попыток
    if (!placed) return 0; 

    // [^] Создаем костер, также 
    int camp_ok = 0;
    for (int tries = 0; tries < 1000 && !camp_ok; tries++)
    {
        int y = randInRange(0, m -> height - 1); // y
        int x = randInRange(0, m -> width - 1); // x  

        // Проверка: точка ли там?
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y))
        {
            m -> cells[y][x] = '^';
            camp_ok = 1; // Поставили
        }
    }

    // Если не хватило 1000 попыток
    if (!camp_ok) return 0;

    // Сброс счетчиков
    m -> edges_crossed = 0;
    m -> portal_active = 0;
    m -> on_camp = 0;
    m -> is_mapped = 0;
    return 1;
}

// Для вывода на экран status_message (карта, инструкция по управлению, прочие инструкции)
void printForest(const ForestMap *m) 
{
    const char *biome_name = "Неизвестно";

    // Какой биом?
    if (m -> biome == BIOME_WOODS) biome_name = "Редколесье";
    else if (m -> biome == BIOME_SWAMP) biome_name = "Топь";
    else if (m -> biome == BIOME_RUINS) biome_name = "Каменные руины";
    else if (m -> biome == BIOME_CRYSTAL) biome_name = "Кристальная пещера";

    // Заголовок
    printf("\n--- Локация %s %dx%d. Пройденно карт: %d | Переходов через край: %d ---\n\n", biome_name,
        m -> width, m -> height, m -> maps_passed, m -> edges_crossed);

    for (int y = 0; y < m -> height; y++) 
    {
        for (int x = 0; x < m -> width; x++) 
            putchar(m -> cells[y][x]); // Печатаем putchar'ом по одной
        putchar('\n'); // Перевод строки
    }

    // Инструкция по управлению
    printf("\nW/A/S/D - идти. Q - домой (только на ^). M - зарисовать карту (5 стамины)\n");

    // Вывод гнева природы
    if (m -> total_resources > 0) 
    {
        int wrath_pct = (m->destroyed_resources * 100) / m->total_resources;
        printf("\n[Гнев природы: %d%%] ", wrath_pct);
        if (wrath_pct > 50) printf("[ЛЕС В ЯРОСТИ!]\n");
        else printf("\n");
    }

    // Вывод последнего события
    if (status_message[0] != '\0') {
        printf("\n[Событие]: %s\n", status_message);
    } else {
        printf("\n[Событие]: Ничего не произошло.\n");
    }
    printf("\n");
}

// [P] Для спавна портала на карте
void maybeSpawnPortal(ForestMap *m) 
{
    // Проверки при попадании в эту функцию
    if (m -> portal_active) return; // portal_active = 0? Если 1 - выходим
    if (m -> edges_crossed < RING_TRIGGER_COUNT) return; // Прошел ли игрок 3+ карты? Если нет - выходим

    for (int tries = 0; tries < 2000; tries++)
    {
        // Рандомизируем координаты
        int y = randInRange(0, m -> height - 1); // y
        int x = randInRange(0, m -> width - 1); // x  

        // Проверка: точка ли там?
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y)) 
        {
            m -> cells[y][x] = 'P';
            m -> portal_active = 1;

            // Уведомляем игрока о появлении портала
            strcpy(status_message, "Вы заблудились... На карте появился портал (P)");
            return;
        }
    }
}

// Для передвижения игрока по карте
// 0 — шаг не состоялся или обычный шаг;
// 1 — игрок вошёл в портал (нужен переход на новый этаж).
int tryMoveForest(ForestMap *m, const char *dir)
{
    int dx = 0, dy = 0;

    if (dir[0] == 'W' || dir[0] == 'w' || strncmp(dir, "Ц", 2) == 0 || strncmp(dir, "ц", 2) == 0) dy = -1;
    else if (dir[0] == 'S' || dir[0] == 's' || strncmp(dir, "Ы", 2) == 0 || strncmp(dir, "ы", 2) == 0) dy = 1;
    else if (dir[0] == 'A' || dir[0] == 'a' || strncmp(dir, "Ф", 2) == 0 || strncmp(dir, "ф", 2) == 0) dx = -1;
    else if (dir[0] == 'D' || dir[0] == 'd' || strncmp(dir, "В", 2) == 0 || strncmp(dir, "в", 2) == 0) dx = 1;
    else return 0;

    int nx = m->hero_x + dx;
    int ny = m->hero_y + dy;

    // Заворот карты
    int crossed = 0;
    if (nx < 0)          { nx = m->width  - 1; crossed = 1; }
    if (nx >= m->width)  { nx = 0;             crossed = 1; }
    if (ny < 0)          { ny = m->height - 1; crossed = 1; }
    if (ny >= m->height) { ny = 0;             crossed = 1; }

    char c = m->cells[ny][nx];

    // Этим занимается interact_with_entity
    if (c == 'T' || c == 'R' || c == 'M' || c == '+' || c == 'E') return 0;

    if (crossed) m->edges_crossed++;

    // Стираем героя со старой клетки
    m->cells[m->hero_y][m->hero_x] = m->on_camp ? '^' : '.';
    m->on_camp = 0;

    // Если портал
    if (c == 'P')
    {
        m->cells[ny][nx] = '@';
        m->hero_x = nx;
        m->hero_y = ny;
        return 1;
    }

    // Новые координаты
    m->hero_x = nx;
    m->hero_y = ny;

    // Если костер
    if (c == '^')
    {
        m->on_camp = 1;
        m->cells[ny][nx] = '@';
        strcpy(status_message, "Вы у костра, можно передохнуть (Q - выйти домой)");
    }
    else
    {
        m->cells[ny][nx] = '@';
    }

    maybeSpawnPortal(m);
    return 0;
}

// Главная, запускающая функция
void forestRingAdventure(Player *p, GameTime *t, char item_names[MAX_ITEMS][MAX_NAME_LEN])
{
    // Проверки условий похода
    if (p->stamina <= 0)
    {
        printf("Ваша выносливость на нуле... Лучше лечь спать!\n");
        return;
    }

    if (t->hour >= 2 && t->hour < 8)
    {
        printf("Слишком поздно, лучше лечь спать!\n");
        return;
    }

    // Генерируем первую карту
    ForestMap m;
    memset(&m, 0, sizeof(m));
    m.maps_passed = 1;
    status_message[0] = '\0';
    // Проверка генерации
    if (!generateForest(&m))
    {
        printf("Ошибка генерации леса!\n");
        freeForest(&m);
        return;
    }

    // Генерируем сущности
    Entity *entities = NULL;
    int entity_count = generate_entities(&m, &entities);
    // Проверка генерации
    if (entity_count < 0)
    {
        printf("Ошибка генерации сущностей!\n");
        freeForest(&m);
        return;
    }

    int running = 1;
    system("clear");
    printForest(&m);

    // Запоминаем сколько золота и сколько монстров было убито до похода
    int start_gold = p -> gold;
    int start_killed = p -> monsters_killed;
    int start_resousers = p -> resources_gathered;

    while (running)
    {
        printf("\nHP: %d/%d | Выносливость: %d/%d | Золото: %d | Этаж: %d | Время %d:00\n",
               p->hp, p->max_hp, p->stamina, p->max_stamina,
               p->gold, m.maps_passed, t->hour);

        printf("\nХод: ");
        char ch[10];
        if (fgets(ch, sizeof(ch), stdin) == NULL)
            break;

        status_message[0] = '\0';

        // [M] - Зарисовать этаж
        if (ch[0] == 'M' || ch[0] == 'm' ||
            strncmp(ch, "ь", 2) == 0 || strncmp(ch, "Ь", 2) == 0)
        {
            if (m.is_mapped == 1)
                strcpy(status_message, "Вы уже зарисовали этот этаж!");
            else if (p->stamina < 5)
                strcpy(status_message, "Зарисовать этаж можно за 5 выносливости, у вас меньше!");
            else
            {
                p->stamina -= 5;
                p->maps_drawn++;
                m.is_mapped = 1;
                strcpy(status_message, "Вы зарисовали этот этаж!");
            }

            system("clear");
            printForest(&m);
            continue;
        }

        // [Q] - Выход домой, только на костре
        if (ch[0] == 'Q' || ch[0] == 'q' ||
            strncmp(ch, "Й", 2) == 0 || strncmp(ch, "й", 2) == 0)
        {
            if (m.on_camp == 1)
            {
                system("clear");
                printf("Вы отдохнули у костра и вернулись домой.\n");
                write_adventure_report(p, t->day, m.maps_passed, 0, start_gold, start_killed, start_resousers);
                running = 0;
            }
            else
            {
                strcpy(status_message, "Выйти домой можно только стоя на костре (^).");
                system("clear");
                printForest(&m);
            }
            continue;
        }

        // Направление
        int dx = 0, dy = 0;
        if (ch[0] == 'W' || ch[0] == 'w' || strncmp(ch, "Ц", 2) == 0 || strncmp(ch, "ц", 2) == 0) dy = -1;
        else if (ch[0] == 'S' || ch[0] == 's' || strncmp(ch, "Ы", 2) == 0 || strncmp(ch, "ы", 2) == 0) dy = 1;
        else if (ch[0] == 'A' || ch[0] == 'a' || strncmp(ch, "Ф", 2) == 0 || strncmp(ch, "ф", 2) == 0) dx = -1;
        else if (ch[0] == 'D' || ch[0] == 'd' || strncmp(ch, "В", 2) == 0 || strncmp(ch, "в", 2) == 0) dx = 1;
        else
        {
            system("clear");
            printForest(&m);
            continue;
        }

        // Координаты куда хочет шагнуть
        int target_x = m.hero_x + dx;
        int target_y = m.hero_y + dy;

        // Заворот координат для поиска сущности
        if (target_x < 0)          target_x = m.width  - 1;
        if (target_x >= m.width)   target_x = 0;
        if (target_y < 0)          target_y = m.height - 1;
        if (target_y >= m.height)  target_y = 0;

        // Проверяем на сущность (если сущность - взаимодействуем)
        int interaction = interact_with_entity(p, entities, entity_count,
            target_x, target_y, &m, item_names);

        int res = 0;

        // Если взаимодействия не было
        if (interaction != 0)
        {
            // Если аптечка
            if (interaction == 2)
            {
                m.cells[m.hero_y][m.hero_x] = m.on_camp ? '^' : '.';
                m.on_camp = 0;
                m.hero_x = target_x;
                m.hero_y = target_y;
                m.cells[m.hero_y][m.hero_x] = '@';
            }
        }

        // Обычный шаг
        else
        {
            int old_x = m.hero_x;
            int old_y = m.hero_y;
            res = tryMoveForest(&m, ch);

            if (m.hero_x != old_x || m.hero_y != old_y)
            {
                int stamina_cost = (m.biome == BIOME_SWAMP) ? 2 : 1;
                p->stamina -= stamina_cost;
                if (p->stamina < 0) p->stamina = 0;
            }
        }

        check_nature_wrath(&m, &entities, &entity_count); // Проверка на Гнев Природы / x2 аптечки

        // [P] - Переход на новый этаж
        if (res == 1)
        {
            if (p->stamina < 5)
            {
                system("clear");
                printf("\nУ вас не хватило сил на переход! Вы падаете без сил...\n");
                p->stamina = 0;
                break;
            }

            // Проверка на то как пройден этаж - зачищено < или > 10% (надо доп аптечки?)
            int gentle = 0;
            if (m.total_resources > 0 &&
                m.destroyed_resources * 100 <= m.total_resources * 10) gentle = 1;
            if (gentle) m.heal_bonus_next = MAX_HEALS_ON_FLOOR;
            else m.heal_bonus_next = 0;

            m.maps_passed++;
            p->stamina -= 5;
            t->hour += 1;
            if (p->stamina < 0) p->stamina = 0;

            int gold_reward = BASE_MULTIPLIER * m.maps_passed;
            if (m.biome == BIOME_CRYSTAL) gold_reward *= 3;
            p->gold += gold_reward;

            if (entities != NULL)
            {
                free_entities(entities, entity_count);
                entities = NULL;
                entity_count = 0;
            }

            freeForest(&m);
            if (!generateForest(&m))
            {
                printf("Ошибка генерации леса.\n");
                write_adventure_report(p, t->day, m.maps_passed, 1, start_gold, start_killed, start_resousers);
                return;
            }

            entity_count = generate_entities(&m, &entities);

            if (entity_count < 0)
            {
                printf("Ошибка генерации сущностей на новом этаже!\n");
                freeForest(&m);
                write_adventure_report(p, t->day, m.maps_passed, 1, start_gold, start_killed, start_resousers);
                return;
            }
        }

        // Обморок (Экстренная эвакуация)
        if (p->stamina <= 0 || (t->hour >= 2 && t->hour < 8))
        {
            system("clear");
            printf("\nВы падаете без сил! Спасатели забрали половину золота.\n");
            p->gold /= 2;
            p->hp = 1;
            write_adventure_report(p, t->day, m.maps_passed, 1, start_gold, start_killed, start_resousers);
            break;
        }

        // Сдох с монстром
        if (p->hp <= 0)
        {
            system("clear");
            printf("\nВы погибли в бою! Спасатели забрали половину золота и доставили вас домой.\n");
            p->gold /= 2;
            p->hp = 1;
            write_adventure_report(p, t->day, m.maps_passed, 1, start_gold, start_killed, start_resousers);
            break;
        }

        system("clear");
        printForest(&m);
    }

    freeForest(&m);
    free_entities(entities, entity_count);
}