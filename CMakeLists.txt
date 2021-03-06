project(qarv)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "-fPIC")
#set(CMAKE_AUTOMOC ON) # Qt5
#set(CMAKE_INCLUDE_CURRENT_DIR ON) # Qt5

# Only set the version in release tarballs.
#set(qarv_VERSION 1.0.99)
set(qarv_ABI 2)
set(qarv_API 2)

if (NOT DEFINED qarv_VERSION)
  message("Program version not specified, using git description.")
  execute_process(COMMAND "git" "show" "-s" "--format='%h'"
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  OUTPUT_VARIABLE qarv_GIT_VERSION
                  RESULT_VARIABLE qarv_GIT_RESULT)
  if (NOT ${qarv_GIT_RESULT} EQUAL 0)
    message("Not building from git, defaulting to version 99.99.99.")
    set(qarv_VERSION "99.99.99")
  else()
    string(REPLACE "qarv-" ""
           qarv_VERSION
           ${qarv_GIT_VERSION})
    string (STRIP ${qarv_VERSION} qarv_VERSION)
  endif()
endif()
message("QArv version: ${qarv_VERSION}")

macro(set_prefixed varname string)
  set(${varname})
  foreach(name ${ARGN})
    set( ${varname} ${${varname}} ${string}${name} )
  endforeach(name)
endmacro(set_prefixed)

find_package(Qt5Core REQUIRED) # Qt5
find_package(Qt5Gui REQUIRED) # Qt5
find_package(Qt5Widgets REQUIRED) # Qt5
find_package(Qt5OpenGL REQUIRED) # Qt5
find_package(Qt5Network REQUIRED) # Qt5
find_package(Qt5Svg REQUIRED) # Qt5

#find_package(Qt4 REQUIRED
#  QtCore
#  QtGui
#  QtOpenGL
#  QtNetwork
#  QtSVG
#)

include(FindPkgConfig)
pkg_search_module(ARAVIS REQUIRED aravis-0.6 aravis-0.4 aravis-0.2)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(SWSCALE REQUIRED libswscale)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)
find_package( OpenCV REQUIRED )

# include(${QT_USE_FILE}) # remove for Qt5

include_directories(
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Declarative_INCLUDE_DIRS}
  ${Qt5OpenGL_INCLUDE_DIRS}
  ${Qt5Svg_INCLUDE_DIRS}
  ${Qt5Network_INCLUDE_DIRS} 
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${ARAVIS_INCLUDE_DIRS}
  ${GIO_INCLUDE_DIRS}
  ${AVUTIL_INCLUDE_DIRS}
)

set(qarv_DATA share/qarv/${qarv_ABI}/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fvisibility-inlines-hidden -fvisibility=hidden")
string(TOLOWER "${CMAKE_BUILD_TYPE}" qarv_BTYPE)
if (NOT qarv_BTYPE MATCHES "debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
endif()
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wno-unused-parameter")
add_definitions(-DQARV_DATA=\"${CMAKE_INSTALL_PREFIX}/${qarv_DATA}\" -DQT_STATICPLUGIN
  -DQARV_VERSION=\"${qarv_VERSION}\" -DQARV_API=\"${qarv_API}\" -DQARV_ABI=\"${qarv_ABI}\")
add_definitions(${Qt5Widgets_DEFINITIONS}) # Qt5 

set(CMAKE_REQUIRED_LIBRARIES ${AVUTIL_LDFLAGS})
set(CMAKE_REQUIRED_INCLUDES ${AVUTIL_INCLUDE_DIRS})

# check_symbol_exists(av_pix_fmt_desc_get libavutil/pixdesc.h HAVE_FMT_DESC_GET) # remove for Qt5
#if (HAVE_FMT_DESC_GET)
  add_definitions(-DHAVE_FMT_DESC_GET)
#endif()
if (ARAVIS_VERSION VERSION_LESS "0.3.5")
  add_definitions(-DARAVIS_OLD_BUFFER)
endif()

set_prefixed(qarv_UIS_pre src/
  mainwindow.ui
)
qt5_wrap_ui(qarv_UIS ${qarv_UIS_pre})
#set_prefixed(qarv_RCS_pre res/ icons/icons.qrc)
qt5_add_resources(qarv_RCS ${qarv_RCS_pre})
set_prefixed(qarv_MOCS src/
  glvideowidget.h
  glhistogramwidget.h
  qarvmainwindow.h
  roicombobox.h
  globals.h
  workthread.h
  api/qarvtype.h
  api/qarvgui.h
  api/qarvcamera.h
  api/qarvcameradelegate.h
  filters/filter.h
  filters/levels.h
)
qt5_wrap_cpp(qarv_MOCD ${qarv_MOCS})
set_prefixed(qarv_SRCS src/
  qarvmainwindow.cpp
  main.cpp
  glvideowidget.cpp
  glhistogramwidget.cpp
  globals.cpp
  roicombobox.cpp
  qarvfeaturetree.cpp
  workthread.cpp
  api/qarvtype.cpp
  recorders/recorder.cpp
  filters/filter.cpp
  api/qarvgui.cpp
  api/qarvcamera.cpp
  api/qarvdecoder.cpp
  api/qarvcameradelegate.cpp
  api/qarvrecordedvideo.cpp
)
set_prefixed(qarv_decoders_MOCS src/decoders/
  mono12packed.h
  mono/mono8.h
  mono/mono8signed.h
  mono/mono10.h
  mono/mono12.h
  mono/mono14.h
  mono/mono16.h
  bayer/bayerbg8.h
  bayer/bayerbg10.h
  bayer/bayerbg12.h
  bayer/bayerbg12_packed.h
  bayer/bayerbg16.h
  bayer/bayergb8.h
  bayer/bayergb10.h
  bayer/bayergb12.h
  bayer/bayergb12_packed.h
  bayer/bayergb16.h
  bayer/bayergr8.h
  bayer/bayergr10.h
  bayer/bayergr12.h
  bayer/bayergr12_packed.h
  bayer/bayergr16.h
  bayer/bayerrg8.h
  bayer/bayerrg10.h
  bayer/bayerrg12.h
  bayer/bayerrg12_packed.h
  bayer/bayerrg16.h
)
#set_prefixed(qarv_decoders_MOCS src/decoders/
#  mono12packed.h
#  monounpackeddecoders.h
#  bayer.h
#)
qt5_wrap_cpp(qarv_decoders_MOCD ${qarv_decoders_MOCS})
set_prefixed(qarv_decoders_SRC src/decoders/
  graymap.cpp
  mono12packed.cpp
  swscaledecoder.cpp
)

#  mono/mono8.cpp
#  mono/mono8signed.cpp
#  mono/mono10.cpp
#  mono/mono12.cpp
#  mono/mono14.cpp
#  mono/mono16.cpp
#  bayer/bayerbg8.cpp
#  bayer/bayerbg10.cpp
#  bayer/bayerbg12.cpp
#  bayer/bayerbg12_packed.cpp
#  bayer/bayerbg16.cpp
#  bayer/bayergb8.cpp
#  bayer/bayergb10.cpp
#  bayer/bayergb12.cpp
#  bayer/bayergb12_packed.cpp
#  bayer/bayergb16.cpp
#  bayer/bayergr8.cpp
#  bayer/bayergr10.cpp
#  bayer/bayergr12.cpp
#  bayer/bayergr12_packed.cpp
#  bayer/bayergr16.cpp
#  bayer/bayerrg8.cpp
#  bayer/bayerrg10.cpp
#  bayer/bayerrg12.cpp
#  bayer/bayerrg12_packed.cpp
#  bayer/bayerrg16.cpp

#set_prefixed(qarv_decoders_SRC src/decoders/
#  graymap.cpp
#  mono12packed.cpp
#  monounpackeddecoders.cpp
#  swscaledecoder.cpp
#  bayer.cpp
#)
set_prefixed(qarv_recorders_MOCS src/recorders/
  rawrecorders/rawdecoded16.h
  rawrecorders/rawdecoded8.h
  rawrecorders/rawundecoded.h
  gstrecorders/huffyuvavi.h
  gstrecorders/x264mkv.h
  imagerecorders/imagerecorder.h
)
#set_prefixed(qarv_recorders_MOCS src/recorders/
#  rawrecorders.h
#  gstrecorders.h
#  imagerecorder.h
#)
qt5_wrap_cpp(qarv_recorders_MOCD ${qarv_recorders_MOCS})
set_prefixed(qarv_recorders_SRC src/recorders/
  rawrecorders/rawdecoded16.cpp
  rawrecorders/rawdecoded8.cpp
  rawrecorders/rawundecoded.cpp
  gstrecorders/huffyuvavi.cpp
  gstrecorders/x264mkv.cpp
  gstrecorders/gstrecorder_implementation.cpp
  imagerecorders/imagerecorder.cpp
)
#set_prefixed(qarv_recorders_SRC src/recorders/
#  rawrecorders.cpp
#  gstrecorders.cpp
#  gstrecorder_implementation.cpp
#  imagerecorder.cpp
#)
set_prefixed(qarv_filters_MOCS src/filters/
  levels.h
  filter.h
)
qt5_wrap_cpp(qarv_filters_MOCD ${qarv_filters_MOCS})
set_prefixed(qarv_filters_SRC src/filters/
  levels.cpp
  filter.cpp
)
set_prefixed(qarv_filters_UIS_pre src/filters/
  levels.ui
)
qt5_wrap_ui(qarv_filters_UIS ${qarv_filters_UIS_pre})
set_prefixed(qarv_TRANS_pre i18n/qarv_ sl.ts)
# qt5_add_translation(qarv_TRANS ${qarv_TRANS_pre}) # remove for Qt5

set(libqarv "qarv-${qarv_API}")
add_library(${libqarv} SHARED
  ${qarv_filters_MOCD}
  ${qarv_filters_SRC}
  ${qarv_filters_UIS}
  ${qarv_recorders_MOCD}
  ${qarv_recorders_SRC}
  ${qarv_decoders_SRC}
  ${qarv_decoders_MOCD}
  ${qarv_SRCS}
  ${qarv_MOCD}
  ${qarv_UIS}
  ${qarv_RCS}
  ${qarv_TRANS}
)
target_link_libraries(${libqarv}
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Declarative_LIBRARIES}
  ${Qt5OpenGL_LIBRARIES}
  ${Qt5Svg_LIBRARIES}
  ${Qt5Network_LIBRARIES} 
  ${ARAVIS_LDFLAGS}
  ${GIO_LDFLAGS}
  ${SWSCALE_LDFLAGS}
  ${AVCODEC_LDFLAGS}
  ${AVUTIL_LDFLAGS}
  ${OpenCV_LIBS}
)
set_target_properties(${libqarv} PROPERTIES SOVERSION ${qarv_ABI})

add_executable(qarvexe src/main.cpp)
target_link_libraries(qarvexe 
  ${Qt5Widgets_LIBRARIES} 
  ${Qt5OpenGL_LIBRARIES} 
  ${Qt5Svg_LIBRARIES} 
  ${Qt5Network_LIBRARIES} 
  ${libqarv}
)
set_target_properties(qarvexe PROPERTIES OUTPUT_NAME qarv)

qt5_wrap_ui(qarv_videoplayer_UI src/utils/qarv_videoplayer.ui)
qt5_wrap_cpp(qarv_videoplayer_MOC
  src/utils/qarv_videoplayer.h
  src/glvideowidget.h
  src/globals.h
)
add_executable(qarv_videoplayer
  src/utils/qarv_videoplayer.cpp
  src/glvideowidget.cpp
  src/recorders/gstrecorders/gstrecorder_implementation.cpp
  src/globals.cpp
  ${qarv_videoplayer_MOC}
  ${qarv_videoplayer_UI}
)
target_link_libraries(qarv_videoplayer 
  ${Qt5Widgets_LIBRARIES} 
  ${Qt5OpenGL_LIBRARIES} 
  ${Qt5Svg_LIBRARIES} 
  ${Qt5Network_LIBRARIES} 
  ${libqarv}
)

#set_prefixed(qarv_ICONS res/icons/
#  document-open.svgz
#  document-save.svgz
#  edit-clear.svgz
#  media-playback-start.svgz
#  media-record.svgz
#  video-display.svgz
#  view-refresh.svgz
#  zoom-original.svgz
#  office-chart-bar.svgz
#  view-object-histogram-linear.svgz
#  view-object-histogram-logarithmic.svgz
#  edit-clear-locationbar-rtl.svgz
#  edit-select.svgz
#  media-playback-stop.svgz
#  media-playback-pause.svgz
#  dialog-information.svgz
#  go-first.svgz
#  go-last.svgz
#)

install(TARGETS ${libqarv} qarvexe qarv_videoplayer
        RUNTIME DESTINATION bin/
        LIBRARY DESTINATION lib/)
set_prefixed(qarv_IHDR src/api/
             qarvcamera.h
             qarvcameradelegate.h
             qarvgui.h
             qarvdecoder.h
             qarvrecordedvideo.h
             qarvtype.h
)
install(FILES ${qarv_IHDR}
        DESTINATION include/qarv-${qarv_API}/)
install(FILES ${qarv_TRANS} ${qarv_ICONS}
        DESTINATION ${qarv_DATA})
#install(FILES res/icons/qarv.svgz
#        RENAME application-x-qarv.svgz
#        DESTINATION share/icons/)
install(FILES res/qarv.desktop res/qarv-player.desktop
        DESTINATION share/applications/)
install(FILES res/qarv.xml
        DESTINATION share/mime/packages/)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/qarv.pc.in
	       ${CMAKE_CURRENT_BINARY_DIR}/qarv-${qarv_API}.pc
               @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qarv-${qarv_API}.pc
	DESTINATION lib/pkgconfig/)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
COMMAND echo
COMMAND echo
COMMAND echo "API documentation can be found in ${CMAKE_CURRENT_BINARY_DIR}/apidoc/"
COMMAND echo
)
endif(DOXYGEN_FOUND)
