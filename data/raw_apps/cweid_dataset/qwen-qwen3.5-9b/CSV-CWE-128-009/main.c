#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[50];
    int capacity;
    int currentStudents;
} SchoolData;

int main() {
    int n;
    printf("Enter number of schools: ");
    scanf("%d", &n);

    SchoolData *schools = (SchoolData *)malloc(n * sizeof(SchoolData));

    if (schools == NULL) {
        printf("Memory allocation failed!");
        return -1;
    }

    int totalStudents = 0;

    for (int i = 0; i < n; i++) {
        schools[i].id = i + 1;
        printf("Enter name for school %d: ", i + 1);
        scanf("%49s", schools[i].name);
        printf("Enter capacity and current students (space separated): ");
        scanf("%d %d", &schools[i].capacity, &schools[i].currentStudents);
        totalStudents += schools[i].currentStudents;
    }

    printf("Total current students allocated: %d\n", totalStudents);
    printf("Total memory allocated for schools: %ld bytes\n", (long)n * sizeof(SchoolData));

    free(schools);
    return 0;
}