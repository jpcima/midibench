project('simple', 'cpp')

sdl2 = dependency('sdl2')
opengl = dependency('GL')
glew = dependency('glew')
rtmidi = dependency('rtmidi')

sources = [
        'sources/main.cpp',
        'sources/widgets.cpp',
        'sources/midibench.cpp',
]
includes = [
]

# ImGui
sources += [
        'thirdparty/imgui/imgui.cpp',
        'thirdparty/imgui/imgui_draw.cpp',
        'thirdparty/imgui/imgui_tables.cpp',
        'thirdparty/imgui/imgui_widgets.cpp',
        'thirdparty/imgui/backends/imgui_impl_sdl.cpp',
        'thirdparty/imgui/backends/imgui_impl_opengl3.cpp',
]
includes += [
        'thirdparty/imgui',
        'thirdparty/imgui/backends',
]

#
executable(
  'midibench',
  sources : sources,
  include_directories : includes,
  dependencies : [ sdl2, opengl, glew, rtmidi ],
  install : true)

#
install_data(
  ['resources/linux/midibench.desktop'],
  install_dir : join_paths(get_option('datadir'), 'applications'))
install_data(
  ['resources/icons/512x512/midibench.png'],
  install_dir : join_paths(get_option('datadir'), 'icons/hicolor/512x512/apps'))
