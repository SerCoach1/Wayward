#define _POSIX_C_SOURCE 200809L
#include <wlr/types/wlr_output.h>
#include "include/output.h"
#include "include/server.h"
#include <wlr/render/wlr_renderer.h>
#include <stdlib.h>

void output_destroy_notify(struct wl_listener *listener, void *data) {
struct wayward_output *output = wl_container_of(listener, output, destroy);
wl_list_remove(&output->link);
wl_list_remove(&output->destroy.link);
wl_list_remove(&output->frame.link);
free(output);
}

void output_frame_notify(struct wl_listener *listener, void *data) {
  struct wayward_output *output = wl_container_of(listener,output, frame);
  struct wlr_output *wlr_output = data;

  struct wlr_renderer *renderer = wlr_backend_get_renderer(wlr_output->backend);

  wlr_output_make_current(wlr_output, NULL);
  wlr_renderer_begin(renderer, wlr_output->width,wlr_output->height);

  float color[4] = {1.0, 0, 0, 1.0}; 
  wlr_renderer_clear(renderer, color);//render red colour

  wlr_output_swap_buffers(wlr_output, NULL, NULL);
  wlr_renderer_end(renderer);
}

void new_output_notify(struct wl_listener * listener, void *data) {
  struct wayward_server *server = wl_container_of(listener, server, new_output);
  struct wlr_output *wlr_output = data;

  if(!wl_list_empty(&wlr_output->modes)) {
    struct wlr_output_mode * mode = wl_container_of(wlr_output->modes.prev, mode, link);
    wlr_output_set_mode(wlr_output, mode);
  }

  struct wayward_output * output = calloc(1, sizeof(struct wayward_output));
  clock_gettime(CLOCK_MONOTONIC, &output->last_frame);
  output->server = server;
  output->wlr_output = wlr_output;
  wl_list_insert(&server->outputs, &output->link);

  output->destroy.notify = output_destroy_notify;
  wl_signal_add(&wlr_output->events.destroy, &output->destroy);
  
  output->frame.notify = output_frame_notify;
  wl_signal_add(&wlr_output->events.frame, &output->frame);

  wlr_output_create_global(wlr_output);
}