#pragma once
#include <string>

namespace HashNS {

static const int TABLE_SIZE = 30;
static const int BASE_ADDR = 0;
static const int ALPHABET_BASE = 33;

static const int ID_LEN   = 64;
static const int DATA_LEN = 128;

class Cell {
public:
    char id[ID_LEN];
    char data[DATA_LEN];

    bool flagC;
    bool flagU;
    bool flagT;
    bool flagL;
    bool flagD;
    int  p0;

    Cell();
    void clear();
};

class HashTable {
private:
    Cell cells_[TABLE_SIZE];

    static int  nextProbe(int current);
    static bool keysEqual(const char* keyA, const char* keyB);
    static void safeCopyStr(char* dst, const char* src, int maxLen);

public:
    HashTable();

    bool insert(const std::string& key, const std::string& data);
    int  search(const std::string& key) const;
    bool update(const std::string& key, const std::string& newData);
    bool remove(const std::string& key);

    void print() const;
    void printLoad() const;

    const Cell& getCell(int idx) const;

};

int charToAlphaIndex(const std::string& key, int bytePos);
int calcV(const std::string& key);
int calcH(int v);

}