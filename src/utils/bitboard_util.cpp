//
// Created by Арман Чукин on 30.11.2022.
//

#include "bitboard_util.h"

bool BitboardUtil::containsSquare(unsigned long bitboard, int square) {
    return ((bitboard >> square) & 1) != 0;
}
