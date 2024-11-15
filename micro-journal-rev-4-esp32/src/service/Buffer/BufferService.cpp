#include "BufferService.h"
#include "app/app.h"

// Create the buffer and a pointer to track the current position
static char buffer[BUFFER_LEN];
static size_t buffer_pos = 0;

// Clears the buffer
void buffer_clear() {
    buffer_pos = 0; // Reset position
    buffer[0] = '\0'; // Null-terminate the buffer
}

// Retrieves the current content of the buffer
char* buffer_get() {
    return buffer;
}

// Adds a character to the buffer
void buffer_add(char key) {
    if (buffer_pos < BUFFER_LEN - 1) { // Check if there's space left in the buffer
        buffer[buffer_pos++] = key;  // Add character and increment position
        buffer[buffer_pos] = '\0';   // Null-terminate the buffer
    }
}

// Removes the last character from the buffer
void buffer_remove() {
    if (buffer_pos > 0) { // Ensure there's something to remove
        buffer_pos--;     // Decrement position
        buffer[buffer_pos] = '\0'; // Null-terminate the buffer
    }
}