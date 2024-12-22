#include <stdio.h>
#include "raylib.h"
#include "types.h"
#include "rng.h"

///////////////////////////////////////////////////
// Variables //////////////////////////////////////
///////////////////////////////////////////////////
TetroGridBlock grid[GRIDYMAX*GRIDXMAX];
uInt deletedLines[4]; // Used for deletion and scoring
uShort tetroBlockCnt = 0;
uInt gridOffsetX = 224;
uInt gridOffsetY = 96;
float tickSpeed   = 1.0f;
float tickCount  = 0.0f; // Updated by deltaTime
Tetromino controlledTetromino;
GameState state = PLAYING;

///////////////////////////////////////////////////
// Rotational Matrices ////////////////////////////
///////////////////////////////////////////////////

Vector2Int LRotMat[5][5] = 
{
    {{0,0},{0,0},{-2, 2},{ 0, 0},{ 0, 0}},
    {{0,0},{0,2},{-1, 1},{-2, 0},{ 0, 0}},
    {{2,2},{1,1},{ 0, 0},{-1,-1},{-2,-2}},
    {{0,0},{2,0},{ 1,-1},{ 0,-2},{ 0, 0}},
    {{0,0},{0,0},{ 2,-2},{ 0, 0},{ 0, 0}}
};
Vector2Int RRotMat[5][5] = 
{
    {{0, 0},{0, 0},{ 2, 2},{ 0, 0},{ 0, 0}},
    {{0, 0},{2, 0},{ 1, 1},{ 0, 2},{ 0, 0}},
    {{2,-2},{1,-1},{ 0, 0},{-1, 1},{-2, 2}},
    {{0, 0},{0,-2},{-1,-1},{-2, 0},{ 0, 0}},
    {{0, 0},{0, 0},{-2,-2},{ 0, 0},{ 0, 0}}
};

///////////////////////////////////////////////////
// Tetromino Prototypes ///////////////////////////
///////////////////////////////////////////////////
Tetromino box    = (Tetromino){{{GRIDXMAX/2, 0}, {GRIDXMAX/2, 1}, {(GRIDXMAX/2)+1, 0}, {(GRIDXMAX/2)+1, 1}}, Box};
Tetromino line   = (Tetromino){{{GRIDXMAX/2, 1}, {GRIDXMAX/2, 0}, {(GRIDXMAX/2), 2}, {(GRIDXMAX/2), 3}}, Line};
Tetromino lr     = (Tetromino){{{GRIDXMAX/2, 1}, {GRIDXMAX/2, 0}, {(GRIDXMAX/2)+1, 0}, {(GRIDXMAX/2), 2}}, LR};
Tetromino ll     = (Tetromino){{{GRIDXMAX/2, 1}, {GRIDXMAX/2, 0}, {(GRIDXMAX/2)-1, 0}, {(GRIDXMAX/2), 2}}, LL};
Tetromino zr     = (Tetromino){{{GRIDXMAX/2, 1}, {GRIDXMAX/2, 0}, {(GRIDXMAX/2)+1, 0}, {(GRIDXMAX/2)-1, 1}}, ZR};
Tetromino zl     = (Tetromino){{{GRIDXMAX/2, 1}, {GRIDXMAX/2, 0}, {(GRIDXMAX/2)-1, 0}, {(GRIDXMAX/2)+1, 1}}, ZL};
Tetromino tblock = (Tetromino){{{GRIDXMAX/2, 1}, {GRIDXMAX/2, 0}, {(GRIDXMAX/2)-1, 0}, {(GRIDXMAX/2)+1, 0}}, T};



///////////////////////////////////////////////////
// Function Predeclarations ///////////////////////
///////////////////////////////////////////////////
static void CheckLineDelete();

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
    
    // Check if the previous drop scored points
    CheckLineDelete();
    // Get tetromino type
    controlledTetromino.type = (TetrominoType)GetNextInt(7);
    // controlledTetromino.type = 0;
    // Fill out the Tetromino based upon the type
    switch(controlledTetromino.type)
    {
        case Box:
            controlledTetromino = box;
            break;
        case Line:
            controlledTetromino = line;
            break;
        case LR:
            controlledTetromino = lr;
            break;
        case LL:
            controlledTetromino = ll;
            break;
        case ZR:
            controlledTetromino = zr;
            break;
        case ZL:
            controlledTetromino = zl;
            break;
        case T:
            controlledTetromino = tblock;
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
    state = GAME_OVER;
    
}

static void GameUpdate()
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

static bool AttemptMoveTetromino(int x, int y)
{
    RemoveTetrominoFromGrid();
    MoveTetrominoRelative(x, y);
    if (CheckValidTetromino())
    {
        AddTetrominoToGrid();
        return true;
    }
    else
    {
        MoveTetrominoRelative(-x, -y);
        AddTetrominoToGrid();
        return false;
    }
}


static void RotateTetromino(bool leftRight)
{
    // Create Rotational Matrix;
    Vector2Int rotMat[5][5];
    // Get values
    Vector2Int stable = controlledTetromino.blocks[0];
    Vector2Int one    = controlledTetromino.blocks[1];
    Vector2Int two    = controlledTetromino.blocks[2];
    Vector2Int three  = controlledTetromino.blocks[3];
    // Modulate around stable block
    one   = (Vector2Int){(one.x+2)-stable.x, (one.y+2)-stable.y};
    two   = (Vector2Int){(two.x+2)-stable.x, (two.y+2)-stable.y};
    three = (Vector2Int){(three.x+2)-stable.x, (three.y+2)-stable.y};
    // Insert values into matrix
    rotMat[one.y][one.x]     = controlledTetromino.blocks[1];
    rotMat[two.y][two.x]     = controlledTetromino.blocks[2];
    rotMat[three.y][three.x] = controlledTetromino.blocks[3];
    // Rotate based on predefined matrix
    if(leftRight)
    {
        for (uInt i = 0; i < 5; i++)
        {
            for (uInt j = 0; j < 5; j++)
            {
                rotMat[i][j].x += LRotMat[i][j].x;
                rotMat[i][j].y += LRotMat[i][j].y;
            }
        }
    }
    else
    {
        for (uInt i = 0; i < 5; i++)
        {
            for (uInt j = 0; j < 5; j++)
            {
                rotMat[i][j].x += RRotMat[i][j].x;
                rotMat[i][j].y += RRotMat[i][j].y;
            }
        }
    }
    // Assign new values
    controlledTetromino.blocks[1] = rotMat[one.y][one.x];
    controlledTetromino.blocks[2] = rotMat[two.y][two.x];
    controlledTetromino.blocks[3] = rotMat[three.y][three.x];
}

static bool AttemptRotateTetromino(bool leftRight)
{
    Tetromino before = controlledTetromino;
    RemoveTetrominoFromGrid();
    RotateTetromino(leftRight);
    if (CheckValidTetromino())
    {
        AddTetrominoToGrid();
        return true;
    }
    else
    {
        RotateTetromino(!leftRight);
        AddTetrominoToGrid();
        return false;
    }
}


static void HandlePlayerInput()
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if ((state & PLAYING) == PLAYING)
        {
            state -= PLAYING;
            state += PAUSED;
        }
        else if ((state & PAUSED) == PAUSED)
        {
            state += PLAYING;
            state -= PAUSED;  
        }
    }
    if (((state & PAUSED) == PAUSED) || ((state & GAME_OVER) == GAME_OVER))
    {
        return;
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        AttemptMoveTetromino(1, 0);
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        AttemptMoveTetromino(-1, 0);
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        if(!AttemptMoveTetromino(0, 1))
        {
            SpawnTetromino();
        }
        tickCount = 0;
    }
    if (IsKeyPressed(KEY_COMMA) && controlledTetromino.type != Box)
    {
        // Rotate Left
        AttemptRotateTetromino(true);
    }
    if (IsKeyPressed(KEY_PERIOD) && controlledTetromino.type != Box)
    {
        // Rotate Right
        AttemptRotateTetromino(false);
    }
}



static void DeleteLinesAndScore(uInt linesDeleted)
{
    // Delete Lines
    if (linesDeleted == 0){return;}
    for (uInt i = 0; i < linesDeleted; i++)
    {
        uInt line = deletedLines[i];
        for (uInt j = 0; j < GRIDXMAX; j++)
        {
            uInt Pos = (line * GRIDXMAX) + j;
            grid[Pos].filled = false;
        }
    }
    // Move everything down by lines deleted
    // TODO: If there is a skipped row, e.g., we delete lines 1, 2 and 4,
    // This currently does not handle it properly.
    uInt line = deletedLines[linesDeleted-1]-1;
    for(int i = line; i >= 0; i--)
    {
        line = i;
        for (uInt j = 0; j < GRIDXMAX; j++)
        {
            uInt Pos = (line * GRIDXMAX) + j;
            uInt nPos = ((line+linesDeleted) * GRIDXMAX) + j;
            grid[nPos].filled = grid[Pos].filled;
            grid[Pos].filled = false;
        }
    }
}


static void CheckLineDelete()
{
    uInt linesDeleted = 0;
    deletedLines[0] = 0;
    deletedLines[1] = 0;
    deletedLines[2] = 0;
    deletedLines[3] = 0;
    uInt maxCheck = 0;
    for (int i = GRIDYMAX; i >= 0; i--)
    {
        bool delete = true;
        for (uInt j = 0; j < GRIDXMAX; j++)
        {
            uInt Pos = (i * GRIDXMAX) + j;
            if(!grid[Pos].filled){delete = false;}
        }
        if(delete)
        {
            deletedLines[linesDeleted] = i;
            linesDeleted++;
        }
        if(linesDeleted > 0){maxCheck++;}
        if(maxCheck >= 4){break;}
    }
    if (deletedLines > 0)
    {
        DeleteLinesAndScore(linesDeleted);
    }
    
}

static void DrawBackground()
{
    float boxWidth = 5;
    DrawRectangleLinesEx((Rectangle){gridOffsetX-boxWidth, gridOffsetY-boxWidth, 
                         (BLOCKSIZE*GRIDXMAX)+boxWidth*2, (BLOCKSIZE*GRIDYMAX)+boxWidth*2}, 5.0f, RED);
}

///////////////////////////////////////////////////
// Program Entry //////////////////////////////////
///////////////////////////////////////////////////
int main()
{
    InitWindow(800, 800, "Tetris");
    SetExitKey(KEY_NULL);
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
        
        HandlePlayerInput();
        if (((state & PAUSED) != PAUSED) && ((state & GAME_OVER) != GAME_OVER))
        {
            tickCount += GetFrameTime();
            if (tickCount >= tickSpeed)
            {
                GameUpdate();
            }
        }
        BeginDrawing();
            ClearBackground(BLACK);
            DrawBackground();
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