project('wayward', 'c')

add_project_arguments('-DWLR_USE_UNSTABLE', language: 'c')

wlroots =   dependency('wlroots', fallback: ['wlroots', 'wlroots'])
wayland_server = dependency('wayland-server')
executable('wayward', ['src/main.c','src/output.c'],
     
    dependencies : [wlroots, wayland_server]
)
