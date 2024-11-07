#include "mydefs.h"


bool file2str(const char* fname, char* str) {
    // Step 1: if the pointer passed in is empty 
    if(fname == NULL || str == NULL){
        return false;
    }

    // Step 2: whether the fname is exist
    FILE* fp = fopen(fname, "r");
    if(fp == NULL){
        return false;
    }

    // Step 3: create an array to store the str
    // plus 2 extra space for "\n" and "\0" 
    char line[BRDSZ + EXTRASPACE];
    int index = 0;

    // Step 4: to read the hawk and store it in the first position
    if(fgets(line, sizeof(line), fp)){
        str[index++] = line[0];
        str[index++] = '-';
    }

    // Step 5: to read every line and store the letter in str
    while(fgets(line, sizeof(line), fp)){
        int len = strlen(line);
	
        if(len > 0 && line[len-1] == '\n'){
            line[len-1] = '\0';
            len--;
        }

        for(int i = 0; i < len; i++){
            str[index++] = line[i];
        }
        str[index++] = '-';
    }

    // Step 6: to remove the last "-" and add "\0" to end the str
    if(index > 0){
        str[index-1] = '\0';
    }

    fclose(fp);
    return true;
}


bool validChar(char c){
    if (isupper(c) || c == '-'){
        return true;
    }
    return false;
}


state* str2state(const char* str) {
    // Step 1: if the pointer passed in is empty
    if(str ==  NULL){
        return NULL;
    }

    // Step 2:check that the first character (hawk) is uppercase
    if(isupper(str[0]) == 0){
        return NULL;
    }

    // Step 3: malloc a space for pointer s
    state* s = (state*)malloc(sizeof(state));
    // check whether the pointer space is allocated successfully
    if(s == NULL){
        return NULL;
    }

    // Step 4：initialize the pointer
    s->r = 0;
    s->f = 0;

    // Step 5：get the first board structure pointer in the state structure
    board* original = &s->boards[0];
    // indicates no parent node
    original->parent = -1;
    // indicates the column that has not been moved
    original->moveCol = -1;

    // to read the str
    const char* ch = str;
    original->hawk = *ch++;
    // to skip the "-"
    ch++;

    // Step 6：to measure the width
    const char* temp = ch;
    original->width = 0;
    while(*temp && *temp != '-'){
        if(isupper(*temp) == 0){
            free(s);
            return NULL;
        }
        original->width++;
        temp++;
    }

    // Step 7：fill the letter in board
    int index = 0;
    original->height = 0;

    while(*ch){
        if(*ch == '-'){
            original->height++;
            ch++;
        } 
        else 
        {
            if(validChar(*ch) == 0){
                free(s);
                return NULL;
            }
            original->tiles[index++] = *ch++;
        }
    }
    original->height++;

    return s;

}



bool all_Matched(const board* b) {
    // Step 1: if the pointer passed in is empty
    if(b == NULL){
        return false;
    }

    // Step 2: to check whether the characters in each column are the same
    for(int col = 0; col < b->width; col++){
        char flagChar = b->tiles[col];
        for(int row = 1; row < b->height; row++){
            if(b->tiles[row * b->width + col] != flagChar){
                return false;
            }
        }
    }
    return true;
}



bool same_2Board(const board* b1, const board* b2){

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



void fall_hawk(board* newBoard, const board* cur, int col){
    memcpy(newBoard, cur, sizeof(board));
    newBoard->moveCol = col;

    //to save the bottom element
    char bottom = cur->tiles[(cur->height-1) * cur->width + col];

    //move the column down
    for(int row = cur->height-1; row > 0; row--){
        newBoard->tiles[row * cur->width + col] = cur->tiles[(row-1) * cur->width + col];
    }

    //put the hawk to the top
    newBoard->tiles[col] = cur->hawk;
    newBoard->hawk = bottom;
}



void print_output(const state* s, int endIndex){
    int output[MAXBRDS];
    int curIndex = endIndex;
    int length = 0;


    while(curIndex > 0){
        curIndex = s->boards[curIndex].parent;
    }

    curIndex = endIndex;
    while(curIndex >= 0){
        output[length++] = curIndex;
        curIndex = s->boards[curIndex].parent;
    }

    for(int i = length - 1; i >= 0; i--){
        for(int row = 0; row < s->boards[output[i]].height; row++){
            for(int col = 0; col < s->boards[output[i]].width; col++){
                printf("%c", s->boards[output[i]].tiles[row * s->boards[output[i]].width + col]);
            }
            printf("\n");
        }
        printf("\n");
    }
}



int solve(state* s, bool verbose){
    // Step 1: if the pointer passed in is empty
    if(s == NULL){
        return -1;
    }

    // Step 2: to check whether the letters in boards are all matched 
    if(all_Matched(&s->boards[0])){
        return 0;
    }

    //initialize the pointers
    s->f = 0;
    s->r = 0;

    while(s->f <= s->r && s->r < MAXBRDS - 1){
        board* cur = &s->boards[s->f];

        for(int col = 0; col < cur->width; col++){
            board* newBoard = &s->boards[s->r + 1];
            fall_hawk(newBoard, cur, col);
            newBoard->parent = s->f;

            bool repeatBoard = false;
            for(int i = 0; i <= s->r && repeatBoard == false; i++){
                if(same_2Board(newBoard, &s->boards[i])){
                    repeatBoard = true;
                }
            }

            if(repeatBoard == false){
                s->r++;

                if(all_Matched(newBoard)){
                    int moveNum = 0;
                    int curIndex = s->r;
                    while(curIndex > 0){
                        moveNum++;
                        curIndex = s->boards[curIndex].parent;
                    }

                    if(verbose == true){
                        print_output(s, s->r);
                    }

                    return moveNum;

                }
            }
        }

        s->f++;

    }

    return -1;
}



void test(void) {

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




 
