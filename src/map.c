// Файл для всего, что связано с приключением и картой

// ==== Подключение файлов ==== //
#include "map.h"

// Для вывода сообщений
char status_message[256] = "";

// ================ ФУНКЦИИ ================ //

// ДЛЯ ВЫДЕЛЕНИЯ ДВУМЕРНОГО МАССИВА
char **allocGrid(int h, int w) // Принимает высоту и ширину карты
{
    char **g = (char**)malloc((size_t)h * sizeof(char*)); // выделяем массив из h-указателей

    if (g == NULL) return NULL; // Проверяет выделилась ли память, если нет - сразу выходим NULL

    for (int i = 0; i < h; i++) // Перебор строк
    {
        g[i] = (char*)malloc((size_t)w * sizeof(char)); // Для каждой строки выделяется w

        if (g[i] == NULL) // И опять проверка
        {
            for (int j = 0; j < i; j++) free(g[j]); // Если очередная строка не выделилась — откат всего
            free(g); // Освобождаем массив указателей
            return NULL;
        }
    }
    return g; // вовзращаем указатель на массив, если все выделилось
}

// ДЛЯ ОСВОБОЖДЕНИЯ ДВУМЕРНОГО МАССИВА
void freeGrid(char **g, int h)
{
    if (g == NULL) return; // Проверка есть ли, что освобождать

    // Если есть что
    for (int i = 0; i < h; i++) free(g[i]); // Освобождаем каждую строку
    free(g); // Освобождаем массив указателей
}

// ДЛЯ ОСВОБОЖДЕНИЯ ВСЕЙ КАРТЫ
void freeForest(ForestMap *m)
{
    if (m == NULL) return; // Защита от NULL структуры

    freeGrid(m -> cells, m -> height); // Освобождает двумерный массив cells через freeGrid
    m -> cells = NULL; // обнуляет указатель после освобождения
}

/* ДЛЯ ПРОВЕРКИ ХОТЯ БЫ ОДНОГО СВОБОДНОГО СОСЕДА, 
что бы игрок и/или костер/портал не оказались замкнутыми припятствиями*/
static int hasFreeNeighbor(ForestMap *m, int x, int y) 
{
    // 4 стороны в округ
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    // Перебираем каждую сторону и смотрим
    for (int i = 0; i < 4; i++) 
    {
        // Координаты соседов
        int nx = x + dx[i];
        int ny = y + dy[i];

        // Если сосед за пределаи карты - не считать
        if (nx < 0 || nx >= m -> width || ny < 0 || ny >= m -> height) continue;

        // Если есть хотя бы 1 сосед [.] - выходим из hasFreeNeighbor с 1 - успех
        if (m -> cells[ny][nx] == '.') return 1;
    }
    return 0; // Если в округ припятствия, после того как все 4 просмотрели
}

// ФУНКЦИЯ ДЛЯ ЗАПОЛНЕНИЯ ForestMap И СОЗДАНИЯ НОВОГО УЧАСТКА ЛЕСА
int generateForest(ForestMap *m) 
{
    // Размеры карты, может быть не квадрат! Используем функцию randInRange
    // Может выпасть например h = 12, w = 27 (12x27 карта)
    m -> height = randInRange(MIN_MAP, MAX_MAP); // Для высоты
    m -> width = randInRange(MIN_MAP, MAX_MAP); // Для ширины

    // Выделяем память с числами которые выпали для height и width
    m -> cells = allocGrid(m -> height, m -> width); // Закидываем в allocGrid
    if (m -> cells == NULL) return 0; // Если что-то не сработало

    // [.] Заполняем карту точками
    for (int count_h = 0; count_h < m -> height; count_h++)  // Перебор строк до последней
    {
        for (int count_w = 0; count_w < m -> width; count_w++) // Перебор столбцов до последнего
        {
            m -> cells[count_h][count_w] = '.'; // Заполняем точками
        }
    }

    // [T] [R] Заполняем случайно деревьями и камнями. (T) и (R)
    int pct = randInRange(OBSTACLE_MIN_PERCENT, OBSTACLE_MAX_PERCENT); // Cлучайный % (от 15 до 30)

    for (int y = 0; y < m -> height; y++) // Перебор строк до последней
    {
        for (int x = 0; x < m -> width; x++) // Перебор столбцов до последнего
        {
            if (rand() % 100 < pct) // Если выпадает клетке шанс получить припятствие, то ставим
            {
                m -> cells[y][x] = (rand() % 2) ? 'T' : 'R'; // Рандомим в нее T или R
            }
        }
    }

    // [%] Спавним героя
    int placed = 0; // placed = 0 герой не поставлен, 1 - посвлен

    // Пока !tries (то есть не поставлен персонаж) и tries < 2000 (ограничение)
    /* Ограничение, потому что вся карта мб будет в T и R (не ну а чо нет) и  
    тогда ставить некуда будет перса. Поэтому ограничение, что бы не упала программа */ 
    for (int tries = 0; tries < 2000 && !placed; tries++) 
    {
        // Генерируем случайные координаты для спавна игрока
        int y = randInRange(0, m -> height - 1); // x
        int x = randInRange(0, m -> width - 1); // y
        // -1 потому что если например height 12, то x ∈ [0, 11]

        // Проверяем нет ли там припятствия T или R (точка ли там?)
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y)) 
        {
            m -> cells[y][x] = '@'; // Если '.', то ставим героя

            // Записываем координаты игрока в hero_x, hero_y
            m -> hero_x = x;
            m -> hero_y = y; 
            placed = 1; // Поставили
        }
    }

    // Если не хватило 2000 попыток, то возвращаем 0 (не, ну а вдруг)
    if (!placed) return 0; 

    // [^] Создаем костер. Собственно также
    // ==== КОСТЕР СОЗДАЕТСЯ ПОСЛЕ СПАВНА ГЕРОЯ ЧТО БЫ ОН НЕ ПОПАЛ НА ГЕРОЯ СРАЗУ === // 
    int camp_ok = 0;
    for (int tries = 0; tries < 1000 && !camp_ok; tries++) // Также
    {
        // Рандомом генерируем координаты
        int y = randInRange(0, m -> height - 1); // y
        int x = randInRange(0, m -> width - 1); // x  

        // Проверяем нет ли там припятствия T или R (точка ли там?)
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y))
        {
            m -> cells[y][x] = '^'; // Если '.', то ставим костер
            camp_ok = 1; // Поставили
        }
    }
    // Если не хватило 1000 попыток, то возвращаем 0 (не, ну а вдруг)
    if (!camp_ok) return 0;

    // Сброс счетчиков
    m -> edges_crossed = 0;
    m -> portal_active = 0;
    m -> on_camp = 0;
    return 1;
}

// ДЛЯ ВЫВОДА НА ЭКРАН: карта, инструкция по управлению, прочие инструкции (status_message)
void printForest(const ForestMap *m) 
{
    // Заголовок
    printf("\n--- Локация Лес-кольцо %dx%d. Пройденно карт: %d | Переходов через край: %d ---\n\n", 
        m -> width, m -> height, m -> maps_passed, m -> edges_crossed);

    // Перебираем
    for (int y = 0; y < m -> height; y++) 
    {
        for (int x = 0; x < m -> width; x++) 
            putchar(m -> cells[y][x]); // Печатаем putchar'ом по одной
        putchar('\n'); // Перевод строки
    }

    // Инструкция по управлению
    printf("\nW/A/S/D - идти. Q - домой (только на ^)\n");

    // Вывод подсказок под картой
    if (status_message[0] != '\0') {
        printf("\n>>> %s <<<\n", status_message);
    }
    printf("\n");
}

// ДЛЯ СПАВНА ПОРТАЛА НА КАРТЕ
void maybeSpawnPortal(ForestMap *m) 
{
    // Проверки при попадании в эту функцию
    if (m -> portal_active) return; // portal_active = 0? Если 1 - выходим
    if (m -> edges_crossed < RING_TRIGGER_COUNT) return; // Прошел ли игрок 3+ карты? Если нет - выходим

    for (int tries = 0; tries < 2000; tries++)  // 2000 попыток спавна портала
    {
        // Рандомизируем координаты
        int y = randInRange(0, m -> height - 1); // y
        int x = randInRange(0, m -> width - 1); // x  

        // Проверяем точка ли там
        if (m -> cells[y][x] == '.' && hasFreeNeighbor(m, x, y)) 
        {
            m -> cells[y][x] = 'P'; // Ставим P, ели свободно
            m -> portal_active = 1; // Меняем m -> portal_active на 1

            // Уведомляем игрока о появлении портала
            strcpy(status_message, "Вы заблудились... На карте появился портал (P)");
            return;
        }
    }
}

// ДЛЯ ПЕРЕДВИЖЕНИЯ ИГРОКА ПО КАРТЕ
int tryMoveForest(ForestMap *m, const char *dir) 
{
    // Для подсчетов
    int dx = 0;
    int dy = 0;

    // Биндим кнопки
    if (dir[0] == 'W' || dir[0] == 'w' || strncmp(dir, "Ц", 2) == 0 || 
        strncmp(dir, "ц", 2) == 0) dy = -1; // Вверх
    else if (dir[0] == 'S' || dir[0] == 's' || strncmp(dir, "Ы", 2) == 0 || 
        strncmp(dir, "ы", 2) == 0) dy = 1; // Вниз
    else if (dir[0] == 'A' || dir[0] == 'a' || strncmp(dir, "Ф", 2) == 0 || 
        strncmp(dir, "ф", 2) == 0) dx = -1; // Влево
    else if (dir[0] == 'D' || dir[0] == 'd' || strncmp(dir, "В", 2) == 0 || 
        strncmp(dir, "в", 2) == 0) dx = 1; // Вправо
    else return 0; // Если нажал не то, ничего не происходит

    // Новая координата
    int nx = m -> hero_x + dx;
    int ny = m -> hero_y + dy;

    // 0 - не вышел за край, 1 - вышел за край
    int crossed = 0;

    // выходы за края
    if (nx < 0) {nx = m -> width - 1; crossed = 1;} // Если вышел за ЛЕВЫЙ, тепаем у ПРАВОГО 
    if (nx >= m -> width) {nx = 0; crossed = 1;} // Если вышел за ПРАВЫЙ, тепаем у ЛЕВОГО 
    if (ny < 0) {ny = m -> height - 1; crossed = 1;} // Если вышел за ВЕРХНИЙ, тепаем у НИЖНЕГО 
    if (ny >= m -> height) {ny = 0; crossed = 1;} // Если вышел за НИЖНИЙ, тепаем у ВЕРХНЕГО
    
    // Узнаем, что в той клетке, в которую идем
    char c = m -> cells[ny][nx];

    // Если припятствие [T] [R]
    if (c == 'T' || c == 'R') 
    {
        strcpy(status_message, "Туда нельзя, препятствие!");
        return 0;
    } 

    // Переходы через край
    if (crossed) 
    {
        m -> edges_crossed++;
        sprintf(status_message, "Вы пересекли край леса (переходов: %d)", m->edges_crossed);
    }

    // Если перешел, то стираем героя со старой клетки и возвращаем туда [.] или [^]
    m -> cells[m -> hero_y][m -> hero_x] = m -> on_camp ? '^' : '.';
    m -> on_camp = 0; // Если был на костре и ушел с него

    // Если портал [P]
    if (c == 'P') 
    {
        m -> cells[ny][nx] = '@'; // Ставим героя
        m -> hero_x = nx; m -> hero_y = ny; // Обнуляем координаты, тк будет новая карта
        return 1;
    }

    // После проверки припятствий и прочего
    m -> hero_x = nx; m -> hero_y = ny;

    if (c == '^')  // Если там костер
    {
        m -> on_camp = 1;
        m -> cells[ny][nx] = '@';

        strcpy(status_message, "Вы у костра, можно передохнуть (Q - выйти домой)");
    }

    else {m -> cells[ny][nx] = '@';} // Если там ничего

    maybeSpawnPortal(m);
    return 0;
}

// [case: 10] ГЛАВНАЯ ЗАПУСКАЮЩАЯ ФУНКЦИЯ
void forestRingAdventure(void) 
{
    ForestMap m; // Запускает структуру
    memset(&m, 0, sizeof(m)); // Дает 0 всем в структуре
    m.maps_passed = 1; 
    status_message[0] = '\0'; // Отчистка буфера

    if (!generateForest(&m)) 
    {
        printf("Ошибка генерации леса!\n");
        freeForest(&m); 
        return;
    }

    int running = 1;
    system("clear");
    printForest(&m); // Рисуем первую карту

    while (running) 
    {
        // Читаем ход (карта уже на экране, сообщений нет)
        printf("\nХод: ");
        char ch[10];
        if (fgets(ch, sizeof(ch), stdin) == NULL) break;

        status_message[0] = '\0'; // Очищаем сообщение перед ходом

        // [Q] (выход)
        if (ch[0] == 'Q' || ch[0] == 'q' || (strncmp(ch, "Й", 2) == 0) || (strncmp(ch, "й", 2) == 0))  
        {
            if (m.on_camp == 1) // На костре?
            {
                printf("Вы отдохнули у костра и вернулись домой.\n");
                running = 0; 
            }
            else 
            {
                strcpy(status_message, "Выйти домой можно только стоя на костре (^).");

                system("clear");
                printForest(&m);
            } 
            continue; // Не перерисовываем карту, просто покажем сообщение (или выйдем)
        }

        // Движение
        int res = tryMoveForest(&m, ch);

        if (res) 
        {
            m.maps_passed++; 
            freeForest(&m); 
            if (!generateForest(&m)) {printf("Ошибка генерации.\n"); break;}
        }

        // Очищаем экран и рисуем карту + новое сообщение (если есть)
        system("clear");
        printForest(&m);
    } 
    freeForest(&m); 
}