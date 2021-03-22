project('simple', 'cpp')

sdl2 = dependency('sdl2')
opengl = dependency('GL')
glew = dependency('glew')
rtmidi = dependency('rtmidi')

sources = [
        'sources/main.cpp',
        'sources/widgets.cpp',
        'sources/midisurface.cpp',
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
  'midisurface',
  sources : sources,
  include_directories : includes,
  dependencies : [ sdl2, opengl, glew, rtmidi ],
  install : true)
