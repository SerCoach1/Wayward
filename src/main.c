#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_output.h>
#include "include/server.h"
#include "include/output.h"

int main(int argc, char **argv) {
  struct wayward_server server;

  server.wl_display = wl_display_create();

  assert(server.wl_display); // check is display created successfully

  server.wl_event_loop = wl_display_get_event_loop(server.wl_display);

  assert(server.wl_event_loop);

  /*NULL uses backend's default renderer*/
  server.backend = wlr_backend_autocreate(server.wl_display, NULL);
  assert(server.backend);

  wl_list_init(&server.outputs);

  server.new_output.notify = new_output_notify;
  wl_signal_add(&server.backend->events.new_output, &server.new_output);

  if(!wlr_backend_start(server.backend)) {
    fprintf(stderr, "Failed to start backend\n");
    wl_display_destroy(server.wl_display);
    return 1;
  }

  wl_display_run(server.wl_display);
  wl_display_destroy(server.wl_display);

  return 0;
}