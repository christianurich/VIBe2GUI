cmake_minimum_required(VERSION 2.6)
project(vibe2gui)

set(CMAKE_MODULE_PATH 
  "${vibe2gui_SOURCE_DIR}/CMake"
  "${vibe2gui_SOURCE_DIR}/CMake/Utils" 
  "${vibe2gui_SOURCE_DIR}/CMake/Packages"
)
# Find VIBE2Core
find_package(vibe2core)
# Find QGLViewer
find_package(QGLViewer)



set(Boost_USE_STATIC_LIBS OFF)

find_package(Boost 1.38.0 COMPONENTS python program_options date_time REQUIRED)
#set(BOOST_ROOT "C:/Program Files (x86)/boost/boost_1_38")
find_package(PythonLibs REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui QtXml QtOpenGL REQUIRED)


find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
set(QT_USE_QTWEBKIT 1)
set(QT_USE_QTXML 1)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTSVG 1)
set(QT_USE_QTNETWORK 1)
include(${QT_USE_FILE})


include_directories(src/core ${Boost_INCLUDE_DIRS} ${QT_QTCORE_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})
set(LIBRARY_OUTPUT_PATH ${vibe2gui_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${vibe2gui_BINARY_DIR})

add_subdirectory(src/vibegui)
add_subdirectory(src/vibe2)




