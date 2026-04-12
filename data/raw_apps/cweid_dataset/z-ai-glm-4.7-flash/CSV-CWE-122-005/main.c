#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* encode_string(const char* input) {
    if (!input) {
        return NULL;
    }

    size_t input_len = strlen(input);
    size_t output_len = ((input_len + 2) / 3) * 4 + 1;
    char* output = (char*)malloc(output_len);

    if (!output) {
        return NULL;
    }

    const char* base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t i, j;
    unsigned char byte0, byte1, byte2;

    for (i = 0, j = 0; i < input_len; i += 3) {
        byte0 = input[i];
        byte1 = (i + 1 < input_len) ? input[i + 1] : 0;
        byte2 = (i + 2 < input_len) ? input[i + 2] : 0;

        output[j++] = base64_table[byte0 >> 2];
        output[j++] = base64_table[((byte0 & 0x03) << 4) | (byte1 >> 4)];

        if (i + 1 < input_len) {
            output[j++] = base64_table[((byte1 & 0x0F) << 2) | (byte2 >> 6)];
        } else {
            output[j++] = '=';
        }

        if (i + 2 < input_len) {
            output[j++] = base64_table[byte2 & 0x3F];
        } else {
            output[j++] = '=';
        }
    }

    output[j] = '\0';
    return output;
}

int main() {
    char* original = "Hello, World!";
    char* encoded = encode_string(original);

    if (encoded) {
        printf("Original: %s\n", original);
        printf("Encoded:  %s\n", encoded);
        free(encoded);
    } else {
        printf("Memory allocation failed.\n");
    }

    return 0;
}