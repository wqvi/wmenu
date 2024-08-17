#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "menu.h"
#include "wayland.h"

static void read_items(struct menu *menu) {
	char buf[sizeof menu->input];
	while (fgets(buf, sizeof buf, stdin)) {
		char *p = strchr(buf, '\n');
		if (p) {
			*p = '\0';
		}
		menu_add_item(menu, strdup(buf), false);
	}
}

static void print_item(struct menu *menu, char *text, int i, bool exit) {
	printf("%s\n%d\n", text, i);
	fflush(stdout);
	if (exit) {
		menu->exit = true;
	}
}

int main(int argc, char *argv[]) {
	int flags = fcntl(STDIN_FILENO, F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(STDIN_FILENO, F_SETFL, flags) < 0) {
		return EXIT_FAILURE;
	}

	flags = fcntl(STDOUT_FILENO, F_GETFL);
	flags |= O_NONBLOCK;
	if (fcntl(STDOUT_FILENO, F_SETFL, flags) < 0) {
		return EXIT_FAILURE;
	}

	struct menu *menu = menu_create(print_item);
	menu_getopts(menu, argc, argv);
	menu->event_amount = 3;
	read_items(menu);
	int status = menu_run(menu);
	menu_destroy(menu);
	return status;
}
