cmake_minimum_required(VERSION 3.20)
project(pl0 LANGUAGES CXX)

option(PL0_ENABLE_ASAN "Enable address and undefined behavior sanitizers" ON)
option(PL0_BUILD_TOOLS "Build standalone tooling binaries" ON)
option(PL0_BUILD_TESTS "Build unit tests" ON)
option(PL0_BUILD_GUI "Build Qt graphical interface" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(PL0_SOURCES
    src/Codegen.cpp
    src/Driver.cpp
    src/Diagnostics.cpp
    src/Lexer.cpp
    src/PCode.cpp
    src/Parser.cpp
    src/Symbol.cpp
    src/SymbolTable.cpp
    src/Token.cpp
    src/Utility.cpp
    src/VM.cpp
)

add_library(pl0 STATIC ${PL0_SOURCES})
add_library(pl0::pl0 ALIAS pl0)

target_include_directories(pl0
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(pl0 PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
elseif(MSVC)
  target_compile_options(pl0 PRIVATE /W4 /WX)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug" AND PL0_ENABLE_ASAN)
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(PL0_SANITIZER_FLAGS -fsanitize=address -fsanitize=undefined)
    target_compile_options(pl0 PRIVATE ${PL0_SANITIZER_FLAGS})
    target_link_options(pl0 PUBLIC ${PL0_SANITIZER_FLAGS})
  endif()
endif()

if(PL0_BUILD_TOOLS)
  add_executable(pl0-cli src/main.cpp)
  target_link_libraries(pl0-cli PRIVATE pl0::pl0)
  set_target_properties(pl0-cli PROPERTIES OUTPUT_NAME pl0)

  add_executable(pl0c tools/pl0c.cpp)
  target_link_libraries(pl0c PRIVATE pl0::pl0)

  add_executable(pl0run tools/pl0run.cpp)
  target_link_libraries(pl0run PRIVATE pl0::pl0)

  add_executable(pl0dis tools/pl0dis.cpp)
  target_link_libraries(pl0dis PRIVATE pl0::pl0)
endif()

if(PL0_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(PL0_BUILD_GUI)
  find_package(Qt6 COMPONENTS Widgets QUIET)
  set(_qt_target "")
  if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    qt_add_executable(pl0-gui
      gui/main.cpp
      gui/CodeEditor.cpp
      gui/MainWindow.cpp
      gui/MainWindow.hpp
      gui/CodeEditor.hpp
    )
    target_link_libraries(pl0-gui PRIVATE pl0::pl0 Qt6::Widgets)
    set(_qt_target Qt6::Widgets)
  else()
    find_package(Qt5 COMPONENTS Widgets QUIET)
    if(Qt5_FOUND)
      set(CMAKE_AUTOMOC ON)
      set(CMAKE_AUTORCC ON)
      set(CMAKE_AUTOUIC ON)
      add_executable(pl0-gui
        gui/main.cpp
        gui/CodeEditor.cpp
        gui/MainWindow.cpp
        gui/MainWindow.hpp
        gui/CodeEditor.hpp
      )
      target_link_libraries(pl0-gui PRIVATE pl0::pl0 Qt5::Widgets)
      set(_qt_target Qt5::Widgets)
    endif()
  endif()
endif()
