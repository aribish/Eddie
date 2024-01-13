#ifndef AB_EDITOR_H
#define AB_EDITOR_H

#include <stdbool.h>
#include <ncurses.h>

#define NO_STATE 0
#define JUST_QUIT 1
#define JUST_SAVED (1 >> 1)

typedef enum {
	COMMAND,
	REPLACE,
	INSERT
} EDITOR_MODE;

typedef struct {
	EDITOR_MODE mode;
	unsigned int flags;

	char* bufferPath;
	char* buffer;
	int bufferChunkSize; // size of a chunk (eg. 512)
	int bufferIndex; // the index of the cursor
	int bufferLen; // the size of the buffer in characters
	int bufferChunks; // the size of the buffer in chunks
	
	char* insertBuffer; // shit after the cursor
	
	int viewTopLine;
} EDITOR;

EDITOR* initEditor();
void endEditor(EDITOR* editor);

bool editorFlags(EDITOR* editor, unsigned int flags);
void handleInput(EDITOR* editor, WINDOW* win, char input); // RESETS ALL FLAGS

void loadBuffer(EDITOR* editor, char* bufferPath);
void writeBuffer(EDITOR* editor);

int getCursorX(EDITOR* editor);
int getCursorY(EDITOR* editor);

int getBufferLines(EDITOR* editor);

#endif
