#include "mydefs.h"


bool file2str(const char* fname, char* str) {
    // Step 1: if the pointer passed in is empty
    if (fname == NULL || str == NULL) {
        return false;
    }

    // Step 2: whether the fname is exist
    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        return false;
    }

    // Step 3: create a array to store the str, extra space for "\n" and "\0" 
    char line[BRDSZ + OFFSET_2];
    int index = 0;

    // Step 4: to read the hawk
    if (fgets(line, sizeof(line), fp)) {
        str[index++] = line[0];
        str[index++] = '-';
    }

    // Step 5: to read every line
    while (fgets(line, sizeof(line), fp)) {
        int len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
            len--;
        }

        for (int i = 0; i < len; i++) {
            str[index++] = line[i];
        }
        str[index++] = '-';
    }

    // to remove the last "-" and add "\0" to end the str
    if (index > 0) {
        str[index-1] = '\0';
    }

    fclose(fp);
    return true;
}

state* str2state(const char* str) {
    // Step 1: if the pointer passed in is empty
    if (str ==  NULL) {
        return NULL;
    }

    // Step 2:check that the first character (hawk) is uppercase
    if (isupper(str[0]) == 0) {
        return NULL;
    }

    // Step 3: malloc a space for pointer s
    state* s = (state*)malloc(sizeof(state));
    // check whether the pointer space is allocated successfully
    if (s == NULL) {
        return NULL;
    }

    // Step 4：initialize the pointer
    s->f = 0;
    s->r = 0;

    // Step 5：gets the first board structure pointer in the state structure
    board* original = &s->boards[0] ;
    //indicates no parent node
    original->parent = -1;
    //indicates the column that has not been moved
    original->move_col = -1;

    //to read the str
    const char* ch = str;
    original->hawk = *ch++;
    //to skip the "-"
    ch++;

    // Step 6：to measure the width
    const char* temp = ch;
    original->width = 0;
    while (*temp && *temp != '-') {
        if(isupper(*temp) == 0 && *temp != '-'){
            free(s);
            return NULL;
        }
        original->width++;
        temp++;
    }

    // Step 7：fill the letter in board
    int index = 0;
    original->height = 0;

    while (*ch) {
        if (*ch == '-') {
            original->height++;
            ch++;
        } 
        else 
        {
            if(isupper(*ch) == 0){
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
    if (b == NULL){
        return false;
    }

    //to check whether the characters in each column are the same
    for (int col = 0; col < b->width; col++) {
        char first_char = b->tiles[col];
        for (int row = 1; row < b->height; row++) {
            if (b->tiles[row * b->width + col] != first_char) {
                return false;
            }
        }
    }
    return true;
}

bool same_2Board(const board* b1, const board* b2) {

    if ( b1 == NULL || b2 == NULL){
        return false;
    }
    
    if (b1->hawk != b2->hawk){
        return false;
    }    
        
    if (b1->width != b2->width || b1->height != b2->height){
        return false;
    }

    return strcmp(b1->tiles, b2->tiles) == 0;

}

void drop_hawk(board* newBoard, const board* cur, int col) {
    memcpy(newBoard, cur, sizeof(board));
    newBoard->move_col = col;

    //to save the bottom element
    char bottom = cur->tiles[(cur->height-1) * cur->width + col];

    //move down column
    for (int row = cur->height-1; row > 0; row--) {
        newBoard->tiles[row * cur->width + col] = cur->tiles[(row-1) * cur->width + col];
    }

    //put the hawk to the top
    newBoard->tiles[col] = cur->hawk;
    newBoard->hawk = bottom;
}

void print_output(const state* s, int endIndex) {
    int outputArray[MAXBRDS];
    int moveNum = 0;
    int currentIndex = endIndex;
    int outputArray_len = 0;


    while (currentIndex > 0) {
        moveNum++;
        currentIndex = s->boards[currentIndex].parent;
    }

    currentIndex = endIndex;
    while (currentIndex >= 0) {
        outputArray[outputArray_len++] = currentIndex;
        currentIndex = s->boards[currentIndex].parent;
    }

    for (int i = outputArray_len - 1; i >= 0; i--) {
        for (int row = 0; row < s->boards[outputArray[i]].height; row++) {
            for (int col = 0; col < s->boards[outputArray[i]].width; col++) {
                printf("%c ", s->boards[outputArray[i]].tiles[row * s->boards[outputArray[i]].width + col]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

int solve(state* s, bool verbose) {
    // Step 1: if the pointer passed in is empty
    if ( s == NULL){
        return -1;
    }

    // whether the letters in boards are all matched 
    if (all_Matched(&s->boards[0])){
        return 0;
    }

    //initialize the pointers
    s->f = 0;
    s->r = 0;

    while (s->f <= s->r && s->r < MAXBRDS - 1) {
        board* cur = &s->boards[s->f];

        for (int col = 0; col < cur->width; col++) {
            board* newBoard = &s->boards[s->r + 1];
            drop_hawk(newBoard, cur, col);
            newBoard->parent = s->f;

            bool repeatBoard = false;
            for (int i = 0; i <= s->r && repeatBoard == false; i++) {
                if (same_2Board(newBoard, &s->boards[i])) {
                    repeatBoard = true;
                }
            }

            if (repeatBoard == false) {
                s->r++;

                if (all_Matched(newBoard)) {
                    int moveNum = 0;
                    int currentIndex = s->r;
                    while (currentIndex > 0) {
                        moveNum++;
                        currentIndex = s->boards[currentIndex].parent;
                    }

                    if (verbose == true) {
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

    //TEST 1
    assert(file2str("test1.txt", str));
    assert(strcmp("B-A", str) == 0);
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 0);
    free(s);

    //TEST 2
    assert(file2str("test2.txt", str));
    assert(strcmp("A-BCA-BCA-BCA-ACB", str) == 0);
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 2);
    free(s);
   
    //TEST 3
    assert(file2str("test3.txt", str));
    assert(strcmp("A-MN-IJ", str) == 0);
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == -1);
    free(s);

    //TEST 4
    assert(file2str("test4.txt", str));
    assert(strcmp("H-HMD-HMD-HMD-DMD", str) == 0);
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 1);
    free(s);

    //TEST 5
    assert(file2str("test5.txt", str));
    assert(strcmp("A-C-BD", str) == 0);
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == -1);
    free(s);

    //TEST 6    
    assert(file2str("test6.txt", str));
    assert(strcmp("A-BAC-BAC-BAC", str) == 0);
    s = str2state(str);
    assert(s);
    assert(solve(s, false) == 0);
    free(s);

}
 