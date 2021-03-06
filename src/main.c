#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wlr/types/wlr_screencopy_v1.h> // use this instead of screenshooter
#include <wlr/types/wlr_idle.h>
#include <wlr/types/wlr_data_device.h>
#include "include/server.h"
#include "include/output.h"
#include <wlr/types/wlr_xdg_shell.h> //V6 is now obsolete so use this one

int main(int argc, char **argv) {
  struct wayward_server server;

  server.wl_display = wl_display_create();

  assert(server.wl_display); // check is display created successfully



  const char *socket = wl_display_add_socket_auto(server.wl_display);
  assert(socket);

  server.wl_event_loop = wl_display_get_event_loop(server.wl_display);

  assert(server.wl_event_loop);

  /*NULL uses backend's default renderer*/
  server.backend = wlr_backend_autocreate(server.wl_display, NULL); //create wayland backend instead of auto later?s
  assert(server.backend);

  wl_list_init(&server.outputs);

  server.new_output.notify = new_output_notify;
  wl_signal_add(&server.backend->events.new_output, &server.new_output);

  if(!wlr_backend_start(server.backend)) {
    fprintf(stderr, "Failed to start backend\n");
    wl_display_destroy(server.wl_display);
    return 1;
  }

  printf("Wayward running on wayland display '%s'\n", socket);
  setenv("WAYLAND_DISPLAY", socket, true);

  wl_display_init_shm(server.wl_display);
  wlr_data_device_manager_create(server.wl_display);
  wlr_idle_create(server.wl_display);

  server.compositor = wlr_compositor_create(server.wl_display, wlr_backend_get_renderer(server.backend));

  wlr_xdg_shell_create(server.wl_display);

  wl_display_run(server.wl_display);
  wl_display_destroy(server.wl_display);

  return 0;
}
