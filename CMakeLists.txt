# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("OpenGLMiniEngine" LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  add_compile_options(/utf-8)
endif()
# 搜索所有的cpp，加入SRCS变量中
#aux_source_directory(. SRCS)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include)

link_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib)

# 将文件夹纳入到编译系统
add_subdirectory(wrapper)
add_subdirectory(application)
add_subdirectory(glframework)
add_subdirectory(imgui)
add_subdirectory(editor)
# 将源代码添加到此项目的可执行文件。
add_executable (MiniEngine "OpenGL.cpp" "glad.c")
# 将lib接入OpenGL
target_link_libraries(MiniEngine glfw3.lib wrapper app fw imguilib editor)

add_custom_command(TARGET MiniEngine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:MiniEngine>/assets"
  COMMENT "Copying runtime assets"
)

if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MiniEngine)
endif()
