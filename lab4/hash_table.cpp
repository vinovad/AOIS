#include "hash_table.h"
#include <cstring>
#include <cstdio>

namespace HashNS {

Cell::Cell() {
    clear();
}

void Cell::clear() {
    id[0] = '\0';
    data[0] = '\0';
    flagC = false;
    flagU = false;
    flagT = false;
    flagL = false;
    flagD = false;
    p0 = -1;
}

int charToAlphaIndex(const std::string& key, int bytePos) {
    if (bytePos + 1 >= (int)key.size()) return -1;

    unsigned char firstByte = (unsigned char)key[bytePos];
    unsigned char secondByte = (unsigned char)key[bytePos + 1];

    int alphaCode = -1;

    if (firstByte == 0xD0) {
        if (secondByte == 0x81) alphaCode = 6;
        else if (secondByte >= 0x90 && secondByte <= 0x9F) alphaCode = secondByte - 0x90;
        else if (secondByte >= 0xA0 && secondByte <= 0xAF) alphaCode = secondByte - 0xA0 + 16;
        else if (secondByte >= 0xB0 && secondByte <= 0xBF) alphaCode = secondByte - 0xB0;
    } else if (firstByte == 0xD1) {
        if (secondByte == 0x91) alphaCode = 6;
        else if (secondByte >= 0x80 && secondByte <= 0x8F) alphaCode = secondByte - 0x80 + 16;
    }

    if (alphaCode > 5 && alphaCode != 6) {
        alphaCode += 1;
    }

    return alphaCode;
}

static int nthCyrillicBytePos(const std::string& key, int targetIndex) {
    int foundCount = 0;
    for (int byteIdx = 0; byteIdx < (int)key.size(); ) {
        unsigned char currentByte = (unsigned char)key[byteIdx];
        int charBytes = 1;
        if ((currentByte & 0xE0) == 0xC0) charBytes = 2;
        else if ((currentByte & 0xF0) == 0xE0) charBytes = 3;
        else if ((currentByte & 0xF8) == 0xF0) charBytes = 4;

        if (charBytes == 2 && charToAlphaIndex(key, byteIdx) >= 0) {
            if (foundCount == targetIndex) return byteIdx;
            foundCount++;
        }
        byteIdx += charBytes;
    }
    return -1;
}

int calcV(const std::string& key) {
    int firstCharPos = nthCyrillicBytePos(key, 0);
    int secondCharPos = nthCyrillicBytePos(key, 1);
    int firstCharIdx = (firstCharPos >= 0) ? charToAlphaIndex(key, firstCharPos) : 0;
    int secondCharIdx = (secondCharPos >= 0) ? charToAlphaIndex(key, secondCharPos) : 0;
    return firstCharIdx * ALPHABET_BASE + secondCharIdx;
}

int calcH(int numericValue) {
    return (numericValue % TABLE_SIZE) + BASE_ADDR;
}

HashTable::HashTable() {
}

void HashTable::safeCopyStr(char* dst, const char* src, int maxLen) {
    std::strncpy(dst, src, maxLen - 1);
    dst[maxLen - 1] = '\0';
}

int HashTable::nextProbe(int current) {
    return (current + 1) % TABLE_SIZE;
}

static void lowercaseCyrByte(unsigned char firstByte, unsigned char secondByte,
                              unsigned char& outFirst, unsigned char& outSecond) {
    outFirst = firstByte; outSecond = secondByte;
    if (firstByte == 0xD0) {
        if (secondByte == 0x81) { outFirst = 0xD1; outSecond = 0x91; return; }
        if (secondByte >= 0x90 && secondByte <= 0x9F) { outSecond = secondByte + 0x20; return; }
        if (secondByte >= 0xA0 && secondByte <= 0xAF) { outFirst = 0xD1; outSecond = secondByte - 0x20; return; }
    }
}

bool HashTable::keysEqual(const char* keyA, const char* keyB) {
    int idxA = 0, idxB = 0;
    while (keyA[idxA] != '\0' && keyB[idxB] != '\0') {
        unsigned char byteA1 = (unsigned char)keyA[idxA];
        unsigned char byteB1 = (unsigned char)keyB[idxB];

        if ((byteA1 == 0xD0 || byteA1 == 0xD1) && (byteB1 == 0xD0 || byteB1 == 0xD1)
            && keyA[idxA+1] != '\0' && keyB[idxB+1] != '\0') {
            unsigned char byteA2 = (unsigned char)keyA[idxA+1];
            unsigned char byteB2 = (unsigned char)keyB[idxB+1];
            unsigned char lowerA1, lowerA2, lowerB1, lowerB2;
            lowercaseCyrByte(byteA1, byteA2, lowerA1, lowerA2);
            lowercaseCyrByte(byteB1, byteB2, lowerB1, lowerB2);
            if (lowerA1 != lowerB1 || lowerA2 != lowerB2) return false;
            idxA += 2; idxB += 2;
            continue;
        }

        unsigned char lowerA = (byteA1 >= 'A' && byteA1 <= 'Z') ? (byteA1 + 32) : byteA1;
        unsigned char lowerB = (byteB1 >= 'A' && byteB1 <= 'Z') ? (byteB1 + 32) : byteB1;
        if (lowerA != lowerB) return false;
        idxA++; idxB++;
    }
    return keyA[idxA] == '\0' && keyB[idxB] == '\0';
}

bool HashTable::insert(const std::string& key, const std::string& data) {
    if (nthCyrillicBytePos(key, 1) < 0) {
        std::printf("[ОШИБКА] Ключ '%s' содержит менее 2 кириллических букв.\n", key.c_str());
        return false;
    }

    int numericValue = calcV(key);
    int hashAddr = calcH(numericValue);

    int freeSlot = -1;
    int probeIdx = hashAddr;
    int probed = 0;

    while (probed < TABLE_SIZE) {
        Cell& cell = cells_[probeIdx];

        if (cell.flagU && !cell.flagD) {
            if (keysEqual(cell.id, key.c_str())) {
                std::printf("[ОШИБКА] Ключ '%s' уже существует в таблице.\n", key.c_str());
                return false;
            }
        } else if (freeSlot == -1) {
            freeSlot = probeIdx;
        }

        probeIdx = nextProbe(probeIdx);
        probed++;
    }

    if (freeSlot == -1) {
        std::printf("[ОШИБКА] Таблица полностью заполнена.\n");
        return false;
    }

    Cell& target = cells_[freeSlot];
    safeCopyStr(target.id, key.c_str(), ID_LEN);
    safeCopyStr(target.data, data.c_str(), DATA_LEN);
    target.flagU = true;
    target.flagD = false;
    target.flagL = false;
    target.flagT = true;
    target.flagC = (freeSlot != hashAddr);
    target.p0 = -1;

    if (freeSlot != hashAddr) {
        int chainIdx = hashAddr;
        while (cells_[chainIdx].flagU && !cells_[chainIdx].flagD && !cells_[chainIdx].flagT) {
            chainIdx = cells_[chainIdx].p0;
        }
        if (cells_[chainIdx].flagU && !cells_[chainIdx].flagD) {
            cells_[chainIdx].flagT = false;
            cells_[chainIdx].p0 = freeSlot;
        }
    }

    return true;
}

int HashTable::search(const std::string& key) const {
    int hashAddr = calcH(calcV(key));
    int probeIdx = hashAddr;
    int probed = 0;

    while (probed < TABLE_SIZE) {
        const Cell& cell = cells_[probeIdx];
        if (cell.flagU && !cell.flagD && keysEqual(cell.id, key.c_str())) {
            return probeIdx;
        }
        probeIdx = nextProbe(probeIdx);
        probed++;
    }
    return -1;
}

bool HashTable::update(const std::string& key, const std::string& newData) {
    int idx = search(key);
    if (idx == -1) {
        std::printf("[ОШИБКА] Ключ '%s' не найден.\n", key.c_str());
        return false;
    }
    safeCopyStr(cells_[idx].data, newData.c_str(), DATA_LEN);
    return true;
}

bool HashTable::remove(const std::string& key) {
    int cellIdx = search(key);
    if (cellIdx == -1) {
        std::printf("[ОШИБКА] Ключ '%s' не найден.\n", key.c_str());
        return false;
    }

    Cell& deletedCell = cells_[cellIdx];
    deletedCell.flagD = true;

    int hashAddr = calcH(calcV(key));

    if (deletedCell.flagT && !deletedCell.flagC && cellIdx == hashAddr) {
        deletedCell.clear();
        return true;
    }

    if (deletedCell.flagT) {
        for (int scanIdx = 0; scanIdx < TABLE_SIZE; scanIdx++) {
            if (scanIdx != cellIdx
                && cells_[scanIdx].flagU
                && !cells_[scanIdx].flagD
                && cells_[scanIdx].p0 == cellIdx)
            {
                cells_[scanIdx].flagT = true;
                cells_[scanIdx].p0 = -1;
                break;
            }
        }
        deletedCell.clear();
        return true;
    }

    int nextCellIdx = deletedCell.p0;
    Cell& nextCell = cells_[nextCellIdx];

    safeCopyStr(deletedCell.id, nextCell.id, ID_LEN);
    safeCopyStr(deletedCell.data, nextCell.data, DATA_LEN);
    deletedCell.flagC = nextCell.flagC;
    deletedCell.flagT = nextCell.flagT;
    deletedCell.flagL = nextCell.flagL;
    deletedCell.flagD = false;
    deletedCell.flagU = true;
    deletedCell.p0 = nextCell.p0;

    nextCell.clear();
    return true;
}

static int utf8Len(const char* str) {
    int charCount = 0;
    for (int byteIdx = 0; str[byteIdx] != '\0'; ) {
        unsigned char currentByte = (unsigned char)str[byteIdx];
        if (currentByte < 0x80) { byteIdx += 1; }
        else if ((currentByte & 0xE0) == 0xC0) { byteIdx += 2; }
        else if ((currentByte & 0xF0) == 0xE0) { byteIdx += 3; }
        else { byteIdx += 4; }
        charCount++;
    }
    return charCount;
}

static void printPadded(const char* str, int width) {
    std::printf("%s", str);
    int padCount = width - utf8Len(str);
    for (int padIdx = 0; padIdx < padCount; padIdx++) std::printf(" ");
}

void HashTable::print() const {
    static const int W_IDX = 3;
    static const int W_ID = 14;
    static const int W_DATA = 35;
    static const int W_V = 4;
    static const int W_H = 3;

    std::printf("\n");
    std::printf("%-*s | ", W_IDX, "№");
    printPadded("ID (ключ)", W_ID);
    std::printf(" | C U T L D | %4s | ", "P0");
    printPadded("Данные", W_DATA);
    std::printf(" | %*s %*s\n", W_V, "V", W_H, "h");

    int separatorLen = W_IDX + 3 + W_ID + 3 + 11 + 3 + 6 + 3 + W_DATA + 3 + W_V + 1 + W_H + 1;
    for (int sepIdx = 0; sepIdx < separatorLen; sepIdx++) std::printf("-");
    std::printf("\n");

    for (int rowIdx = 0; rowIdx < TABLE_SIZE; rowIdx++) {
        const Cell& cell = cells_[rowIdx];
        int numericValue = (cell.flagU && !cell.flagD) ? calcV(cell.id) : -1;
        int hashAddr = (numericValue >= 0) ? calcH(numericValue) : -1;

        const char* idStr = cell.flagU ? cell.id : "(пусто)";
        const char* dataStr = cell.flagU ? cell.data : "";

        char vStr[12], hStr[12];
        if (numericValue >= 0) std::snprintf(vStr, sizeof(vStr), "%d", numericValue);
        else std::snprintf(vStr, sizeof(vStr), "-");
        if (hashAddr >= 0) std::snprintf(hStr, sizeof(hStr), "%d", hashAddr);
        else std::snprintf(hStr, sizeof(hStr), "-");

        std::printf("%-*d | ", W_IDX, rowIdx);
        printPadded(idStr, W_ID);
        std::printf(" | %d %d %d %d %d | %4d | ", (int)cell.flagC, (int)cell.flagU, (int)cell.flagT, (int)cell.flagL, (int)cell.flagD, cell.p0);
        printPadded(dataStr, W_DATA);
        std::printf(" | %*s %*s\n", W_V, vStr, W_H, hStr);
    }
    std::printf("\n");
}

void HashTable::printLoad() const {
    int occupiedCount = 0;
    for (int rowIdx = 0; rowIdx < TABLE_SIZE; rowIdx++) {
        if (cells_[rowIdx].flagU && !cells_[rowIdx].flagD) occupiedCount++;
    }
    double loadFactor = (double)occupiedCount / TABLE_SIZE * 100.0;
    std::printf("Коэффициент заполнения: %d/%d = %.1f%%\n", occupiedCount, TABLE_SIZE, loadFactor);
}

const Cell& HashTable::getCell(int idx) const {
    return cells_[idx];
}

}
