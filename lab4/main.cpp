#include "hash_table.h"
#include <cstdio>
#include <string>

using namespace HashNS;
static void flushInputLine() {
    int c;
    while ((c = std::getchar()) != '\n' && c != EOF) {}
}

static std::string readLine(const char* prompt) {
    std::printf("%s", prompt);
    std::fflush(stdout);
    char buf[256] = {};
    if (std::fgets(buf, sizeof(buf), stdin)) {
        std::string s(buf);
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
        return s;
    }
    return "";
}

static void loadDemoData(HashTable& ht) {
    ht.insert("Автобус",     "Безрельсовое многоместное ТС");
    ht.insert("Автомобиль",  "Самоходное колёсное ТС");
    ht.insert("Аэропорт",    "Комплекс для взлёта и посадки судов");
    ht.insert("Трамвай",     "Городской рельсовый электротранспорт");
    ht.insert("Троллейбус",  "Электротранспорт без рельсов");
    ht.insert("Такси",       "Перевозка пассажиров по заказу");
    ht.insert("Метро",       "Подземный скоростной транспорт");
    ht.insert("Паром",       "Судно для переправы через воду");
    ht.insert("Велосипед",   "Двухколёсное ТС с педалями");
    ht.insert("Поезд",       "Состав вагонов с локомотивом");
    ht.insert("Корабль",     "Крупное морское или речное судно");
    ht.insert("Самолёт",     "Летательный аппарат тяжелее воздуха");
    ht.insert("Фуникулёр",   "Канатная дорога на склонах");
}

static void printMenu() {
    std::printf("\n╔══════════════════════════════════════╗\n");
    std::printf("║   Хеш-таблица — Тематика: Транспорт ║\n");
    std::printf("╠══════════════════════════════════════╣\n");
    std::printf("║  1. Вывести таблицу                  ║\n");
    std::printf("║  2. Вставить запись                  ║\n");
    std::printf("║  3. Найти запись                     ║\n");
    std::printf("║  4. Обновить запись                  ║\n");
    std::printf("║  5. Удалить запись                   ║\n");
    std::printf("║  6. Коэффициент заполнения           ║\n");
    std::printf("║  7. Показать V и h для ключа         ║\n");
    std::printf("║  0. Выход                            ║\n");
    std::printf("╚══════════════════════════════════════╝\n");
    std::printf("Выбор: ");
    std::fflush(stdout);
}

int main() {
    HashTable ht;
    loadDemoData(ht);
    std::printf("Демонстрационные данные загружены (13 терминов).\n");

    bool running = true;
    while (running) {
        printMenu();
        int ch = std::getchar();
        flushInputLine();

        if (ch == EOF) break;

        switch (ch) {
            case '1': {
                ht.print();
                break;
            }
            case '2': {
                std::string key  = readLine("Ключевое слово (термин): ");
                std::string data = readLine("Данные (определение):    ");
                if (ht.insert(key, data)) {
                    int v = calcV(key);
                    int h = calcH(v);
                    std::printf("[OK] Вставлено. V=%d, h=%d\n", v, h);
                }
                break;
            }
            case '3': {
                std::string key = readLine("Ключевое слово: ");
                int idx = ht.search(key);
                if (idx == -1) {
                    std::printf("[НЕ НАЙДЕНО] '%s'\n", key.c_str());
                } else {
                    const Cell& c = ht.getCell(idx);
                    int v = calcV(key);
                    int h = calcH(v);
                    std::printf("[НАЙДЕНО] Строка №%d\n", idx);
                    std::printf("  Ключ:   %s\n", c.id);
                    std::printf("  Данные: %s\n", c.data);
                    std::printf("  V=%d, h=%d | C=%d U=%d T=%d L=%d D=%d P0=%d\n",
                                v, h,
                                (int)c.flagC, (int)c.flagU, (int)c.flagT,
                                (int)c.flagL, (int)c.flagD, c.p0);
                }
                break;
            }
            case '4': {
                std::string key     = readLine("Ключевое слово: ");
                std::string newData = readLine("Новые данные:   ");
                if (ht.update(key, newData)) {
                    std::printf("[OK] Данные обновлены.\n");
                }
                break;
            }
            case '5': {
                std::string key = readLine("Ключевое слово: ");
                if (ht.remove(key)) {
                    std::printf("[OK] Запись удалена.\n");
                }
                break;
            }
            case '6': {
                ht.printLoad();
                break;
            }
            case '7': {
                std::string key = readLine("Ключевое слово: ");
                int v = calcV(key);
                int h = calcH(v);
                std::printf("V = %d,  h = %d\n", v, h);
                break;
            }
            case '0': {
                running = false;
                std::printf("Выход.\n");
                break;
            }
            default: {
                std::printf("Неверный выбор.\n");
                break;
            }
        }
    }

    return 0;
}
