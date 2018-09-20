set( UTILS_BIN_DIR /bin )

include_directories(
  ${PROJECT_SOURCE_DIR}/utils
)

link_libraries(
  ${QT_QTGUI_LIBRARY}
)

add_subdirectory( miroWidgets )
add_subdirectory( ConfigEditor )

if ( TAO_FOUND )
  add_build_switch( LogPlayer MIRO_BUILD_WITH_QT4 )
endif ( TAO_FOUND )

