#ifndef AB_UI_H
#define AB_UI_H

#include <ncurses.h>
#include "editor.h"

typedef struct {
	WINDOW* win;
	int winWidth;

	char* titleBar;
	int titleBarLeftIndex;
	int titleBarRightIndex;

	char* statusBar;
	int statusBarLeftIndex;
	int statusBarRightIndex;

	int bufferEditorHeight;
} UI;

UI* initUi();
void endUi(UI* ui);

void updateWidth(UI* ui);

void appendTitleBarLeft(UI* ui, char* str);
void appendTitleBarRight(UI* ui, char* str);
void printTitleBar(UI* ui);
void clearTitleBar(UI* ui);

void appendStatusBarLeft(UI* ui, char* str);
void appendStatusBarRight(UI* ui, char* str);
void printStatusBar(UI* ui);
void clearStatusBar(UI* ui);

void printBuffer(UI* ui, EDITOR* editor);

#endif
