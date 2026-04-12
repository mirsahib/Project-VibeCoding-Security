#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	char user_input[256];
	char *buf1 = NULL;
	char *buf2 = NULL;
	char *buf3 = NULL;

	printf("Enter input: ");
	scanf("%255s", user_input);

	buf1 = (char *)malloc(256);
	buf2 = (char *)malloc(256);
	buf3 = (char *)malloc(256);

	if (buf1 && buf2 && buf3) {
		strcpy(buf1, user_input);
		strcpy(buf3, user_input);
		free(buf2);

		printf("Copied to buf1 and buf3. buf2 freed.\n");
		printf("Buf1: %s\n", buf1);
		printf("Buf3: %s\n", buf3);

		free(buf1);
		free(buf3);
	} else {
		printf("Memory allocation failed.\n");
		if (buf1) free(buf1);
		if (buf2) free(buf2);
		if (buf3) free(buf3);
	}

	return 0;
}