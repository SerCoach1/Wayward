#define _POSIX_C_SOURCE 200809L
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_output.h>
#include "include/output.h"
#include "include/server.h"
#include <stdlib.h>
#include <wlr/types/wlr_surface.h>
#include <stdio.h>
#include <wlr/types/wlr_matrix.h>
#include <time.h>
#include <wlr/render/wlr_renderer.h>

void output_destroy_notify(struct wl_listener *listener, void *data) {
struct wayward_output *output = wl_container_of(listener, output, destroy);
wl_list_remove(&output->link);
wl_list_remove(&output->destroy.link);
wl_list_remove(&output->frame.link);
free(output);
}

void output_frame_notify(struct wl_listener *listener, void *data) {
  struct wayward_output *output = wl_container_of(listener,output, frame);
  struct wayward_server *server = output->server;
  struct wlr_output *wlr_output = data;

  struct wlr_renderer *renderer = wlr_backend_get_renderer(wlr_output->backend);


	struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  if(!wlr_output_make_current(wlr_output, NULL)) {
    printf("did not make current output"); //just for testing purposes
    return;
  }



  int height, width;
  wlr_output_effective_resolution(wlr_output,&width, &height);

  wlr_renderer_begin(renderer, width,height);

  float color[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
  wlr_renderer_clear(renderer, color);//render red colour

  struct wl_resource *_surface;
  wl_resource_for_each(_surface, &server->compositor->surface_resources) {
    struct wlr_surface *surface = wlr_surface_from_resource(_surface);
    struct wlr_texture * texture = wlr_surface_get_texture(surface);
    if(!wlr_surface_has_buffer(surface)) {
      continue;
    }
    struct wlr_box render_box = {
      .x = 0, .y = 0,
      .width = surface->current.width,
      .height = surface->current.width
    };
    float matrix[9];
    wlr_matrix_project_box(matrix, &render_box, surface->current.transform, 0, wlr_output->transform_matrix);

    wlr_render_texture_with_matrix(renderer, texture, matrix, 1);
    wlr_surface_send_frame_done(surface, &now);
  }



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