#include <stdio.h>
#include <stdlib.h>

struct school {
    char name[100];
    int students;
    int *allocations;
};

typedef struct school School;

void allocate_memory(School *school, int num_schools) {
    school->allocations = (int *) malloc(num_schools * sizeof(int));
}

void calculate_allocations(School *school, int num_schools) {
    int total_students = 0;
    for (int i = 0; i < num_schools; i++) {
        total_students += school[i].students;
    }
    
    for (int i = 0; i < num_schools; i++) {
        school[i].allocations = (int) ((school[i].students / (float) total_students) * 100);
    }
}

void print_allocations(School *school, int num_schools) {
    for (int i = 0; i < num_schools; i++) {
        printf("School %s has %d students and an allocation of %d%%\n", school[i].name, school[i].students, school[i].allocations);
    }
}

int main() {
    int num_schools;
    printf("Enter the number of schools: ");
    scanf("%d", &num_schools);
    
    School *schools = (School *) malloc(num_schools * sizeof(School));
    
    for (int i = 0; i < num_schools; i++) {
        printf("Enter the name of school %d: ", i + 1);
        scanf("%s", schools[i].name);
        printf("Enter the number of students in school %d: ", i + 1);
        scanf("%d", &schools[i].students);
    }
    
    allocate_memory(schools, num_schools);
    calculate_allocations(schools, num_schools);
    print_allocations(schools, num_schools);
    
    free(schools);
    return 0;
}