#include "ui.h"
#include "debug.h"

#include <stdlib.h>
#include <locale.h>
#include <string.h>

UI* initUi() {
	UI* ui = (UI*) malloc(sizeof(UI));
	setlocale(LC_ALL, "");
	ui->win = initscr();
	cbreak();
	noecho();

	ui->titleBar = NULL;
	ui->statusBar = NULL;
	ui->linePrintBuffer = NULL;

	updateWidth(ui);

	ui->titleBarLeftIndex = 0;
	ui->titleBarRightIndex = ui->winWidth - 1;

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

		if(ui->titleBar != NULL) {
			free(ui->titleBar);
			free(ui->statusBar);
			free(ui->linePrintBuffer);
		}

		ui->titleBar = (char*) malloc(ui->winWidth);
		clearTitleBar(ui);

		ui->statusBar = (char*) malloc(ui->winWidth);
		clearStatusBar(ui);

		ui->linePrintBuffer = (char*) malloc(ui->winWidth);

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

	static int viewTopLine = 0;
	int y = 0;
	int x = 0;

	// get cursor pos
	for(int i = 0; i < editor->bufferIndex; i++) {
		x++;

		if(editor->buffer[i] == '\n') {
			y++;
			x = 0;
		}
	}

	// shift view if necessary
	if(y < viewTopLine)
		viewTopLine--;
	else if(y - viewTopLine >= ui->bufferEditorHeight)
		viewTopLine++;

	char* sourceBuffer = editor->buffer;
	int insertBufferOffset = 0;
	int lineCount = 0;
	int lineIndex = 0;

	for(int i = 0; i < editor->bufferLen; i++) {
		// change buffer for insert mode
		if(editor->mode == INSERT && sourceBuffer == editor->buffer && sourceBuffer[i] == 0) {
			sourceBuffer = editor->insertBuffer;
			insertBufferOffset = i;
		}

		if(lineCount >= viewTopLine) {
			// add to print buffer
			ui->linePrintBuffer[lineIndex] = sourceBuffer[i - insertBufferOffset];
			lineIndex++;
			ui->linePrintBuffer[lineIndex] = 0;

			// print on newlines
			if(sourceBuffer[i - insertBufferOffset] == '\n') {
				ui->linePrintBuffer[lineIndex] = 0;
				addstr(ui->linePrintBuffer);
				lineIndex = 0;
			} // ***line wrapping
			else if(lineIndex >= ui->winWidth) {
				ui->linePrintBuffer[lineIndex] = 0;
				addstr(ui->linePrintBuffer);
				lineIndex = 0;
				
				lineCount++;
			}

			// printed max lines of text
			if(lineCount - viewTopLine >= ui->bufferEditorHeight) {
				break;
			}
		}

		if(sourceBuffer[i - insertBufferOffset] == '\n') {
			lineCount++;
		}
	}

	if(lineIndex != 0 && lineCount - viewTopLine < ui->bufferEditorHeight)
		addstr(ui->linePrintBuffer);

	move(y - viewTopLine + 1, x);
}
