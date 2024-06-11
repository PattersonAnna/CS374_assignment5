#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

#define MAX_LINE 50
#define MAX_CHAR 1000
#define FULL_BUFFER_SIZE 80

// Boolean value to keep track of stop
bool stopValue = false;

// Struct to hold buffers
struct readCharsBuffer {
    char *inputBuffer;
    char *lineSeperatorBuffer;
};

void print80Chars(struct readCharsBuffer *buffers){
    printf("The 80 char buffer after everything: %s\n", buffers->lineSeperatorBuffer);
    size_t size = strlen(buffers->lineSeperatorBuffer);
    printf("The size is: %zu\n", size);
}

void replacePlusSign(struct readCharsBuffer *buffers){
    for(int i = 0; i < FULL_BUFFER_SIZE; i++){
        if (buffers->lineSeperatorBuffer[i] == '+' && buffers->lineSeperatorBuffer[i+1] == '+') {
            buffers->lineSeperatorBuffer[i] = '^';
            //the space is added to make up for the lost char when ++ is replaced with ^
            buffers->lineSeperatorBuffer[i+1] = ' ';
            i++;
        }
    }
    print80Chars(buffers);
}

void lineSeparator(struct readCharsBuffer *buffers){
    for(int i = 0; i < FULL_BUFFER_SIZE; i++){
        if (buffers->lineSeperatorBuffer[i] == '\n') {
            buffers->lineSeperatorBuffer[i] = ' ';
        }
    }
    replacePlusSign(buffers);
}

void *readChars(void *arg) {
    // Buffer for the input and line separator
    struct readCharsBuffer *buffers = (struct readCharsBuffer *)arg;
    char tempInputBuffer[1081];
    int leftoverInBuffer;

    // Ensure lineSeperatorBuffer is cleared
    memset(buffers->lineSeperatorBuffer, 0, FULL_BUFFER_SIZE);

    printf("Enter line of input: ");
    // Use fgets to read a line of input including spaces
    fgets(tempInputBuffer, 1081, stdin);

    // Concatenate new input to the inputBuffer
    strncat(buffers->inputBuffer, tempInputBuffer, 1080 - strlen(buffers->inputBuffer));

    size_t size = strlen(buffers->inputBuffer);
    printf("The size is: %zu\n", size);

    // Read user input into the buffer
    if (size >= FULL_BUFFER_SIZE) {
        // Copy the first 80 characters to lineSeperatorBuffer
        strncpy(buffers->lineSeperatorBuffer, buffers->inputBuffer, FULL_BUFFER_SIZE);

        // Copy the remaining characters after the first 80 to the front of inputBuffer
        leftoverInBuffer = size - FULL_BUFFER_SIZE;
        memmove(buffers->inputBuffer, buffers->inputBuffer + FULL_BUFFER_SIZE, leftoverInBuffer);

        // Null-terminate the remaining buffer
        buffers->inputBuffer[leftoverInBuffer] = '\0';
        lineSeparator(buffers);
    } else {
        buffers->lineSeperatorBuffer[0] = '\0';
    }

    printf("The og buffer: %s\n", buffers->inputBuffer);
    printf("The 80 char buffer: %s\n", buffers->lineSeperatorBuffer);

    return NULL;
}

int main() {
    struct readCharsBuffer buffers;
    buffers.inputBuffer = malloc(1081);
    buffers.lineSeperatorBuffer = malloc(FULL_BUFFER_SIZE + 1); // +1 for null-terminator

    // Initialize buffers
    memset(buffers.inputBuffer, 0, 1081);
    memset(buffers.lineSeperatorBuffer, 0, FULL_BUFFER_SIZE + 1);

    pthread_t input_thread, lineSeparator_thread, plusSign_thread, final_thread;

    while (!stopValue) {
        pthread_create(&input_thread, NULL, readChars, (void *)&buffers);
        // pthread_create(&lineSeparator_thread, NULL, lineSeparator, (void *)&buffers);
        // pthread_create(&plusSign_thread, NULL, replacePlusSign, (void *)&buffers);
        // pthread_create(&final_thread, NULL, print80Chars, (void *)&buffers);
        pthread_join(input_thread, NULL);
        // pthread_join(lineSeparator_thread, NULL);
        // pthread_join(plusSign_thread, NULL);
        // pthread_join(final_thread, NULL);
    }

    free(buffers.inputBuffer);
    free(buffers.lineSeperatorBuffer);

    return 0;
}
