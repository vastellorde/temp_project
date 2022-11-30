//
// Created by Арман Чукин on 27.11.2022.
//

#include <utility>
#include <vector>
#include "fen_util.h"

std::string FenUtil::currentFen() {
    return std::string();
}

PositionInfo FenUtil::PositionFromFen(std::string fen) {
    PositionInfo positionInfo;

    std::vector<std::string> sections = split(fen, " ");
    int file = 0;
    int rank = 7;

    for (char sym : sections[0]) {
        if (sym == '/') {
            file = 0;
            rank--;
        } else {
            if (isdigit(sym)) {
                file += (int)sym;
            } else {
                int pieceColor = isupper(sym) == 1 ? Piece::White : Piece::Black;
                int pieceType = pieceTypeFromSymbol[(char)tolower(sym)];
                positionInfo.squares[rank * 8 + file] = pieceType | pieceColor;
                file++;
            }
        }
    }

    positionInfo.whiteToMove = (sections[1] == "w");

    std::string castlingRights = (sections.size() > 2) ? sections[2] : "KQkq";
    positionInfo.whiteCastleKingSide = castlingRights.contains("K");
    positionInfo.whiteCastleQueenSide = castlingRights.contains("Q");
    positionInfo.blackCastleKingSide = castlingRights.contains("k");
    positionInfo.blackCastleQueenSide = castlingRights.contains("q");

    if(sections.size() > 3) {
        std::string enPassantFile = std::to_string(sections[3][0]);
        if (fileNames.contains(enPassantFile)) {
            positionInfo.epFile = fileNames.find(enPassantFile, 0) + 1;
        }
    }

    return positionInfo;
}

std::vector<std::string> FenUtil::split(const std::string& input, const std::string& delimiter) {
    size_t posStart = 0, posEnd, delimLength = delimiter.length();
    std::string token;
    std::vector<std::string> res{};

    while ((posEnd = input.find(delimiter, posStart)) != std::string::npos) {
        token = input.substr(posStart, posEnd - posStart);
        posStart = posEnd + delimLength;
        res.push_back(token);
    }

    res.push_back(input.substr(posStart));
    return res;
}
