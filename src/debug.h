#ifndef AB_DEBUG_H
#define AB_DEBUG_H

void openDebugBuffer(const char* path);
void closeDebugBuffer();

void debugPrint(const char* msg);
void debugError(const char* msg);

#endif
