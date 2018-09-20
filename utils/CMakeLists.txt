set( UTILS_BIN_DIR /bin )

include_directories(
  ${PROJECT_SOURCE_DIR}/utils
)

add_subdirectory( miroWidgets )
add_subdirectory( ConfigEditor )

if ( TAO_FOUND )
  add_build_swithc( LogPlayer MIRO_BUILD_WITH_QT4 )
endif ( TAO_FOUND )

