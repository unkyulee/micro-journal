#pragma once

// counts word count until the last buffer
int wordcounter_file(const char* filename);

// counts word count inside the buffer
int wordcounter_buffer(const char* buffer);