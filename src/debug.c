#include <stdio.h>

FILE* fout = NULL;

void openDebugBuffer(const char* path) {
	if(fout == NULL) {
		fout = fopen(path, "w");
	}
}
void closeDebugBuffer() {
	if(fout != NULL) {
		fclose(fout);
		fout = NULL;
	}
}

void debugPrint(const char* msg) {
	fputs(msg, fout);
	fputc('\n', fout);
}
void debugError(const char* msg) {
	fputs("ERROR: ", fout);
	fputs(msg, fout);
	fputc('\n', fout);
}
