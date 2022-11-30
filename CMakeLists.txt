cmake_minimum_required(VERSION 3.23)
project(chess_sdl)

set(CMAKE_CXX_STANDARD 23)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

add_executable(chess_sdl main.cpp src/board.cpp src/board.h src/Piece.cpp src/Piece.h src/piece_list.cpp src/piece_list.h src/utils/fen_util.cpp src/utils/fen_util.h src/utils/position_info.h src/board_representation.cpp src/board_representation.h src/ui/piece_ui.cpp src/ui/piece_ui.h src/utils/move_generator.cpp src/utils/move_generator.h src/utils/move.h src/utils/precomputed_move_data.cpp src/utils/precomputed_move_data.h src/utils/bitboard_util.cpp src/utils/bitboard_util.h)

find_package(spdlog REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML COMPONENTS system graphics window REQUIRED)

include_directories(${SFML_INCLUDE_DIR})

target_link_libraries(chess_sdl sfml-system sfml-window sfml-graphics spdlog::spdlog)
