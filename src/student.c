#include "student.h"
#include "utility.h"

void set_id(student stud, int new_id){
    stud.id = new_id;
}

void set_ade_mark(student stud, int new_ade_mark){
    stud.ade_mark = new_ade_mark;
}

void set_rand_ade_mark(student stud){
    stud.ade_mark = random_between(getpid(), 18, 30);
    printf("Mark: %d", stud.ade_mark); // debug
}

void set_nof_elem(student stud, int new_nof_elem) {
    stud.ade_mark = new_nof_elem;
}

void set_ask(student stud, int new_ask) {
    stud.ask = new_ask;
}

void set_nof_invites(student stud, int new_nof_invites) {
    stud.nof_invites = new_nof_invites;
}

void set_max_reject(student stud, int new_max_reject) {
    stud.max_reject = new_max_reject;
}

void read_conf(char * config_path){
    int max = 100;
    char line[max];
    FILE *config_fp = fopen(config_path, "r");

    if (config_fp == NULL){
        print_error(errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "sim_time = %d", &sim_time) != 1) {
        print_error(errno);
    } else {
        if (sim_time < 0){
            print_error(errno);
        } else {
            printf ("sim_time setted to: %d\n", sim_time);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_2 = %d", &dev_preference_2) != 1) {
        print_error(errno);
    } else {
        if (sim_time < 0){
            print_error(errno);
        } else {
            printf ("dev_preference_2 setted to: %d\n", dev_preference_2);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_3 = %d", &dev_preference_3) != 1) {
        print_error(errno);
    } else {
        if (sim_time < 0){
            print_error(errno);
        } else {
            printf ("dev_preference_3 setted to: %d\n", dev_preference_3);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_4 = %d", &dev_preference_4) != 1) {
        print_error(errno);
    } else {
        if (sim_time < 0){
            print_error(errno);
        } else {
            printf ("dev_preference_4 setted to: %d\n", dev_preference_4);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "max_reject = %d", &max_reject) != 1) {
        print_error(errno);
    } else {
        if (sim_time < 0){
            print_error(errno);
        } else {
            printf ("max_reject setted to: %d\n", max_reject);
        }
    }
}

void find_mates(){

}