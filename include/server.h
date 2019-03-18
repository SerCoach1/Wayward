#pragma once

#include <wayland-server-core.h>
#include <wlr/types/wlr_compositor.h>

/*Holds the compositor's state*/
struct wayward_server {
    struct wl_display *wl_display;
    struct wl_event_loop *wl_event_loop;
    struct wlr_backend *backend;

    struct wlr_compositor * compositor;

    struct wl_listener new_output; // gets signaled when new output is added
    struct wl_list outputs;
};