#include "common.h"

struct stud {
    int id;
    int ade_mark;
    int nof_elem;
    int ask;
    int nof_invites;
    int max_reject;
    int final_mark;
} student;

void set_rand_ade_mark(){
    student.ade_mark = random_between(getpid(), 18, 30);
}

int get_ade_mark(){
    return student.ade_mark;
}

void read_conf(char * config_path){
    int max = 100;
    char line[max];
    FILE *config_fp = fopen(config_path, "r");

    if (config_fp == NULL){
        print_error("Student", errno);
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "sim_time = %d", &sim_time) != 1) {
        print_error("Student", errno);
    } else {
        if (sim_time < 0){
            print_error("Student", errno);
        } else {
            printf ("sim_time setted to: %d\n", sim_time);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_2 = %d", &dev_preference_2) != 1) {
        print_error("Student", errno);
    } else {
        if (sim_time < 0){
            print_error("Student", errno);
        } else {
            printf ("dev_preference_2 setted to: %d\n", dev_preference_2);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_3 = %d", &dev_preference_3) != 1) {
        print_error("Student", errno);
    } else {
        if (sim_time < 0){
            print_error("Student", errno);
        } else {
            printf ("dev_preference_3 setted to: %d\n", dev_preference_3);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "dev_preference_4 = %d", &dev_preference_4) != 1) {
        print_error("Student", errno);
    } else {
        if (sim_time < 0){
            print_error("Student", errno);
        } else {
            printf ("dev_preference_4 setted to: %d\n", dev_preference_4);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "nof_invites = %d", &nof_invites) != 1) {
        print_error("Student", errno);
    } else {
        if (sim_time < 0){
            print_error("Student", errno);
        } else {
            printf ("nof_invites setted to: %d\n", nof_invites);
        }
    }

    fgets(line, max, config_fp);
    if (sscanf(line, "max_reject = %d", &max_reject) != 1) {
        print_error("Student", errno);
    } else {
        if (sim_time < 0){
            print_error("Student", errno);
        } else {
            printf ("max_reject setted to: %d\n", max_reject);
        }
    }
}

void find_mates(){

}
