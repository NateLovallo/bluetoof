# CMAKELists

cmake_minimum_required (VERSION 2.8.11)
project (bluetoof)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fpermissive")

find_package (PkgConfig REQUIRED)
pkg_check_modules (GLIB2 REQUIRED glib-2.0)
pkg_check_modules (GIO REQUIRED glib-2.0)

include_directories (${GLIB2_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS} include)
link_directories (${GLIB2_LIBRARY_DIRS} ${GIO_LIBRARY_DIRS})

add_executable (bluetoof bluetoof.cpp)

target_link_libraries (bluetoof LINK_PUBLIC ${GLIB2_LIBRARIES} ${GIO_LIBRARIES} glib-2.0 gio-2.0 m gobject-2.0)
