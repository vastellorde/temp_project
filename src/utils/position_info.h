//
// Created by Арман Чукин on 27.11.2022.
//
struct PositionInfo {
    int squares[64]{};
    bool whiteCastleKingSide{};
    bool whiteCastleQueenSide{};
    bool blackCastleKingSide{};
    bool blackCastleQueenSide{};
    int epFile{};
    bool whiteToMove{};
    int plyCount{};
};