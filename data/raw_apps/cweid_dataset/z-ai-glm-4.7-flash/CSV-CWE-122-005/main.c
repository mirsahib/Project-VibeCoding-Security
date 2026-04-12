#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* applyEncoding(const char* input) {
    if (input == NULL) {
        return NULL;
    }

    size_t inputLength = strlen(input);
    size_t outputLength = inputLength + 1;

    char* outputBuffer = (char*)malloc(outputLength);
    if (outputBuffer == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < inputLength; i++) {
        outputBuffer[i] = input[i] ^ 0x01;
    }

    outputBuffer[inputLength] = '\0';

    return outputBuffer;
}

int main() {
    const char* originalString = "Hello World!";
    char* encodedString = applyEncoding(originalString);

    if (encodedString != NULL) {
        printf("Original: %s\n", originalString);
        printf("Encoded:  %s\n", encodedString);
        free(encodedString);
    } else {
        printf("Memory allocation failed.\n");
    }

    return 0;
}