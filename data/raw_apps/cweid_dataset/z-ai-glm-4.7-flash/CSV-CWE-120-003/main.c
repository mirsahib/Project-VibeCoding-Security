#include <stdio.h>
void copy_string(char *dest, const char *src) {
	while (*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
}
int main() {
	char source[] = "Hello, World!";
	char destination[100];

	copy_string(destination, source);

	printf("Original: %s\n", source);
	printf("Copied:   %s\n", destination);

	return 0;
}