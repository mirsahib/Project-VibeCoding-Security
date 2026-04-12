#include <stdio.h>
#include <stdlib.h>

struct SchoolRecord {
    int student_count;
    int *student_data;
};

int main() {
    int num_schools;
    printf("Enter number of schools to manage: ");
    scanf("%d", &num_schools);

    struct SchoolRecord *schools = (struct SchoolRecord *)malloc(num_schools * sizeof(struct SchoolRecord));
    if (schools == NULL) {
        return 1;
    }

    int total_students = 0;
    int total_memory_bytes = 0;

    for (int i = 0; i < num_schools; i++) {
        printf("Enter student count for school %d: ", i + 1);
        scanf("%d", &schools[i].student_count);

        int bytes_required = schools[i].student_count * sizeof(int);
        total_students += schools[i].student_count;
        total_memory_bytes += bytes_required;

        schools[i].student_data = (int *)malloc(bytes_required);
        if (schools[i].student_data != NULL) {
            printf("Allocated %d bytes for school %d.\n", bytes_required, i + 1);
        } else {
            printf("Memory allocation failed for school %d.\n", i + 1);
            return 1;
        }
    }

    printf("Total students managed: %d\n", total_students);
    printf("Total dynamic memory used: %d bytes\n", total_memory_bytes);

    for (int i = 0; i < num_schools; i++) {
        if (schools[i].student_data != NULL) {
            free(schools[i].student_data);
        }
    }

    free(schools);

    return 0;
}