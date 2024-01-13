eddie: main.o editor.o ui.o debug.o
	gcc $^ -o $@ -lncurses

main.o: src/main.c
	gcc $< -o $@ -c

editor.o: src/editor.c
	gcc $< -o $@ -c

ui.o: src/ui.c
	gcc $< -o $@ -c

debug.o: src/debug.c
	gcc $< -o $@ -c

clean:
	rm -f *.o
