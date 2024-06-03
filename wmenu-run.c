#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "menu.h"
#include "wayland.h"
#include "xdg-activation-v1-client-protocol.h"

static void read_items(struct menu *menu) {
	char *path = strdup(getenv("PATH"));
	for (char *p = strtok(path, ":"); p != NULL; p = strtok(NULL, ":")) {
		DIR *dir = opendir(p);
		if (dir == NULL) {
			continue;
		}
		for (struct dirent *ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
			if (ent->d_name[0] == '.') {
				continue;
			}
			menu_add_item(menu, strdup(ent->d_name), true);
		}
		closedir(dir);
	}
	free(path);
}

struct executable {
	struct menu *menu;
	char *name;
};

static void activation_token_done(void *data, struct xdg_activation_token_v1 *activation_token,
	const char *token) {
	struct executable *exe = data;
	xdg_activation_token_v1_destroy(activation_token);
	menu_destroy(exe->menu);

	setenv("XDG_ACTIVATION_TOKEN", token, true);
	char* cmd[] = {"/bin/sh", "-c", exe->name, NULL};
	execvp(cmd[0], (char**)cmd);

	fprintf(stderr, "Failed to execute selection: %s\n", strerror(errno));
	free(exe->name);
	free(exe);
	exit(EXIT_FAILURE);
}

static const struct xdg_activation_token_v1_listener activation_token_listener = {
	.done = activation_token_done,
};

static void exec(struct menu *menu) {
	struct executable *exe = calloc(1, sizeof(struct executable));
	exe->menu = menu;
	exe->name = strdup(menu->input);

	struct xdg_activation_v1 *activation = context_get_xdg_activation(menu->context);
	struct xdg_activation_token_v1 *activation_token = xdg_activation_v1_get_activation_token(activation);
	xdg_activation_token_v1_set_surface(activation_token, context_get_surface(menu->context));
	xdg_activation_token_v1_add_listener(activation_token, &activation_token_listener, exe);
	xdg_activation_token_v1_commit(activation_token);
}

int main(int argc, char *argv[]) {
	struct menu *menu = menu_create();
	menu->callback = exec;
	menu_getopts(menu, argc, argv);
	read_items(menu);
	int status = menu_run(menu);
	menu_destroy(menu);
	return status;
}
