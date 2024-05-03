#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "menu.h"
#include "wayland.h"
#include "xdg-activation-v1-client-protocol.h"

static void read_items(struct menu *menu) {
	char buf[sizeof menu->input];
	while (fgets(buf, sizeof buf, stdin)) {
		char *p = strchr(buf, '\n');
		if (p) {
			*p = '\0';
		}
		menu_add_item(menu, strdup(buf));
	}
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
	execlp(exe->name, exe->name, NULL);

	fprintf(stderr, "Failed to execute selection: %s\n", strerror(errno));
	free(exe->name);
	free(exe);
	exit(EXIT_FAILURE);
}

static const struct xdg_activation_token_v1_listener activation_token_listener = {
	.done = activation_token_done,
};

static void exec(struct menu *menu) {
	if (!menu->sel) {
		return;
	}

	struct executable *exe = calloc(1, sizeof(struct executable));
	exe->menu = menu;
	exe->name = strdup(menu->sel->text);

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
