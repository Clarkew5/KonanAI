#include "game.h"

Board *makeBoard(char *filepath){
    FILE *fp = fopen(filepath, "r");
    Board *b = malloc(sizeof(Board));
    if (b == NULL) exit(-1);
    char line[8];
    bool bwFlag = false;
    for (int j = 0; j < 8; j++){
        fscanf(fp, "%s", line);
        b->board[j] = 0;
        for (int i = 0; i < 8; i++){
            if (line[i] != 'O'){
                b->board[j] += (1<<(7-i));
                if (!bwFlag){
                    if ((i%2 == j%2 && line[i] == 'B') || 
                        (i%2 != j%2 && line[i] == 'W'))
                        b->blackThenWhite = true;
                    else
                        b->blackThenWhite = false;
                    bwFlag = true;
                }
            }
        }
    }
    fclose(fp);
    return b;        
}

int printBoard(Board *b){
    fprintf(stderr, "  ABCDEFGH\n");
    fprintf(stderr, " +--------+\n");
    for (int j = 0; j < 8; j++){
        fprintf(stderr, "%d|", 8-j);
        for (int i = 0; i < 8; i++){
            if ((b->board[j] & (1<<(7-i))) == (1<<(7-i))){
                if (b->blackThenWhite && i%2 == j%2) fprintf(stderr, "B");
                else if (b->blackThenWhite && i%2 != j%2) fprintf(stderr, "W");
                else if (!b->blackThenWhite && i%2 == j%2) fprintf(stderr, "B");
                else fprintf(stderr, "W");
            }
            else fprintf(stderr, "_");
        }
        fprintf(stderr, "|\n");
    }
    fprintf(stderr, " +--------+\n");
    return 0;
}


int makeMove(char *move, Board *b){
    char *colLetters = "ABCDEFGH";   
    char *rowNumbers = "87654321";

    int i1 = strchr(colLetters, move[0]) - colLetters;
    int j1 = strchr(rowNumbers, move[1]) - rowNumbers;
    if (strlen(move) == 5){
        int i2 = strchr(colLetters, move[3]) - colLetters;
        int j2 = strchr(rowNumbers, move[4]) - rowNumbers;
        b->board[j2] ^= (1<<(7-i2));
        int jumps = (i1-i2)+(j1-j2);
        if (jumps < 0) jumps *= -1;
        for(int jump = 1; jump < jumps; jump+=2){
            int i3;
            if (i1 > i2) i3 = i2+jump;
            else if (i1 < i2) i3 = i1+jump;
            else i3 = i1;
            int j3;
            if (j1 > j2) j3 = j2+jump;
            else if (j1 < j2) j3 = j1+jump;
            else j3 = j1;
            b->board[j3] ^= (1<<(7-i3));
        }
    }
    b->board[j1] ^= (1<<(7-i1));
    return 0;
}

int numberOfMoves(Board *b, char turn){
    int numberOfMoves = 0;
    int boardSum = 0;
    for (int i = 0; i < 8; i++){
        boardSum += b->board[i];
    }
    if (boardSum == 2040)
        numberOfMoves = 2;
    else if ((boardSum == 2024 || boardSum == 2032) && turn == 'W' )
        numberOfMoves = 2;
    else{
        int starti;
        if ((b->blackThenWhite && turn == 'B') ||
           (!b->blackThenWhite && turn == 'W'))
            starti = 0;
        else
            starti = 1;
        for (int j = 0; j < 2; j++){
            int8_t checkDown = b->board[j] & b->board[j+1] & ~b->board[j+2];
            int8_t checkDown2 = b->board[j+3] & ~b->board[j+4];
            for (int i = starti; i < 6; i+=2){
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(5-i))) == (7<<(5-i))){
                    numberOfMoves++; //right jump
                    if ((i < 4) && ((((b->board[j]) & (3<<(3-i))) ^ (1<<(3-i))) == (3<<(3-i)))){
                        numberOfMoves++; //right double jump
                    }
                }
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(7-i))) == (7<<(5-i))){
                    numberOfMoves++; //left jump
                    //if ((i > 1 && i < 6) && ((((b->board[j]) & (3<<(6-i))) ^ (2<<(6-i))) == (3<<(6-i)))){
                        //numberOfMoves++; //left double jump
                    //}
                }
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //down jump
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //down double jump
                    }
                }
            }
            for (int i = starti + 6; i < 8; i+=2){
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //down jump
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //down double jump
                    }
                }
            }
            starti ^= 1;
        }
        for (int j = 2; j < 6; j++){ 
            int8_t checkDown = b->board[j] & b->board[j+1] & ~b->board[j+2];
            int8_t checkDown2 = 0;
            if(j+4 < 8)
                 checkDown2 = b->board[j+3] & ~b->board[j+4];
            int8_t checkAbove = b->board[j] & b->board[j-1] & ~b->board[j-2];
            int8_t checkAbove2 = 0;
            if((j-3) > 0)
                 checkAbove2 = b->board[j-3] & ~b->board[j-4];
            for (int i = starti; i < 6; i+=2){
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(5-i))) == (7<<(5-i))){
                    numberOfMoves++; //right jump
                    if ((i < 4) && (((b->board[j]) & (3<<(3-i))) ^ (1<<(3-i))) == (3<<(3-i))){
                        numberOfMoves++; //right double jump
                    }
                }
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(7-i))) == (7<<(5-i))){
                    numberOfMoves++; //left jump
                    //if ((i > 1 && i < 6) && ((((b->board[j]) & (3<<(6-i))) ^ (2<<(6-i))) == (3<<(6-i)))){
                        //numberOfMoves++; //left double jump
                    //}
                }
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //down jump
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //down double jump
                    }
                }
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //up jump
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //up double jump
                    }
                }
            }
            for (int i = starti + 6; i < 8; i+=2){
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //down jump
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //down double jump
                    }
                }
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //up jump
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //up double jump
                    }
                }
            }
            starti ^= 1;
        }
        for (int j = 6; j < 8 ; j++){
            int8_t checkAbove = b->board[j] & b->board[j-1] & ~b->board[j-2];
            int8_t checkAbove2 = b->board[j-3] & ~b->board[j-4];
            for (int i = starti; i < 6; i+=2){
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(5-i))) == (7<<(5-i))){
                    numberOfMoves++; //right jump
                    if ((i < 4) && (((b->board[j]) & (3<<(3-i))) ^ (1<<(3-i))) == (3<<(3-i))){
                        numberOfMoves++; //right double jump
                    }
                }
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(7-i))) == (7<<(5-i))){
                    numberOfMoves++; //left jump
                    //if ((i > 1 && i < 6) && ((((b->board[j]) & (3<<(6-i))) ^ (2<<(6-i))) == (3<<(6-i)))){
                        //numberOfMoves++; //left double jump
                    //}
                }
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //up jump
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //up double jump
                    }
                }
            }
            for (int i = starti + 6; i < 8; i+=2){
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    numberOfMoves++; //up jump
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        numberOfMoves++; //up double jump
                    }
                }
            }
            starti ^= 1;
        }
    }
    return numberOfMoves;
}

char *makeMoveNotation(int i1, int j1, int i2, int j2){
    char *move = calloc(6, sizeof(char));
    char *colLetters = "ABCDEFGH";   
    char *rowNumbers = "87654321";
    //if (i1 < 0) i1 += 8;
    //else if (i1 > 8) i1 -= 8;
    //if (j1 < 0) j1 += 8;
    //else if (j1 > 8) j1 -= 8;  
    if (i2 < 0) i2 += 8;
    else if (i2 > 8) i2 -= 8;
    if (j2 < 0) j2 += 8;
    else if (j2 > 8) j2 -= 8; 
    move[0] = colLetters[i1];
    move[1] = rowNumbers[j1];
    move[2] = '-';
    move[3] = colLetters[i2];
    move[4] = rowNumbers[j2];
    return move; 
}

char *validMoves(Board *b, char turn){

    int boardSum = 0;
    for (int i = 0; i < 8; i++){
        boardSum += b->board[i];
    }
    if (boardSum == 2040){
        char *moves = calloc(5, sizeof(char));
        if (b->blackThenWhite){
            strcat(moves, "D5E4");
            return moves;
        }
        else{
            strcat(moves, "D4E5");
            return moves;
        }
    }       
    else if (turn == 'W' && (boardSum == 2024 || boardSum == 2032)){
        char *moves = calloc(5, sizeof(char));
        if (!b->blackThenWhite){
            strcat(moves, "D5E4");
            return moves;
        }
        else{
            strcat(moves, "D4E5");
            return moves;
        }
    }
    else{
        int starti;
        char *moves = calloc((numberOfMoves(b, turn)*5)+1, sizeof(char));
        char *moveNotation;
        if ((b->blackThenWhite && turn == 'B') ||
           (!b->blackThenWhite && turn == 'W'))
            starti = 0;
        else
            starti = 1;
        for (int j = 0; j < 2; j++){
            int8_t checkDown = b->board[j] & b->board[j+1] & ~b->board[j+2];
            int8_t checkDown2 = b->board[j+3] & ~b->board[j+4];
            for (int i = starti; i < 6; i+=2){
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(5-i))) == (7<<(5-i))){
                    //right jump
                    moveNotation = makeMoveNotation(i, j, i+2, j);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((i < 4) && ((((b->board[j]) & (3<<(3-i))) ^ (1<<(3-i))) == (3<<(3-i)))){
                        //right double jump
                        moveNotation = makeMoveNotation(i, j, i+4, j);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(7-i))) == (7<<(5-i))){
                    //left jump
                    moveNotation = makeMoveNotation(i+2, j, i, j);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    //if ((i > 1 && i < 6) && ((((b->board[j]) & (3<<(6-i))) ^ (2<<(6-i))) == (3<<(6-i)))){
                        //left double jump
                        //moveNotation = makeMoveNotation(i+2, j, i-2, j);
                        //strcat(moves, moveNotation);
                        //free(moveNotation);
                    //}
                }
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    //down jump
                    moveNotation = makeMoveNotation(i, j, i, j+2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        //down double jump
                        moveNotation = makeMoveNotation(i, j, i, j+4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
            }
            for (int i = starti + 6; i < 8; i+=2){
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    //down jump
                    moveNotation = makeMoveNotation(i, j, i, j+2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        //down double jump
                        moveNotation = makeMoveNotation(i, j, i, j+4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
            }
            starti ^= 1;
        }
        for (int j = 2; j < 6; j++){ 
            int8_t checkDown = b->board[j] & b->board[j+1] & ~b->board[j+2];
            int8_t checkDown2 = 0;
            if(j+4 < 8)
                 checkDown2 = b->board[j+3] & ~b->board[j+4];
            int8_t checkAbove = b->board[j] & b->board[j-1] & ~b->board[j-2];
            int8_t checkAbove2 = 0;
            if((j-3) > 0)
                 checkAbove2 = b->board[j-3] & ~b->board[j-4];
            for (int i = starti; i < 6; i+=2){
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(5-i))) == (7<<(5-i))){
                    //right jump
                    moveNotation = makeMoveNotation(i, j, i+2, j);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((i < 4) && ((((b->board[j]) & (3<<(3-i))) ^ (1<<(3-i))) == (3<<(3-i)))){
                        //right double jump
                        moveNotation = makeMoveNotation(i, j, i+4, j);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(7-i))) == (7<<(5-i))){
                    //left jump
                    moveNotation = makeMoveNotation(i+2, j, i, j);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    //if ((i > 1 && i < 6) && ((((b->board[j]) & (3<<(6-i))) ^ (2<<(6-i))) == (3<<(6-i)))){
                        //left double jump
                        //moveNotation = makeMoveNotation(i+2, j, i-2, j);
                        //strcat(moves, moveNotation);
                        //free(moveNotation);
                    //}
                }
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    //down jump
                    moveNotation = makeMoveNotation(i, j, i, j+2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        //down double jump
                        moveNotation = makeMoveNotation(i, j, i, j+4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    //up jump
                    moveNotation = makeMoveNotation(i, j, i, j-2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        //up double jump
                        moveNotation = makeMoveNotation(i, j, i, j-4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
            }
            for (int i = starti + 6; i < 8; i+=2){
                if ((checkDown & (1<<(7-i))) == (1<<(7-i))){
                    //down jump
                    moveNotation = makeMoveNotation(i, j, i, j+2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkDown2 & (1<<(7-i))) == (1<<(7-i))){
                        //down double jump
                        moveNotation = makeMoveNotation(i, j, i, j+4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    //up jump
                    moveNotation = makeMoveNotation(i, j, i, j-2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        //up double jump
                        moveNotation = makeMoveNotation(i, j, i, j-4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
            }
            starti ^= 1;
        }
        for (int j = 6; j < 8 ; j++){
            int8_t checkAbove = b->board[j] & b->board[j-1] & ~b->board[j-2];
            int8_t checkAbove2 = b->board[j-3] & ~b->board[j-4];
            for (int i = starti; i < 6; i+=2){
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(5-i))) == (7<<(5-i))){
                    //right jump
                    moveNotation = makeMoveNotation(i, j, i+2, j);                  
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((i < 4) && ((((b->board[j]) & (3<<(3-i))) ^ (1<<(3-i))) == (3<<(3-i)))){
                        //right double jump
                        moveNotation = makeMoveNotation(i, j, i+4, j);
                        strcat(moves, moveNotation);
                    }
                }
                if ((((b->board[j]) & (7<<(5-i))) ^ (1<<(7-i))) == (7<<(5-i))){
                    //left jump
                    moveNotation = makeMoveNotation(i+2, j, i, j);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    //if ((i > 1 && i < 6) && ((((b->board[j]) & (3<<(6-i))) ^ (2<<(6-i))) == (3<<(6-i)))){
                        //left double jump
                        //moveNotation = makeMoveNotation(i+2, j, i-2, j);
                        //strcat(moves, moveNotation);
                        //free(moveNotation);
                    //}
                }
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    //up jump
                    moveNotation = makeMoveNotation(i, j, i, j-2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        //up double jump
                        moveNotation = makeMoveNotation(i, j, i, j-4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
            }
            for (int i = starti + 6; i < 8; i+=2){
                if ((checkAbove & (1<<(7-i))) == (1<<(7-i))){
                    //up jump
                    moveNotation = makeMoveNotation(i, j, i, j-2);
                    strcat(moves, moveNotation);
                    free(moveNotation);
                    if ((checkAbove2 & (1<<(7-i))) == (1<<(7-i))){
                        //up double jump
                        moveNotation = makeMoveNotation(i, j, i, j-4);
                        strcat(moves, moveNotation);
                        free(moveNotation);
                    }
                }
            }
            starti ^= 1;
        }
        return moves;
    }
    return NULL;
}
