#ifndef typesh
#define typesh
#include <stdint.h>
#include <stdbool.h>

#define GRIDYMAX  20
#define GRIDXMAX  10
#define BLOCKSIZE 32
#define BLOCKBORDER 2


#define uByte  uint8_t
#define uShort uint16_t
#define uInt   uint32_t
#define uLong  uint64_t

#define Byte  int8_t
#define Short int16_t
#define Int   int32_t
#define Long  int64_t

typedef enum TetrominoType
{
    Box,
    Line,
    LR,
    LL,
    ZR,
    ZL,
    T
} TetrominoType;

typedef enum GameState
{
    PAUSED    = 0x1,
    PLAYING   = 0x2,
    GAME_OVER = 0x4
} GameState;

typedef struct Vector2Int
{
    uInt x;
    uInt y;
} Vector2Int;

typedef struct Tetromino
{
    Vector2Int blocks[4];
    TetrominoType type;
} Tetromino;

typedef struct TetroGridBlock
{
    bool filled;
    TetrominoType type;
} TetroGridBlock;

#endif