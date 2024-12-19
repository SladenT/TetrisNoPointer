#include "raylib.h"
#include "types.h"
#include "rng.h"

///////////////////////////////////////////////////
// Variables //////////////////////////////////////
///////////////////////////////////////////////////
TetroGridBlock grid[GRIDYMAX*GRIDXMAX];
// Tetromino tetrominos[GRIDYMAX*GRIDXMAX]; // Can probably make this smaller by dividing by 4
uShort tetroBlockCnt = 0;
uInt gridOffsetX = 224;
uInt gridOffsetY = 32;
float tickSpeed   = 1.0f;
float tickCount  = 0.0f; // Updated by deltaTime
Tetromino controlledTetromino;
GameState state = PLAYING;

///////////////////////////////////////////////////
// Tetromino Prototypes ///////////////////////////
///////////////////////////////////////////////////
Tetromino box = (Tetromino){{{GRIDXMAX/2, 0}, {GRIDXMAX/2, 1}, {(GRIDXMAX/2)+1, 0}, {(GRIDXMAX/2)+1, 1}}, Box};

///////////////////////////////////////////////////
// Static Functions ///////////////////////////////
///////////////////////////////////////////////////
static bool CheckValidTetromino(void)
{
    for(int i = 0; i < 4; i++)
    {
        if (controlledTetromino.blocks[i].y >= GRIDYMAX){return false;}
        if (controlledTetromino.blocks[i].x >= GRIDXMAX){return false;}
        uInt Pos = (controlledTetromino.blocks[i].y * GRIDXMAX) + controlledTetromino.blocks[i].x;
        if(grid[Pos].filled){return false;}
    }
    return true;
}

static void MoveTetrominoRelative(Int x, Int y)
{
    for(int i = 0; i < 4; i++)
    {
        controlledTetromino.blocks[i].y += y;
        controlledTetromino.blocks[i].x += x;
    }
}

static void RemoveTetrominoFromGrid(void)
{
    for(int i = 0; i < 4; i++)
    {
        uInt Pos = (controlledTetromino.blocks[i].y * GRIDXMAX) + controlledTetromino.blocks[i].x;
        grid[Pos].filled = false;
    }
}

static void AddTetrominoToGrid(void)
{
    for(int i = 0; i < 4; i++)
    {
        uInt Pos = (controlledTetromino.blocks[i].y * GRIDXMAX) + controlledTetromino.blocks[i].x;
        grid[Pos].filled = true;
        grid[Pos].type   = controlledTetromino.type;
    }
}

static void SpawnTetromino(void)
{
    // Get tetromino type
    controlledTetromino.type = (TetrominoType)GetNextInt(6);
    controlledTetromino.type = 0;
    // Fill out the Tetromino based upon the type
    switch(controlledTetromino.type)
    {
        case Box:
            controlledTetromino = box;
            break;
        case Line:
            break;
        case LR:
            break;
        case LL:
            break;
        case ZR:
            break;
        case ZL:
            break;
        case T:
            break;
    }
    // Check for valid spawn location, game over if none
    if(CheckValidTetromino())
    {
        // Add tetromino to the grid
        AddTetrominoToGrid();
        return;
    }
    // Do game over

    
}

///////////////////////////////////////////////////
// Program Entry //////////////////////////////////
///////////////////////////////////////////////////
int main()
{
    InitWindow(800, 800, "Tetris");
    SetSeedWithClock();
    // Init grid
    for (int i = 0; i < GRIDYMAX*GRIDXMAX; i++)
    {
        grid[i].filled = false;
        grid[i].type   = Box;
    }
    SpawnTetromino();

    while(!WindowShouldClose())
    {
        if (((state & PAUSED) == PAUSED) || ((state & GAME_OVER) == GAME_OVER))
        {continue;}
        tickCount += GetFrameTime();
        if (tickCount >= tickSpeed)
        {
            RemoveTetrominoFromGrid();
            MoveTetrominoRelative(0, 1);
            if (CheckValidTetromino())
            {
                AddTetrominoToGrid();
            }
            else
            {
                MoveTetrominoRelative(0, -1);
                // Put down block and spawn new Tetromino
                AddTetrominoToGrid();
                SpawnTetromino();
            }
            tickCount = 0;
        }
        BeginDrawing();
            ClearBackground(BLACK);
            for(int i = 0; i < GRIDYMAX*GRIDXMAX; i++)
            {
                if(grid[i].filled)
                {
                    uInt xPos = ((i%GRIDXMAX)*BLOCKSIZE)+gridOffsetX;
                    uInt yPos = ((i/GRIDXMAX)*BLOCKSIZE)+gridOffsetY;
                    // TODO: Set color based on tetromino type and current level
                    DrawRectangle(xPos, yPos, BLOCKSIZE, BLOCKSIZE, BLUE);
                    DrawRectangle(xPos+BLOCKBORDER/2, yPos+BLOCKBORDER/2, BLOCKSIZE-BLOCKBORDER, BLOCKSIZE-BLOCKBORDER, RED);
                }
            }
        EndDrawing();
    }
    return 0;
}