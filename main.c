#include <stdio.h>
#include <stdint.h>

// Both players have their indices. Index 0 is 'o' player and index 1 is 'x' player
char sign[2] = {'o', 'x'};

enum PlayerType{Human, AI};
enum GameState{Playing, HumanWon, ComputerWon, Draw};

const uint16_t ALL_SQUARES_OCCUPIED = 0b0000000111111111;

const uint16_t ROW_1 = 0b0000000000000111;
const uint16_t ROW_2 = 0b0000000000111000;
const uint16_t ROW_3 = 0b0000000111000000;

const uint16_t COL_1 = 0b0000000001001001;
const uint16_t COL_2 = 0b0000000010010010;
const uint16_t COL_3 = 0b0000000100100100;

const uint16_t LEFT_DIAG = 0b0000000100010001;
const uint16_t RIGHT_DIAG = 0b0000000001010100;

uint16_t WinningConditions[8] = {
        ROW_1, ROW_2, ROW_3,
        COL_1, COL_2, COL_3,
        LEFT_DIAG, RIGHT_DIAG
};

#define GetBit(bitboard, index) (bitboard & (1 << index))
#define SetBit(bitboard, index) (bitboard |= (1 << index))
#define ClearBit(bitboard, index) (bitboard ^= (1 << index))

typedef struct {
    enum PlayerType playerType;
    int index;
    uint16_t board;
} player;

void DrawBoard(char board[]) {
    printf(" %c | %c | %c \n", board[0], board[1], board[2]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[3], board[4], board[5]);
    printf("---|---|---\n");
    printf(" %c | %c | %c \n", board[6], board[7], board[8]);
    printf("\n");
}

void MakeMove(char board[], player *Player, int squareIndex){
    SetBit(Player->board, squareIndex);
    board[squareIndex] = sign[Player->index];
}

int CheckWinningConditions(uint16_t board){
    for(int i=0; i<8; i++){
        if ((board & WinningConditions[i]) == WinningConditions[i]){
            return 1;
        }
    }
    return 0;
}

int IsDraw(uint16_t board){
    return (board == ALL_SQUARES_OCCUPIED);
}

int Evaluate(int player1, int player2, int depth){
    if (CheckWinningConditions(player1)){
        return 10-depth;
    }
    if (CheckWinningConditions(player2)){
        return depth-10;
    }
    return 0;
}

int Search(uint16_t player1, uint16_t player2, int depth, int alpha, int beta, int isComputerMove){
    if (depth > 0) {
        int score = Evaluate(player1, player2, depth);
        if (score){
            return score;
        }
        else if (IsDraw(player1 | player2)) {
            return 0;
        }
    }

    int BestMove = -1;
    uint16_t currentBoard = (player1 | player2);
    for (int index=0; index<9; index++){
        if (!GetBit(currentBoard, index)) {
            SetBit(player1, index);
            int evalScore = -Search(player2, player1, depth+1, -beta, -alpha, 1-isComputerMove);
            ClearBit(player1, index);

            if (evalScore >= beta){
                if (depth == 0){
                    return BestMove;
                }
                return beta;
            }

            if (evalScore > alpha) {
                alpha = evalScore;
                if (depth == 0) {
                    BestMove = index;
                }
            }
        }
    }

    if (depth == 0) {
        return BestMove;
    }
    return alpha;
}

enum GameState GetGameState(player *lastTurnPlayer, player *nextTurnPlayer){
    if (CheckWinningConditions(lastTurnPlayer->board)){
        if (lastTurnPlayer->playerType == Human){
            return HumanWon;
        }
        else{
            return ComputerWon;
        }
    }

    if (IsDraw(lastTurnPlayer->board | nextTurnPlayer->board)){
        return Draw;
    }
    return Playing;
}

void InitializePlayers(player *HumanPlayer, player *AIPlayer, int humanPlayerIndex){
    HumanPlayer->playerType = Human;
    AIPlayer->playerType = AI;

    HumanPlayer->index = humanPlayerIndex;
    AIPlayer->index = 1 - humanPlayerIndex;

    HumanPlayer->board = 0b0000000000000000;
    AIPlayer->board = 0b0000000000000000;
}

void StartGame(player *HumanPlayer, player *AIPlayer){
    int humanPlayerIndex;
    printf("TicTacToe Game\n");
    printf("1 - play as o\n");
    printf("2 - play as x\n");
    printf("Your choice: ");
    scanf("%d", &humanPlayerIndex);
    printf("\n");
    humanPlayerIndex--;

    InitializePlayers(HumanPlayer, AIPlayer, humanPlayerIndex);
}

void MainLoop(player *HumanPlayer, player *AIPlayer, char board[]){
    enum GameState gameState = Playing;
    player *playerToMove = (HumanPlayer->index == 1) ? HumanPlayer : AIPlayer;

    while (gameState == Playing){
        DrawBoard(board);
        int squareToMove;
        if (playerToMove->playerType == HumanPlayer->playerType) {
            while (1) {
                printf("Choose your move: ");
                scanf("%d", &squareToMove);
                printf("\n");
                squareToMove--;
                if (squareToMove >= 0 && squareToMove < 9){
                    if (board[squareToMove] != sign[0] && board[squareToMove] != sign[1]) {
                        break;
                    }
                }
                printf("This move is wrong, choose again\n");
            }
        }
        else{
            squareToMove = Search(AIPlayer->board, HumanPlayer->board, 0, -1000, 1000, 1);
            printf("Computer move is: %d\n\n", squareToMove+1);
        }
        MakeMove(board, playerToMove, squareToMove);

        player *nextTurnPlayer = (playerToMove->playerType == Human) ? AIPlayer : HumanPlayer;
        gameState = GetGameState(playerToMove, nextTurnPlayer);
        playerToMove = nextTurnPlayer;
    }
    DrawBoard(board);
    if (gameState == HumanWon){
        printf("Human won the game!");
    }
    else if (gameState == ComputerWon){
        printf("Computer won the game!");
    }
    else{
        printf("Draw!");
    }
}

int main() {
    char board[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    player HumanPlayer;
    player AIPlayer;

    StartGame(&HumanPlayer, &AIPlayer);
    MainLoop(&HumanPlayer, &AIPlayer, board);

    return 0;
}
