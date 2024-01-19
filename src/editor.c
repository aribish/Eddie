#include "editor.h"

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

EDITOR* initEditor() {
	EDITOR* editor = (EDITOR*) malloc(sizeof(EDITOR));
	editor->mode = COMMAND;
	editor->flags = 0;
	editor->bufferChunkSize = 512;
}
void endEditor(EDITOR* editor) {
	free(editor);
}

bool editorFlags(EDITOR* editor, unsigned int flags) {
	return editor->flags & flags;
}

void checkBufferFull(EDITOR* editor) {
	if(editor->bufferLen >= editor->bufferChunks * editor->bufferChunkSize) {
		editor->bufferChunks++;
		char* newBuffer = malloc(editor->bufferChunks * editor->bufferChunkSize);

		if(editor->mode == INSERT) {
			for(int i = 0; i < editor->bufferIndex; i++) {
				newBuffer[i] = editor->buffer[i];
			}
			newBuffer[editor->bufferIndex] = 0;
		}
		else {
			for(int i = 0; i < editor->bufferLen; i++) {
				newBuffer[i] = editor->buffer[i];
			}
			newBuffer[editor->bufferLen] = 0;
		}
		
		free(editor->buffer);
		editor->buffer = newBuffer;
	}
}

// local function
void handleCommandInput(EDITOR* editor, char input) {
	if(input == 'q')
		editor->flags |= JUST_QUIT;
	else if(input == 'r')
		editor->mode = REPLACE;
	else if(input == 'i') {
		editor->mode = INSERT;
		editor->insertBuffer = (char*) malloc(editor->bufferLen - editor->bufferIndex + 1);

		for(int i = 0; i < editor->bufferLen - editor->bufferIndex; i++) {
			editor->insertBuffer[i] = editor->buffer[editor->bufferIndex + i];
		}
		editor->insertBuffer[editor->bufferLen - editor->bufferIndex] = 0;

		editor->buffer[editor->bufferIndex] = 0;
	}
	else if(input == 'w') {
		writeBuffer(editor);
		editor->flags |= JUST_SAVED;
	}
	else if(input == 'l') {
		openDebugBuffer("editor_log");
		debugError("Testing error print.");
		debugPrint(editor->buffer);
		closeDebugBuffer();
	} // CURSOR MOVEMENT
	else if(input == 68 && editor->bufferIndex > 0) { // left				
		editor->bufferIndex--;
	}
	else if(input == 67 && editor->bufferIndex < editor->bufferLen) { // right
		editor->bufferIndex++;
	}
	else if(input == 65) { // up
		// make sure cursor is not already on the first line + get x
		int x = 0;
		while(editor->buffer[editor->bufferIndex - 1 - x] != '\n' && editor->bufferIndex - 1 - x > 0) {
			x++;
		}

		if(editor->bufferIndex == x)
			return;

		bool onPrevLine = false;
		int prevLineStart = 0;
		int prevLineLen = 0;

		for(int i = editor->bufferIndex - 1; i > 0; i--) {
			if(onPrevLine) {
				prevLineLen++;
			}
			if(editor->buffer[i] == '\n') {
				if(onPrevLine) {
					prevLineStart = i + 1;
					break;
				}
				else
					onPrevLine = true;
			}
		}

		if(prevLineLen < x) {
			x = prevLineLen - 1;
		}

		editor->bufferIndex = prevLineStart + x;
	}
	else if(input == 66) { // down
		// make sure cursor is not already at the bottom line
		for(int i = editor->bufferIndex + 1; i <= editor->bufferLen; i++) {
			if(i == editor->bufferLen)
				return;
			else if(editor->buffer[i] == '\n')
				break;
		}

		// get x
		int x = 0;
		while(editor->buffer[editor->bufferIndex - 1 - x] != '\n' && editor->bufferIndex - 1 - x > 0) {
			x++;
		}

		bool onNextLine = false;
		int nextLineStart = 0;
		int nextLineLen = 0;

		for(int i = editor->bufferIndex; i < editor->bufferLen; i++) {
			if(onNextLine) {
				nextLineLen++;
			}
			if(editor->buffer[i] == '\n') {
				if(onNextLine) {
					nextLineLen++;
					break;
				}
				else {
					onNextLine = true;
					nextLineStart = i + 1;
				}
			}
		}

		editor->bufferIndex = nextLineStart + x;
	}
}

// local function
void handleInsertReplaceInput(EDITOR* editor, char input) {
	if(input == 27) { // esc
		if(editor->mode == INSERT) {
			strcat(editor->buffer, editor->insertBuffer);
			free(editor->insertBuffer);
		}

		editor->mode = COMMAND;
	}
	else if(input == 127) { // backspace/delete
		if(editor->mode == REPLACE) {
			if(editor->bufferIndex == editor->bufferLen) {
				editor->bufferIndex--;
				editor->bufferLen--;
				editor->buffer[editor->bufferIndex] = 0; 
			}
		}
		else if(editor->bufferIndex > 0) {
			editor->bufferIndex--;
			editor->bufferLen--;
			editor->buffer[editor->bufferIndex] = 0;
		}
	}
	else {
		if(editor->mode == REPLACE && editor->buffer[editor->bufferIndex] == '\n') {
			return;
		}

		editor->buffer[editor->bufferIndex] = input;
		editor->bufferIndex++;
		editor->bufferLen++;

		if((editor->mode == REPLACE && editor->bufferIndex == editor->bufferLen) || editor->mode == INSERT) {
			editor->buffer[editor->bufferIndex] = 0;
		}
		
		checkBufferFull(editor);
	}
}

void handleInput(EDITOR* editor, WINDOW* win, char input) {
	editor->flags = 0;

	if(editor->mode == COMMAND)
		handleCommandInput(editor, input);
	else if(editor->mode == REPLACE || editor->mode == INSERT)
		handleInsertReplaceInput(editor, input);
}

void loadBuffer(EDITOR* editor, char* bufferPath) {
	editor->buffer = (char*) malloc(editor->bufferChunkSize);
	editor->buffer[0] = 0;
	editor->bufferPath = bufferPath;

	editor->bufferIndex = 0;
	editor->bufferLen = 0;
	editor->bufferChunks = 1;

	FILE* fin = fopen(bufferPath, "r");
	if(fin == NULL)
		return;

	char c;
	while ( (c = fgetc(fin)) != EOF) {
		editor->buffer[editor->bufferIndex] = c;
		editor->bufferIndex++;
		editor->bufferLen++;

		checkBufferFull(editor);
	}

	editor->bufferIndex = 0;
}
void writeBuffer(EDITOR* editor) {
	FILE* fout = fopen(editor->bufferPath, "w");
	fwrite(editor->buffer, strlen(editor->buffer), 1, fout);
	fclose(fout);
}

int getBufferLines(EDITOR* editor) {
	int lines = 1;
	for(int i = 0; i < editor->bufferLen; i++) {
		if(editor->buffer[i] == '\n')
			lines++;
	}

	return lines;
}
