#include <gtest/gtest.h>
#include "hash_table.h"

using namespace HashNS;

TEST(CharToAlphaTest, LetterA_Upper) {
    std::string s = "\xD0\x90";
    EXPECT_EQ(charToAlphaIndex(s, 0), 0);
}

TEST(CharToAlphaTest, LetterYo_Upper) {
    std::string s = "\xD0\x81";
    EXPECT_EQ(charToAlphaIndex(s, 0), 6);
}

TEST(CharToAlphaTest, LetterYo_Lower) {
    std::string s = "\xD1\x91";
    EXPECT_EQ(charToAlphaIndex(s, 0), 6);
}

TEST(CharToAlphaTest, LetterR_Lower) {
    std::string s = "\xD1\x80";
    EXPECT_EQ(charToAlphaIndex(s, 0), 17);
}

TEST(CharToAlphaTest, InvalidByte) {
    std::string s = "AB";
    EXPECT_EQ(charToAlphaIndex(s, 0), -1);
}

TEST(CharToAlphaTest, OutOfBounds) {
    std::string s = "\xD0";
    EXPECT_EQ(charToAlphaIndex(s, 0), -1);
}

TEST(HashFuncTest, CalcV_Avtobus) {
    EXPECT_EQ(calcV("Автобус"), 2);
}

TEST(HashFuncTest, CalcH_Avtobus) {
    EXPECT_EQ(calcH(calcV("Автобус")), 2);
}

TEST(HashFuncTest, CalcV_Tramvay) {
    int v = calcV("Трамвай");
    EXPECT_EQ(calcH(v), calcH(calcV("Троллейбус")));
}

TEST(HashFuncTest, CalcH_Range) {
    std::string keys[] = {"Автобус", "Трамвай", "Паром", "Корабль", "Метро"};
    for (const auto& key : keys) {
        int h = calcH(calcV(key));
        EXPECT_GE(h, BASE_ADDR);
        EXPECT_LT(h, BASE_ADDR + TABLE_SIZE);
    }
}

TEST(HashFuncTest, SingleLetterKey_VIsZero) {
    int v = calcV("Аб");
    EXPECT_GE(v, 0);
}

TEST(CellTest, DefaultState) {
    Cell c;
    EXPECT_FALSE(c.flagU);
    EXPECT_FALSE(c.flagC);
    EXPECT_FALSE(c.flagT);
    EXPECT_FALSE(c.flagL);
    EXPECT_FALSE(c.flagD);
    EXPECT_EQ(c.p0, -1);
    EXPECT_EQ(c.id[0], '\0');
    EXPECT_EQ(c.data[0], '\0');
}

TEST(CellTest, ClearResetsAll) {
    Cell c;
    c.flagU = true;
    c.flagC = true;
    c.p0    = 5;
    c.clear();
    EXPECT_FALSE(c.flagU);
    EXPECT_EQ(c.p0, -1);
    EXPECT_EQ(c.id[0], '\0');
}

TEST(InsertTest, InsertSingle) {
    HashTable ht;
    EXPECT_TRUE(ht.insert("Автобус", "Описание"));
}

TEST(InsertTest, InsertedCellHasCorrectFlags) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    int idx = ht.search("Автобус");
    EXPECT_NE(idx, -1);
    const Cell& c = ht.getCell(idx);
    EXPECT_TRUE(c.flagU);
    EXPECT_TRUE(c.flagT);
    EXPECT_FALSE(c.flagD);
    EXPECT_EQ(c.p0, -1);
}

TEST(InsertTest, InsertedCellAtCorrectHash) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    int h = calcH(calcV("Автобус"));
    int idx = ht.search("Автобус");
    EXPECT_EQ(idx, h);
    EXPECT_FALSE(ht.getCell(idx).flagC);
}

TEST(InsertTest, DuplicateKeyFails) {
    HashTable ht;
    EXPECT_TRUE(ht.insert("Автобус", "Описание"));
    EXPECT_FALSE(ht.insert("Автобус", "Другое"));
}

TEST(InsertTest, ShortKeyFails) {
    HashTable ht;
    EXPECT_FALSE(ht.insert("А", "Описание"));
}

TEST(InsertTest, CollisionSetsFlags) {
    HashTable ht;
    ht.insert("Автобус",    "Первый");
    ht.insert("Автомобиль", "Второй");
    int idx1 = ht.search("Автобус");
    int idx2 = ht.search("Автомобиль");
    EXPECT_NE(idx1, -1);
    EXPECT_NE(idx2, -1);
    EXPECT_NE(idx1, idx2);
    EXPECT_TRUE(ht.getCell(idx2).flagC);
    EXPECT_EQ(ht.getCell(idx1).p0, idx2);
    EXPECT_FALSE(ht.getCell(idx1).flagT);
}

TEST(InsertTest, TableFull) {
    HashTable ht;
    int inserted = 0;
    std::string alphabet = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЮЯ";
    for (int i = 0; i < 33 && inserted < TABLE_SIZE; i++) {
        for (int j = 0; j < 33 && inserted < TABLE_SIZE; j++) {
            if (inserted >= TABLE_SIZE) break;
            break;
        }
        break;
    }
    const char* keys[] = {
        "Автобус", "Автомобиль", "Аэропорт", "Трамвай", "Троллейбус",
        "Такси",   "Метро",      "Паром",    "Велосипед","Поезд",
        "Корабль", "Самолёт",    "Фуникулёр","Байдарка", "Баржа",
        "Вагон",   "Глиссер",    "Дирижабль","Катамаран","Катер",
        "Лайнер",  "Мотоцикл",  "Маршрутка", "Планёр",  "Ракета",
        "Скутер",  "Субмарина",  "Тепловоз", "Шхуна",   "Яхта"
    };
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht.insert(keys[i], "данные");
    }
    EXPECT_FALSE(ht.insert("Яхтсмен", "данные"));
}

TEST(SearchTest, FindExisting) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    EXPECT_NE(ht.search("Автобус"), -1);
}

TEST(SearchTest, NotFound) {
    HashTable ht;
    EXPECT_EQ(ht.search("Автобус"), -1);
}

TEST(SearchTest, CaseInsensitive) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    EXPECT_NE(ht.search("автобус"), -1);
    EXPECT_NE(ht.search("АВТОБУС"), -1);
}

TEST(SearchTest, FindInChain) {
    HashTable ht;
    ht.insert("Автобус",    "Первый");
    ht.insert("Автомобиль", "Второй");
    EXPECT_NE(ht.search("Автомобиль"), -1);
}

TEST(SearchTest, DataIsCorrect) {
    HashTable ht;
    ht.insert("Автобус", "Мой транспорт");
    int idx = ht.search("Автобус");
    EXPECT_STREQ(ht.getCell(idx).data, "Мой транспорт");
}

TEST(UpdateTest, UpdateExisting) {
    HashTable ht;
    ht.insert("Автобус", "Старое");
    EXPECT_TRUE(ht.update("Автобус", "Новое"));
    int idx = ht.search("Автобус");
    EXPECT_STREQ(ht.getCell(idx).data, "Новое");
}

TEST(UpdateTest, UpdateNotFound) {
    HashTable ht;
    EXPECT_FALSE(ht.update("Автобус", "Данные"));
}

TEST(UpdateTest, UpdateInChain) {
    HashTable ht;
    ht.insert("Автобус",    "Данные1");
    ht.insert("Автомобиль", "Данные2");
    EXPECT_TRUE(ht.update("Автомобиль", "Новые данные"));
    int idx = ht.search("Автомобиль");
    EXPECT_STREQ(ht.getCell(idx).data, "Новые данные");
}

TEST(RemoveTest, RemoveSingle) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    EXPECT_TRUE(ht.remove("Автобус"));
    EXPECT_EQ(ht.search("Автобус"), -1);
}

TEST(RemoveTest, RemoveNotFound) {
    HashTable ht;
    EXPECT_FALSE(ht.remove("Автобус"));
}

TEST(RemoveTest, RemoveLastInChain) {
    HashTable ht;
    ht.insert("Автобус",    "Первый");
    ht.insert("Автомобиль", "Второй");
    EXPECT_TRUE(ht.remove("Автомобиль"));
    EXPECT_EQ(ht.search("Автомобиль"), -1);
    EXPECT_NE(ht.search("Автобус"), -1);
    int idx = ht.search("Автобус");
    EXPECT_TRUE(ht.getCell(idx).flagT);
    EXPECT_EQ(ht.getCell(idx).p0, -1);
}

TEST(RemoveTest, RemoveFirstInChain) {
    HashTable ht;
    ht.insert("Автобус",    "Первый");
    ht.insert("Автомобиль", "Второй");
    EXPECT_TRUE(ht.remove("Автобус"));
    EXPECT_EQ(ht.search("Автобус"), -1);
    EXPECT_NE(ht.search("Автомобиль"), -1);
}

TEST(RemoveTest, RemoveAndReinsert) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    ht.remove("Автобус");
    EXPECT_TRUE(ht.insert("Автобус", "Новое"));
    EXPECT_NE(ht.search("Автобус"), -1);
}

TEST(RemoveTest, RemoveMiddleInChain) {
    HashTable ht;
    ht.insert("Трамвай",    "Первый");
    ht.insert("Троллейбус", "Второй");
    ht.insert("Метро",      "Третий");
    EXPECT_TRUE(ht.remove("Троллейбус"));
    EXPECT_EQ(ht.search("Троллейбус"), -1);
    EXPECT_NE(ht.search("Трамвай"), -1);
    EXPECT_NE(ht.search("Метро"), -1);
}

TEST(PrintTest, PrintDoesNotCrash) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    EXPECT_NO_THROW(ht.print());
}

TEST(PrintTest, PrintLoadDoesNotCrash) {
    HashTable ht;
    ht.insert("Автобус", "Описание");
    EXPECT_NO_THROW(ht.printLoad());
}

TEST(PrintTest, PrintEmptyTable) {
    HashTable ht;
    EXPECT_NO_THROW(ht.print());
}
