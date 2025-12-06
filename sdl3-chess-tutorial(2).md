# SDL3 2D Chess Tutorial - Detailed Multi-File Guide

## Understanding SDL3 Basics

**SDL (Simple DirectMedia Layer)** is a library that gives you direct access to:
- **Window creation** (like JFrame in Swing)
- **Graphics rendering** (like Graphics2D in Swing)
- **Input handling** (mouse, keyboard)
- **Image loading** (PNG, JPG files)

Unlike Swing which automatically redraws when needed, in SDL **you control everything manually**.

---

## Project Structure

```
chess-game/
├── main.cpp              # SDL entry point
├── Game.h                # Game controller header
├── Game.cpp              # Game logic
├── Board.h               # Board header
├── Board.cpp             # Board rendering
├── Pieces.h              # Piece definitions header
├── Pieces.cpp            # Movement rules
├── UI.h                  # UI header
├── UI.cpp                # UI drawing
├── assets/               # Your piece images
│   ├── white_pawn.png
│   ├── white_rook.png
│   └── ... (all pieces)
└── bin/                  # Compiled output
```

---

## Pieces.h - Piece Type Definitions

This is like having enum classes in Java for your piece types.

```cpp
#ifndef PIECES_H
#define PIECES_H

// Piece types - similar to an enum in Java
// EMPTY = 0 represents an empty square on the board
enum PieceType {
    EMPTY = 0,
    WHITE_PAWN, WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING
};

// Color enum - makes it easier to check whose turn it is
enum PieceColor {
    COLOR_NONE,   // For empty squares
    COLOR_WHITE,
    COLOR_BLACK
};

// ============================================
// UTILITY FUNCTIONS
// ============================================

// Returns the filename for the piece image (e.g., "white_pawn")
// Similar to: String getPieceImage(PieceType piece) in Java
const char* GetPieceTextureName(PieceType piece);

// Gets the color of a piece (white, black, or none)
// Similar to: PieceColor getColor(PieceType piece) in Java
PieceColor GetPieceColor(PieceType piece);

// Check what type of piece we have
// These are like: boolean isPawn(Piece p) in Java
bool IsPawn(PieceType piece);
bool IsRook(PieceType piece);
bool IsKnight(PieceType piece);
bool IsBishop(PieceType piece);
bool IsQueen(PieceType piece);
bool IsKing(PieceType piece);

// ============================================
// MOVE VALIDATION FUNCTIONS
// ============================================

// Main validation function - checks if a move is legal
// board: the current state of the chess board (8x8 array)
// from/to Row/Col: where the piece is moving from and to
bool IsValidMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);

// Individual piece movement rules
bool IsValidPawnMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidRookMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidKnightMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidBishopMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidQueenMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);
bool IsValidKingMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol);

#endif
```

---

## Pieces.cpp - Movement Logic Implementation

```cpp
#include "Pieces.h"
#include <cmath>  // For abs() function

// ============================================
// UTILITY FUNCTION IMPLEMENTATIONS
// ============================================

// Maps piece types to their image filenames
// This is like having a HashMap<PieceType, String> in Java
const char* GetPieceTextureName(PieceType piece) {
    switch(piece) {
        case WHITE_PAWN: return "white_pawn";
        case WHITE_ROOK: return "white_rook";
        case WHITE_KNIGHT: return "white_knight";
        case WHITE_BISHOP: return "white_bishop";
        case WHITE_QUEEN: return "white_queen";
        case WHITE_KING: return "white_king";
        case BLACK_PAWN: return "black_pawn";
        case BLACK_ROOK: return "black_rook";
        case BLACK_KNIGHT: return "black_knight";
        case BLACK_BISHOP: return "black_bishop";
        case BLACK_QUEEN: return "black_queen";
        case BLACK_KING: return "black_king";
        default: return nullptr;  // nullptr is like null in Java
    }
}

// Determines which color a piece belongs to
PieceColor GetPieceColor(PieceType piece) {
    // White pieces are numbered 1-6, black pieces are 7-12
    if (piece >= WHITE_PAWN && piece <= WHITE_KING) return COLOR_WHITE;
    if (piece >= BLACK_PAWN && piece <= BLACK_KING) return COLOR_BLACK;
    return COLOR_NONE;  // Empty square
}

// Type checking functions - simple comparisons
bool IsPawn(PieceType piece) {
    return piece == WHITE_PAWN || piece == BLACK_PAWN;
}

bool IsRook(PieceType piece) {
    return piece == WHITE_ROOK || piece == BLACK_ROOK;
}

bool IsKnight(PieceType piece) {
    return piece == WHITE_KNIGHT || piece == BLACK_KNIGHT;
}

bool IsBishop(PieceType piece) {
    return piece == WHITE_BISHOP || piece == BLACK_BISHOP;
}

bool IsQueen(PieceType piece) {
    return piece == WHITE_QUEEN || piece == BLACK_QUEEN;
}

bool IsKing(PieceType piece) {
    return piece == WHITE_KING || piece == BLACK_KING;
}

// ============================================
// PAWN MOVEMENT RULES
// ============================================
// Pawns: move forward 1 square, or 2 from start position
// Can capture diagonally one square
bool IsValidPawnMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    PieceType piece = board[fromRow][fromCol];
    PieceType target = board[toRow][toCol];
    
    // White pawns move up (-1), black pawns move down (+1)
    int direction = (piece == WHITE_PAWN) ? -1 : 1;
    
    // Starting row for two-square moves
    int startRow = (piece == WHITE_PAWN) ? 6 : 1;
    
    // RULE 1: Move forward one square (must be empty)
    if (toCol == fromCol && toRow == fromRow + direction && target == EMPTY) {
        return true;
    }
    
    // RULE 2: Move forward two squares from starting position
    // Both squares must be empty
    if (fromRow == startRow && toCol == fromCol && toRow == fromRow + 2*direction 
        && target == EMPTY && board[fromRow + direction][fromCol] == EMPTY) {
        return true;
    }
    
    // RULE 3: Capture diagonally (target must have an enemy piece)
    if (abs(toCol - fromCol) == 1 && toRow == fromRow + direction && target != EMPTY) {
        return true;
    }
    
    return false;
}

// ============================================
// ROOK MOVEMENT RULES
// ============================================
// Rooks: move in straight lines (horizontal or vertical) any distance
// Cannot jump over pieces
bool IsValidRookMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    // Must move in straight line (either row OR column changes, not both)
    if (fromRow != toRow && fromCol != toCol) return false;
    
    // Calculate direction of movement
    // If toRow > fromRow, we're moving down (+1)
    // If toRow < fromRow, we're moving up (-1)
    // If toRow == fromRow, we're not moving vertically (0)
    int rowDir = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
    int colDir = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;
    
    // Start from the square after our piece
    int currentRow = fromRow + rowDir;
    int currentCol = fromCol + colDir;
    
    // Check every square along the path
    // Stop when we reach the destination square
    while (currentRow != toRow || currentCol != toCol) {
        // If we hit a piece, the path is blocked
        if (board[currentRow][currentCol] != EMPTY) return false;
        
        // Move to next square in the path
        currentRow += rowDir;
        currentCol += colDir;
    }
    
    return true;  // Path is clear!
}

// ============================================
// KNIGHT MOVEMENT RULES
// ============================================
// Knights: move in an L-shape (2 squares in one direction, 1 in perpendicular)
// Knights can jump over other pieces
bool IsValidKnightMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    int rowDiff = abs(toRow - fromRow);  // How many rows moved?
    int colDiff = abs(toCol - fromCol);  // How many columns moved?
    
    // L-shape: (2,1) or (1,2)
    // Examples: move 2 up and 1 right, or 1 down and 2 left, etc.
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

// ============================================
// BISHOP MOVEMENT RULES
// ============================================
// Bishops: move diagonally any distance
// Cannot jump over pieces
bool IsValidBishopMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    // Must move diagonally - row change must equal column change
    // Example: moving from (0,0) to (3,3) - both changed by 3
    if (abs(toRow - fromRow) != abs(toCol - fromCol)) return false;
    
    // Calculate diagonal direction
    int rowDir = (toRow > fromRow) ? 1 : -1;  // Moving down or up?
    int colDir = (toCol > fromCol) ? 1 : -1;  // Moving right or left?
    
    // Check path is clear (similar to rook)
    int currentRow = fromRow + rowDir;
    int currentCol = fromCol + colDir;
    
    while (currentRow != toRow || currentCol != toCol) {
        if (board[currentRow][currentCol] != EMPTY) return false;
        currentRow += rowDir;
        currentCol += colDir;
    }
    
    return true;
}

// ============================================
// QUEEN MOVEMENT RULES
// ============================================
// Queen: combines rook and bishop movement
// Can move straight OR diagonally
bool IsValidQueenMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    // Queen can move like a rook OR like a bishop
    return IsValidRookMove(board, fromRow, fromCol, toRow, toCol) ||
           IsValidBishopMove(board, fromRow, fromCol, toRow, toCol);
}

// ============================================
// KING MOVEMENT RULES
// ============================================
// King: moves one square in any direction
bool IsValidKingMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    int rowDiff = abs(toRow - fromRow);
    int colDiff = abs(toCol - fromCol);
    
    // Can only move one square (horizontally, vertically, or diagonally)
    return rowDiff <= 1 && colDiff <= 1;
}

// ============================================
// MAIN VALIDATION FUNCTION
// ============================================
// This is called whenever a player tries to move a piece
bool IsValidMove(PieceType board[8][8], int fromRow, int fromCol, int toRow, int toCol) {
    PieceType piece = board[fromRow][fromCol];
    PieceType target = board[toRow][toCol];
    
    // RULE: Can't move to same square
    if (fromRow == toRow && fromCol == toCol) return false;
    
    // RULE: Can't capture your own piece
    if (target != EMPTY) {
        if (GetPieceColor(piece) == GetPieceColor(target)) return false;
    }
    
    // Check piece-specific movement rules
    if (IsPawn(piece)) return IsValidPawnMove(board, fromRow, fromCol, toRow, toCol);
    if (IsRook(piece)) return IsValidRookMove(board, fromRow, fromCol, toRow, toCol);
    if (IsKnight(piece)) return IsValidKnightMove(board, fromRow, fromCol, toRow, toCol);
    if (IsBishop(piece)) return IsValidBishopMove(board, fromRow, fromCol, toRow, toCol);
    if (IsQueen(piece)) return IsValidQueenMove(board, fromRow, fromCol, toRow, toCol);
    if (IsKing(piece)) return IsValidKingMove(board, fromRow, fromCol, toRow, toCol);
    
    return false;  // Unknown piece type
}
```

---

## Board.h - Board Class Definition

```cpp
#ifndef BOARD_H
#define BOARD_H

#include <SDL3/SDL.h>
#include <map>
#include <string>
#include "Pieces.h"

// The Board class manages:
// 1. The 8x8 grid of pieces
// 2. Loading piece images
// 3. Drawing the chessboard
// 4. Drawing the pieces on the board
class Board {
public:
    Board();   // Constructor - sets up initial state
    ~Board();  // Destructor - cleans up memory
    
    // Initialize SDL image loading and load all piece textures
    // renderer: SDL_Renderer is needed to load images into GPU memory
    bool Initialize(SDL_Renderer* renderer);
    
    // Free all loaded textures and clean up
    void Cleanup();
    
    // Reset board to starting chess position
    void Reset();
    
    // Draw the 8x8 chessboard pattern
    // selectedRow/Col: highlights a square if piece is selected
    void Draw(SDL_Renderer* renderer, int selectedRow, int selectedCol, bool pieceSelected);
    
    // Draw all pieces on their current squares
    void DrawPieces(SDL_Renderer* renderer);
    
    // Get the piece at a specific position
    PieceType GetPiece(int row, int col) const;
    
    // Place a piece at a specific position
    void SetPiece(int row, int col, PieceType piece);
    
    // Move a piece from one square to another
    void MovePiece(int fromRow, int fromCol, int toRow, int toCol);
    
    // Constants - public so other classes can use them
    static const int SQUARE_SIZE = 100;  // Each square is 100x100 pixels
    static const int BOARD_SIZE = 8;     // 8x8 chess board
    
private:
    // The actual board state - 2D array of piece types
    PieceType board[8][8];
    
    // Map of piece names to their loaded textures
    // Similar to: HashMap<String, Image> in Java
    // Textures are GPU-optimized images for fast drawing
    std::map<std::string, SDL_Texture*> pieceTextures;
    
    // Load all 12 piece images (6 white + 6 black)
    bool LoadPieceTextures(SDL_Renderer* renderer);
    
    // Load a single image file and convert it to a texture
    SDL_Texture* LoadTexture(SDL_Renderer* renderer, const char* path);
};

#endif
```

---

## Board.cpp - Board Implementation

```cpp
#include "Board.h"
#include <SDL3/SDL_image.h>  // For loading PNG images

// ============================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================

Board::Board() {
    Reset();  // Start with pieces in starting positions
}

Board::~Board() {
    Cleanup();  // Free memory when board is destroyed
}

// ============================================
// INITIALIZATION
// ============================================

bool Board::Initialize(SDL_Renderer* renderer) {
    // Initialize SDL_image library for PNG support
    // IMG_INIT_PNG tells it we want to load PNG files
    if (!IMG_Init(IMG_INIT_PNG)) {
        SDL_Log("IMG_Init failed: %s", SDL_GetError());
        return false;
    }
    
    // Load all piece images into memory
    return LoadPieceTextures(renderer);
}

void Board::Cleanup() {
    // Free all loaded textures from GPU memory
    // Similar to: for (Image img : images) img.flush() in Java
    for (auto& pair : pieceTextures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);  // Free GPU memory
        }
    }
    pieceTextures.clear();  // Clear the map
    IMG_Quit();  // Shutdown SDL_image
}

// ============================================
// BOARD SETUP
// ============================================

void Board::Reset() {
    // Clear entire board to EMPTY
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = EMPTY;
        }
    }
    
    // Place BLACK pieces (top of board, rows 0-1)
    // Row 0: Back rank (Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook)
    board[0][0] = BLACK_ROOK;
    board[0][1] = BLACK_KNIGHT;
    board[0][2] = BLACK_BISHOP;
    board[0][3] = BLACK_QUEEN;
    board[0][4] = BLACK_KING;
    board[0][5] = BLACK_BISHOP;
    board[0][6] = BLACK_KNIGHT;
    board[0][7] = BLACK_ROOK;
    
    // Row 1: Pawns
    for (int i = 0; i < 8; i++) {
        board[1][i] = BLACK_PAWN;
    }
    
    // Place WHITE pieces (bottom of board, rows 6-7)
    // Row 7: Back rank
    board[7][0] = WHITE_ROOK;
    board[7][1] = WHITE_KNIGHT;
    board[7][2] = WHITE_BISHOP;
    board[7][3] = WHITE_QUEEN;
    board[7][4] = WHITE_KING;
    board[7][5] = WHITE_BISHOP;
    board[7][6] = WHITE_KNIGHT;
    board[7][7] = WHITE_ROOK;
    
    // Row 6: Pawns
    for (int i = 0; i < 8; i++) {
        board[6][i] = WHITE_PAWN;
    }
}

// ============================================
// TEXTURE LOADING
// ============================================

// Load a single image file and convert to SDL_Texture
SDL_Texture* Board::LoadTexture(SDL_Renderer* renderer, const char* path) {
    // STEP 1: Load image from disk into CPU memory (SDL_Surface)
    // SDL_Surface is like BufferedImage in Java - raw pixel data
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        SDL_Log("Failed to load image %s: %s", path, SDL_GetError());
        return nullptr;
    }
    
    // STEP 2: Convert surface to texture (GPU memory)
    // Textures are optimized for fast drawing by the graphics card
    // This is like uploading the image to the GPU
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // STEP 3: Free the surface - we don't need CPU memory anymore
    // The texture now lives in GPU memory
    SDL_DestroySurface(surface);
    
    return texture;
}

bool Board::LoadPieceTextures(SDL_Renderer* renderer) {
    // Load all 12 piece images (6 white pieces + 6 black pieces)
    // Store them in a map for easy lookup by name
    
    // White pieces
    pieceTextures["white_pawn"] = LoadTexture(renderer, "assets/white_pawn.png");
    pieceTextures["white_rook"] = LoadTexture(renderer, "assets/white_rook.png");
    pieceTextures["white_knight"] = LoadTexture(renderer, "assets/white_knight.png");
    pieceTextures["white_bishop"] = LoadTexture(renderer, "assets/white_bishop.png");
    pieceTextures["white_queen"] = LoadTexture(renderer, "assets/white_queen.png");
    pieceTextures["white_king"] = LoadTexture(renderer, "assets/white_king.png");
    
    // Black pieces
    pieceTextures["black_pawn"] = LoadTexture(renderer, "assets/black_pawn.png");
    pieceTextures["black_rook"] = LoadTexture(renderer, "assets/black_rook.png");
    pieceTextures["black_knight"] = LoadTexture(renderer, "assets/black_knight.png");
    pieceTextures["black_bishop"] = LoadTexture(renderer, "assets/black_bishop.png");
    pieceTextures["black_queen"] = LoadTexture(renderer, "assets/black_queen.png");
    pieceTextures["black_king"] = LoadTexture(renderer, "assets/black_king.png");
    
    // Verify all textures loaded successfully
    for (const auto& pair : pieceTextures) {
        if (!pair.second) {
            SDL_Log("Failed to load texture: %s", pair.first.c_str());
            return false;
        }
    }
    
    return true;
}

// ============================================
// RENDERING
// ============================================

void Board::Draw(SDL_Renderer* renderer, int selectedRow, int selectedCol, bool pieceSelected) {
    // Draw all 64 squares of the chessboard
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            // Calculate pixel position for this square
            // SDL_FRect is like Rectangle2D.Float in Java
            SDL_FRect rect = {
                (float)(col * SQUARE_SIZE),      // X position
                (float)(row * SQUARE_SIZE),      // Y position
                (float)SQUARE_SIZE,              // Width
                (float)SQUARE_SIZE               // Height
            };
            
            // Choose color for this square
            // Highlight selected square in yellow
            if (pieceSelected && row == selectedRow && col == selectedCol) {
                // Yellow highlight for selected piece
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            }
            // Alternating light and dark squares
            else if ((row + col) % 2 == 0) {
                // Light squares (beige)
                SDL_SetRenderDrawColor(renderer, 240, 217, 181, 255);
            } else {
                // Dark squares (brown)
                SDL_SetRenderDrawColor(renderer, 181, 136, 99, 255);
            }
            
            // Fill the square with the chosen color
            // Similar to: g.fillRect(x, y, width, height) in Java
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void Board::DrawPieces(SDL_Renderer* renderer) {
    // Loop through entire board and draw each piece
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            PieceType piece = board[row][col];
            
            // Skip empty squares
            if (piece == EMPTY) continue;
            
            // Get the texture name for this piece (e.g., "white_pawn")
            const char* textureName = GetPieceTextureName(piece);
            
            // Look up the texture in our map
            SDL_Texture* texture = pieceTextures[textureName];
            
            if (texture) {
                // Calculate where to draw this piece
                SDL_FRect destRect = {
                    (float)(col * SQUARE_SIZE),  // X position
                    (float)(row * SQUARE_SIZE),  // Y position
                    (float)SQUARE_SIZE,          // Width (scale to fit square)
                    (float)SQUARE_SIZE           // Height
                };
                
                // Draw the texture at this position
                // nullptr for source rect means "draw entire image"
                // Similar to: g.drawImage(img, x, y, width, height, null) in Java
                SDL_RenderTexture(renderer, texture, nullptr, &destRect);
            }
        }
    }
}

// ============================================
// BOARD STATE ACCESS
// ============================================

PieceType Board::GetPiece(int row, int col) const {
    // Bounds checking - return EMPTY if coordinates are invalid
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return EMPTY;
    return board[row][col];
}

void Board::SetPiece(int row, int col, PieceType piece) {
    // Bounds checking before setting
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        board[row][col] = piece;
    }
}

void Board::MovePiece(int fromRow, int fromCol, int toRow, int toCol) {
    // Copy piece to new location
    board[toRow][toCol] = board[fromRow][fromCol];
    
    // Clear old location
    board[fromRow][fromCol] = EMPTY;
}
```

---

## UI.h - User Interface Elements

```cpp
#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>
#include "Pieces.h"

// UI class provides visual feedback to the player
// All functions are static because UI doesn't need to store state
class UI {
public:
    // Draw a box showing whose turn it is (white or black)
    static void DrawTurnIndicator(SDL_Renderer* renderer, PieceColor currentTurn);
    
    // Draw green circles on squares where selected piece can move
    static void DrawValidMoves(SDL_Renderer* renderer, PieceType board[8][8], 
                               int selectedRow, int selectedCol);
    
    // Draw "Game Over" message when game ends
    static void DrawGameOver(SDL_Renderer* renderer, PieceColor winner);
};

#endif
```

---

## UI.cpp - UI Implementation

```cpp
#include "UI.h"
#include "Board.h"

void UI::DrawTurnIndicator(SDL_Renderer* renderer, PieceColor currentTurn) {
    // Draw a colored box in top-left corner showing current turn
    SDL_FRect indicator = {10, 10, 100, 40};  // Position and size
    
    // Choose color based on current turn
    if (currentTurn == COLOR_WHITE) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);     // Dark gray/black
    }
    
    // Fill the indicator box
    SDL_RenderFillRect(renderer, &indicator);
    
    // Draw black border around the box
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderRect(renderer, &indicator);
}

void UI::DrawValidMoves(SDL_Renderer* renderer, PieceType board[8][8], 
                       int selectedRow, int selectedCol) {
    // If no piece is selected, don't draw anything
    if (selectedRow < 0 || selectedCol < 0) return;
    
    // Check every square on the board
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            // If this is a valid move destination, draw a green circle
            if (IsValidMove(board, selectedRow, selectedCol, row, col)) {
                // Draw a small rectangle in the center of the square
                // (Could be enhanced to draw actual circles)
                SDL_FRect rect = {
                    (float)(col * Board::SQUARE_SIZE + Board::SQUARE_SIZE/2 - 15),
                    (float)(row * Board::SQUARE_SIZE + Board::SQUARE_SIZE/2 - 15),
                    30, 30
                };
                
                // Semi-transparent green (alpha = 128)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 128);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void UI::DrawGameOver(SDL_Renderer* renderer, PieceColor winner) {
    // Draw a semi-transparent overlay in center of screen
    SDL_FRect overlay = {200, 300, 400, 200};
    
    // Semi-transparent black background (alpha = 200)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_RenderFillRect(renderer, &overlay);
    
    // White border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &overlay);
    
    // TODO: Add text rendering for "White Wins!" or "Black Wins!"
    // (Requires SDL_ttf library for text)
}
```

---

## Game.h - Main Game Controller

```cpp
#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include "Board.h"
#include "Pieces.h"

// Game class controls everything:
// - Creating the window
// - Managing game state (whose turn, selected piece)
// - Handling input
// - Rendering the game
class Game {
public:
    Game();   // Constructor
    ~Game();  // Destructor
    
    // Create window, renderer, and initialize everything
    bool Initialize();
    
    // Process an SDL event (mouse click, window close, etc.)
    void HandleEvent(SDL_Event* event);
    
    // Update game logic (currently unused, but needed for animations later)
    void Update();
    
    // Draw everything to the screen
    void Render();
    
    // Clean up and shut down
    void Cleanup();
    
    // Check if game is still running
    bool IsRunning() const { return running; }
    
    // Get SDL window and renderer (needed by main.cpp)
    SDL_Window* GetWindow() const { return window; }
    SDL_Renderer* GetRenderer() const { return renderer; }
    
private:
    // SDL objects
    SDL_Window* window;      // The game window (like JFrame)
    SDL_Renderer* renderer;  // The drawing context (like Graphics2D)
    
    // Game components
    Board board;  // The chess board
    
    // Game state
    bool running;              // Is the game still running?
    PieceColor currentTurn;    // Whose turn is it? (white or black)
    
    // Selection state (for click-to-move)
    int selectedRow;     // Row of selected piece (-1 if none)
    int selectedCol;     // Column of selected piece (-1 if none)
    bool pieceSelected;  // Is a piece currently selected?
    
    // Handle a mouse click at pixel coordinates
    void HandleMouseClick(int mouseX, int mouseY);
    
    // Switch from white's turn to black's turn (or vice versa)
    void SwitchTurn();
};

#endif