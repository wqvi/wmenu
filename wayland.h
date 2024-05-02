#ifndef WMENU_WAYLAND_H
#define WMENU_WAYLAND_H

#include "menu.h"
#include <wayland-client-protocol.h>

struct wl_context;

int menu_run(struct menu *menu);

int context_get_scale(struct wl_context *context);
struct wl_shm *context_get_shm(struct wl_context *context);
struct wl_surface *context_get_surface(struct wl_context *context);
struct xkb_state *context_get_xkb_state(struct wl_context *context);
bool context_paste(struct wl_context *context);

#endif
