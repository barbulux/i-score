cmake_minimum_required(VERSION 3.0)
project(iscore_app LANGUAGES CXX)

iscore_common_setup()
enable_testing()
set(APPNAME "i-score")
# TODO strip symbols after deployment build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)
if(WIN32)
set(SRCS ${SRCS} "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/i-score.rc")
endif()

if(APPLE)
  set(ICON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/i-score.icns")
  set_source_files_properties(${ICON_SRC} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

if(DEPLOYMENT_BUILD)
    add_executable(${APPNAME} WIN32 MACOSX_BUNDLE ${SRCS} ${ICON_SRC})
else()
    add_executable(${APPNAME} WIN32 ${SRCS})
endif()


target_link_libraries(${APPNAME} PUBLIC iscore_lib_base)

if(ISCORE_STATIC_PLUGINS)
  iscore_write_static_plugins_header()
  target_link_libraries(${APPNAME} PUBLIC ${ISCORE_PLUGINS_LIST})
endif()

include(StaticApp.cmake)

setup_iscore_common_exe_features(${APPNAME})

install(
  TARGETS ${APPNAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION bin)

include(UnityApp.cmake)
include(IScoreDeployment)
