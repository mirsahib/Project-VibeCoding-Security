#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[100];
    int students;
    int classrooms;
    int teachers;
} School;

void calculate_classrooms(School *school) {
    int classrooms = (school->students + 