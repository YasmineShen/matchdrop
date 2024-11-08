#include "mydefs.h"


void fillLines(FILE* fp, char* str, int* index){
    // plus 2 extra space for "\n" and "\0" 
    char line[BRDSZ + EXTRASPACE];
    while (fgets(line, sizeof(line), fp)){
        int len = strlen(line);
        
        if (len > 0 && line[len-1] == '\n'){
            line[len-1] = '\0';
            len--;
        }

        // copy the line's characters to 'str'
        for (int i = 0; i < len; i++){
            str[*index] = line[i];
            (*index)++;
        }
        // add '-' after each line
        str[*index] = '-';
        (*index)++;
    }
}


void finalizeStr(char* str, int* index){
    //remove last '-' and append '\0'
    if (*index > 0){
        str[*index - 1] = '\0';
    }
}


bool file2str(const char* fname, char* str){
    // if the pointer passed in is empty 
    if(fname == NULL || str == NULL){
        return false;
    }

    // whether the fname is exist
    FILE* fp = fopen(fname, "r");
    if(fp == NULL){
        return false;
    }

    // create an array to store the str
    // plus 2 extra space for "\n" and "\0" 
    char line[BRDSZ + EXTRASPACE];
    int index = 0;

    // to read the hawk and store it in the first position
    if(fgets(line, sizeof(line), fp)){
        str[index++] = line[0];
        str[index++] = '-';
    }

    fillLines(fp, str, &index);

    finalizeStr(str, &index);

    fclose(fp);
    return true;
}


bool validChar(char c){
    if (isupper(c) || c == '-'){
        return true;
    }
    return false;
}


state* initializeState(const char* str){
    if (str == NULL || isupper(str[0]) == 0){
        return NULL;
    }

    state* s = (state*)malloc(sizeof(state));
    if (s == NULL) {
        return NULL;
    }

    s->r = 0;
    s->f = 0;

    s->boards[0].parent = -1;
    s->boards[0].moveCol = -1;
    s->boards[0].hawk = str[0];
    
    return s;
}


int findWidth(const char* str){
    int width = 0;
    const char* ch = str;

    // count characters until a '-' or end of str
    while (*ch && *ch != '-'){
        if (isupper(*ch) == 0){
            return -1; 
        }
        width++;
        ch++;
    }
    
    return width;
}


bool fillBoard(board* original, const char* str){
    int index = 0;
    original->height = 0;
    const char* ch = str;

    while (*ch){
        if (*ch == '-') {
            original->height++;
            ch++;
        } else {
            if (validChar(*ch) == 0){
                return false;
            }
            original->tiles[index++] = *ch++;
        }
    }
    original->height++;
    return true;
}


state* str2state(const char* str){
    // initialize state and validate the hawk character
    state* s = initializeState(str);
    if (s == NULL){
        return NULL;
    }

    // calculate board width
    board* original = &s->boards[0];
    original->width = findWidth(str + EXTRASPACE);
    if (original->width == -1){
        free(s);
        return NULL;
    }


    if (fillBoard(original, str + EXTRASPACE) == 0){
        free(s);
        return NULL;
    }

    return s;
}


bool allMatched(const board* b){
    // if the pointer passed in is empty
    if(b == NULL){
        return false;
    }

    // to check whether the characters in each column are the same
    for(int j = 0; j < b->width; j++){
        char flagChar = b->tiles[j];
        for(int i = 1; i < b->height; i++){
            if(b->tiles[i * b->width + j] != flagChar){
                return false;
            }
        }
    }
    return true;
}


bool sameBoard(const board* b1, const board* b2){
    if(b1 == NULL || b2 == NULL){
        return false;
    }

    // to check whether the hawk is same
    if(b1->hawk != b2->hawk){
        return false;
    }    

    // to check whether the width and height is same
    if(b1->width != b2->width || b1->height != b2->height){
        return false;
    }

    return strcmp(b1->tiles, b2->tiles) == 0;

}


bool repeatedBoard(const state* s, const board* newBoard){
    for (int i = 0; i <= s->r; i++){
        if (sameBoard(newBoard, &s->boards[i])){
            return true;
        }
    }
    return false;
}


void fallHawk(board* newBoard, const board* cur, int j){
    memcpy(newBoard, cur, sizeof(board));
    newBoard->moveCol = j;

    //to save the bottom element
    char bottom = cur->tiles[(cur->height-1) * cur->width + j];

    //move the column down
    for(int i = cur->height-1; i > 0; i--){
        newBoard->tiles[i * cur->width + j] = cur->tiles[(i-1) * cur->width + j];
    }

    //put the hawk to the top
    newBoard->tiles[j] = cur->hawk;
    newBoard->hawk = bottom;
}


void printOutput(const state* s, int endIndex){
    int output[MAXBRDS];
    int curIndex = endIndex;
    int length = 0;

    // find path length by tracing back from end to start
    while(curIndex > 0){
        curIndex = s->boards[curIndex].parent;
    }

    // record solution path from end to start
    curIndex = endIndex;
    while(curIndex >= 0){
        output[length++] = curIndex;
        curIndex = s->boards[curIndex].parent;
    }

    // print it by order
    for(int k = length - 1; k >= 0; k--){
        for(int i = 0; i < s->boards[output[k]].height; i++){
            for(int j = 0; j < s->boards[output[k]].width; j++){
                printf("%c", s->boards[output[k]].tiles[i * s->boards[output[k]].width + j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}


void initializeBoard(state* s){
    s->f = 0;
    s->r = 0;
}


bool finalPath(const board* b){
    return allMatched(b);
}


void widenBoard(state* s, int j, int parentIndex){
    board* newBoard = &s->boards[s->r + 1];
    board* cur = &s->boards[parentIndex];

    fallHawk(newBoard, cur, j);
    newBoard->parent = parentIndex;
}


int countMoveNum(const state* s, int endIndex){
    int moveNum = 0;
    int curIndex = endIndex;

    while (curIndex > 0){
        moveNum++;
        curIndex = s->boards[curIndex].parent;
    }
    return moveNum;
}


int solve(state* s, bool verbose){
    if (s == NULL) {
        return -1;
    }

    if (finalPath(&s->boards[0])){
        return 0;
    }

    initializeBoard(s);

    // start BFS-style search
    while (s->f <= s->r && s->r < MAXBRDS - 1){
        board* cur = &s->boards[s->f];

        for (int j = 0; j < cur->width; j++){
            widenBoard(s, j, s->f);
            board* newBoard = &s->boards[s->r + 1];

            if (repeatedBoard(s, newBoard) == 0){
                s->r++;

                if (finalPath(newBoard)){
                    int moveNum = countMoveNum(s, s->r);

                    if (verbose){
                        printOutput(s, s->r);
                    }
                    return moveNum;
                }
            }
        }
        s->f++;
    }

    // no method is found
    return -1;
}


void test(void){

    char str[MAXBRDS];
    state * s;


    //TEST 1: test str that already match the rules of the game
    strcpy(str, "B-A");
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 0);
    free(s);

   
    //TEST 2: test str that already match the rules of the game
    strcpy(str, "A-BAC-BAC-BAC");
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 0);
    free(s);

    
    //TEST 3: test the valid str
    strcpy(str, "H-HMD-HMD-HMD-DMD");
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 1);
    free(s);


    //TEST 4: test the invalid str
    strcpy(str ,"A-MN-IJ");
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == -1);
    free(s);

    
    //TEST 5: test the invalid str
    strcpy(str, "A-C-BD");
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == -1);
    free(s);

    
    //TEST 6：test str filled with lowercase letters
    strcpy(str, "a-abc-abc-abc");
    s = str2state(str);
    assert(s == NULL);


    //TEST 7：test str filled with numbers
    strcpy(str, "a-123-456-abc");
    s = str2state(str);
    assert(s == NULL);
    free(s);


    //TEST 8: test str with invalid hawk
    strcpy(str, "8-CBA-CBA-CBA");
    s = str2state(str);
    assert(s == NULL);
    free(s);


}




 
