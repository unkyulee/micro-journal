#ifndef BufferService_h
#define BufferService_h

#define BUFFER_LEN 256

void buffer_clear();
char* buffer_get();
void buffer_add(char key);
void buffer_remove();

#endif