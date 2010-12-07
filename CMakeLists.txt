# This file serves as an example of how to use cmake with QtGstreamer.
# It can be used for building this example either in the QtGstreamer source tree or standalone.
cmake_minimum_required(VERSION 2.8)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(SRCS main.cpp player.cpp player_window.cpp)
set(HDRS player.h player_window.h)

find_package(QtGstreamer REQUIRED)
find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)

include(${QT_USE_FILE})
include_directories(${QTGSTREAMER_INCLUDES} ${QT_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS} ${QT_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

qt4_wrap_cpp(MOC_SRCS ${HDRS})
add_executable(player ${SRCS} ${MOC_SRCS})

target_link_libraries(player ${QTGSTREAMER_LIBRARIES} ${QT_QTGUI_LIBRARY})
