#include "ui.h"

#include <stdlib.h>
#include <locale.h>
#include <string.h>

UI* initUi() {
	UI* ui = (UI*) malloc(sizeof(UI));
	setlocale(LC_ALL, "");
	ui->win = initscr();
	cbreak();
	noecho();

	updateWidth(ui);

	ui->titleBar = malloc(ui->winWidth);
	ui->titleBarLeftIndex = 0;
	ui->titleBarRightIndex = ui->winWidth - 1;

	ui->statusBar = malloc(ui->winWidth);
	ui->statusBarLeftIndex = 0;
	ui->statusBarRightIndex = ui->winWidth - 1;
}
void endUi(UI* ui) {
	free(ui);
	endwin();
}

void updateWidth(UI* ui) {
	int width, height;
	getmaxyx(ui->win, height, width); // '&' is NOT required because getmaxyx() is a macro!

	if(width != ui->winWidth) {
		ui->winWidth = width;

		if(ui->titleBar != NULL && ui->statusBar != NULL) {
			free(ui->titleBar);
			free(ui->statusBar);
		}

		ui->titleBar = (char*) malloc(ui->winWidth);
		clearTitleBar(ui);

		ui->statusBar = (char*) malloc(ui->winWidth);
		clearStatusBar(ui);
	}
	if(height - 2 != ui->bufferEditorHeight) {
		ui->bufferEditorHeight = height - 2;
	}
}

void appendTitleBarLeft(UI* ui, char* str) {
	if(ui->titleBarLeftIndex + strlen(str) >= ui->winWidth)
		return;

	for(int i = 0; i < strlen(str); i++) {
		ui->titleBar[ui->titleBarLeftIndex + i] = str[i];
	}

	ui->titleBarLeftIndex += strlen(str);
}

void appendTitleBarRight(UI* ui, char* str) {
	if(ui->titleBarRightIndex - strlen(str) < 0)
		return;

	for(int i = 0; i < strlen(str); i++) {
		ui->titleBar[ui->titleBarRightIndex - (strlen(str) - i)] = str[i];
	}

	ui->titleBarRightIndex -= strlen(str);
}

void clearTitleBar(UI* ui) {
	for(int i = 0; i < ui->winWidth; i++) {
		ui->titleBar[i] = ' ';
	}

	ui->titleBar[ui->winWidth] = 0;
	ui->titleBarLeftIndex = 0;
	ui->titleBarRightIndex = ui->winWidth - 1;
}

void printTitleBar(UI* ui) {
	move(0, 0);
	addstr(ui->titleBar);
}

void appendStatusBarLeft(UI* ui, char* str) {
	if(ui->statusBarLeftIndex + strlen(str) >= ui->winWidth)
		return;

	for(int i = 0; i < strlen(str); i++) {
		ui->statusBar[ui->statusBarLeftIndex + i] = str[i];
	}

	ui->statusBarLeftIndex += strlen(str);
}

void appendStatusBarRight(UI* ui, char* str) {
	if(ui->statusBarRightIndex - strlen(str) < 0)
		return;

	for(int i = 0; i < strlen(str); i++) {
		ui->statusBar[ui->statusBarRightIndex - (strlen(str) - i)] = str[i];
	}

	ui->statusBarRightIndex -= strlen(str);
}

void clearStatusBar(UI* ui) {
	for(int i = 0; i < ui->winWidth; i++) {
		ui->statusBar[i] = ' ';
	}

	ui->statusBar[ui->winWidth] = 0;
	ui->statusBarLeftIndex = 0;
	ui->statusBarRightIndex = ui->winWidth - 1;
}

void printStatusBar(UI* ui) {
	move(ui->bufferEditorHeight + 1, 0);
	addstr(ui->statusBar);
}

void printBuffer(UI* ui, EDITOR* editor) {
	move(1, 0);

	// allocating space every frame is VERY expensive
	// figure how NOT to do that
	char* viewBuffer = NULL;
	char* sourceBuffer = editor->buffer;
	int newlineCount = 0;
	bool writing = false;
	int start = 0;
	int insertBufferOffset = 0;

	for(int i = 0; i < editor->bufferLen; i++) {
		if(editor->mode == INSERT && sourceBuffer == editor->buffer && editor->buffer[i] == 0) {
			insertBufferOffset = i;
			sourceBuffer = editor->insertBuffer;
		}

		if(!writing) {
			if(newlineCount == editor->viewTopLine) {
				start = i;
				viewBuffer = (char*) malloc(editor->bufferLen - start);
				writing = true;
			}

			if(sourceBuffer[i - insertBufferOffset] == '\n')
				newlineCount++;
		}
		
		if(writing) {
			viewBuffer[i - start] = sourceBuffer[i - insertBufferOffset];
		}
	}

	if(viewBuffer != NULL) {
		viewBuffer[editor->bufferLen - start] = 0;
		addstr(viewBuffer);
		free(viewBuffer);
	}

	// Move the cursor to its proper position (**this may be unnecessary)
	if(editor->bufferIndex != editor->bufferLen) {
		move(getCursorY(editor) + 1, getCursorX(editor));
	}
}
