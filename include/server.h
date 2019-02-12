#pragma once

#include <wayland-server.h>

/*Holds the compositor's state*/
struct wayward_server {
    struct wl_display *wl_display;
    struct wl_event_loop *wl_event_loop;
    struct wlr_backend *backend;
    struct wl_listener new_output;
    struct wl_list outputs;
};