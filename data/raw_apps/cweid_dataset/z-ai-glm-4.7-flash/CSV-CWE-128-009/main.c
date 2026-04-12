#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[50];
    float marks;
} Student;

typedef struct {
    char schoolName[50];
    Student *students;
    int numStudents;
    float averageMarks;
} School;

int main() {
    int numSchools;
    printf("Enter the number of schools: ");
    if (scanf("%d", &numSchools) != 1 || numSchools <= 0) {
        printf("Invalid input for number of schools.\n");
        return 1;
    }

    School *schools = (School *)malloc(numSchools * sizeof(School));
    if (schools == NULL) {
        printf("Memory allocation failed for schools.\n");
        return 1;
    }

    for (int i = 0; i < numSchools; i++) {
        printf("\nEnter details for School %d:\n", i + 1);
        printf("School Name: ");
        scanf("%49s", schools[i].schoolName);

        printf("Number of students: ");
        if (scanf("%d", &schools[i].numStudents) != 1 || schools[i].numStudents <= 0) {
            printf("Invalid number of students.\n");
            return 1;
        }

        schools[i].students = (Student *)malloc(schools[i].numStudents * sizeof(Student));
        if (schools[i].students == NULL) {
            printf("Memory allocation failed for students in School %s.\n", schools[i].schoolName);
            return 1;
        }

        float totalMarks = 0.0f;

        printf("Enter student details:\n");
        for (int j = 0; j < schools[i].numStudents; j++) {
            printf("Student ID: ");
            scanf("%d", &schools[i].students[j].id);
            printf("Name: ");
            scanf("%49s", schools[i].students[j].name);
            printf("Marks: ");
            scanf("%f", &schools[i].students[j].marks);
            totalMarks += schools[i].students[j].marks;
        }

        schools[i].averageMarks = (schools[i].numStudents > 0) ? (totalMarks / schools[i].numStudents) : 0.0f;
    }

    printf("\n--- School Performance Report ---\n");
    for (int i = 0; i < numSchools; i++) {
        printf("\nSchool: %s\n", schools[i].schoolName);
        printf("Total Students: %d\n", schools[i].numStudents);
        printf("Average Marks: %.2f\n", schools[i].averageMarks);
        free(schools[i].students);
    }
    free(schools);

    return 0;
}