project('wayward', 'c')

add_project_arguments('-DWLR_USE_UNSTABLE', language: 'c')

pixman         = dependency('pixman-1')
wlroots = dependency('wlroots', fallback: ['wlroots', 'wlroots'])
wayland_server = dependency('wayland-server')
executable('wayward',
    'main.c', 
    dependencies : [wlroots, pixman, wayland_server]
)
