#include "common.h"

typedef struct {
    int id;
    int AdE_mark;
    int nof_elem;
    int ask;
    int nof_invites;
    int max_reject;
    int final_mark;
} student;

int get_random_mark(){
    srand((unsigned) getpid());
    return rand() % (30 + 1 - 18) + 18;
}

void search_mate(){

}