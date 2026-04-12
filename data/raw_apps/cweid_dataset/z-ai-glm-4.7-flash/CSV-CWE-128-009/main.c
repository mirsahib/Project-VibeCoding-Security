#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    float marks;
} Student;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int capacity;
    int current_count;
    Student* students;
} School;

void initializeSchool(School* school, const char* schoolName, int initialCapacity) {
    strncpy(school->name, schoolName, MAX_NAME_LENGTH);
    school->capacity = initialCapacity;
    school->current_count = 0;
    school->students = (Student*)malloc(initialCapacity * sizeof(Student));
}

void addStudent(School* school, int id, const char* name, float marks) {
    if (school->current_count >= school->capacity) {
        int newCapacity = school->capacity * 2;
        Student* tempStudents = (Student*)realloc(school->students, newCapacity * sizeof(Student));
        
        if (tempStudents == NULL) {
            printf("Memory allocation failed. Cannot add more students.\n");
            return;
        }
        
        school->students = tempStudents;
        school->capacity = newCapacity;
        printf("Memory reallocated. New capacity: %d\n", school->capacity);
    }

    school->students[school->current_count].id = id;
    strncpy(school->students[school->current_count].name, name, MAX_NAME_LENGTH);
    school->students[school->current_count].marks = marks;
    school->current_count++;
}

float calculateClassAverage(School* school) {
    if (school->current_count == 0) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (int i = 0; i < school->current_count; i++) {
        sum += school->students[i].marks;
    }
    return sum / school->current_count;
}

void freeSchool(School* school) {
    free(school->students);
}

int main() {
    School mySchool;
    initializeSchool(&mySchool, "Springfield High", 2); // Start with 2 capacity to force reallocation

    int id;
    char name[MAX_NAME_LENGTH];
    float marks;

    printf("Student Management System - Dynamic Memory Allocation Demo\n");
    printf("School Name: %s\n", mySchool.name);
    printf("Initial Capacity: %d\n\n", mySchool.capacity);

    for (int i = 1; i <= 4; i++) {
        printf("Enter details for Student %d:\n", i);
        printf("ID: "); scanf("%d", &id);
        printf("Name: "); scanf("%s", name);
        printf("Marks: "); scanf("%f", &marks);
        addStudent(&mySchool, id, name, marks);
        printf("Current Capacity: %d | Current Students: %d\n\n", mySchool.capacity, mySchool.current_count);
    }

    printf("Calculating Class Average...\n");
    float average = calculateClassAverage(&mySchool);
    printf("\nSchool Statistics for %s:\n", mySchool.name);
    printf("Total Enrolled Students: %d\n", mySchool.current_count);
    printf("Class Average Marks: %.2f\n", average);

    freeSchool(&mySchool);
    return 0;
}