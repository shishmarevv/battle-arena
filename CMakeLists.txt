cmake_minimum_required(VERSION 3.31)
project(battle_arena C)

set(CMAKE_C_STANDARD 11)

add_executable(battle_arena
        main.c
        src/game.c
        src/json.c
        src/logger.c
        src/structs.c
        src/utility.c)


target_include_directories(battle_arena PRIVATE include)
target_link_libraries(battle_arena -lncurses)