#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace MyProject {
    const int BITS_IN_3BIT_COUNTER = 3; // Для 3 задания
    const int BITS_IN_TETRAD = 4;       // Для 1 и 2 заданий
    const int COUNTER_MAX_VALUE = 7;    // 2^3 - 1
    const int BCD_OFFSET = 2; // Ваше смещение n=2
    const int BCD_THRESHOLD = 9; // Порог для BCD коррекции
    const int BCD_CORRECTION_VALUE = 6; // Значение для коррекции (16-10)
    const int BCD_MAX_DIGIT = 9; // Максимальная цифра BCD
    const char LOG_LEVEL_INFO[] = "[INFO] ";
    const char DELIMITER[] = "--------------------------------";
}

#endif