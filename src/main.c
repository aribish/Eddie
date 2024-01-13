#include "ui.h"
#include "editor.h"

#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

UI* ui;
EDITOR* editor;
char barFormatBuffer[64];

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Error: File path required as only argument.\n");
		return 1;
	}

	ui = initUi();
	editor = initEditor();
	loadBuffer(editor, argv[1]);

	char input = 0;
	while(!editorFlags(editor, JUST_QUIT)) {
		updateWidth(ui);
		
		clear();
		clearTitleBar(ui);
		clearStatusBar(ui);	

		// title bar
		sprintf(barFormatBuffer, "File: %s", editor->bufferPath);
		appendTitleBarRight(ui, barFormatBuffer);

		// status bar
		switch(editor->mode) {
			case COMMAND:
				appendStatusBarLeft(ui, "[COMMAND] ");
				break;
			case REPLACE:
				appendStatusBarLeft(ui, "[REPLACE] ");
				break;
			case INSERT:
				appendStatusBarLeft(ui, "[INSERT] ");
				break;
			default:
				appendStatusBarLeft(ui, "[ERROR] ");
		}

		sprintf(barFormatBuffer, "| Cursor: (%d, %d/%d) | i: %d/%d | $ [0x%02x (%d)] | Chunks: %d | ", 
				getCursorX(editor), getCursorY(editor), getBufferLines(editor) - 1,
				editor->bufferIndex, editor->bufferLen, input, input, editor->bufferChunks);
		appendStatusBarRight(ui, barFormatBuffer);

		printBuffer(ui, editor);
		printTitleBar(ui);
		printStatusBar(ui);
		move(getCursorY(editor) - editor->viewTopLine + 1, getCursorX(editor));

		input = getch();
		handleInput(editor, ui->win, input);
	}

	exit:
	endEditor(editor);
	endUi(ui);
	return 0;
}
