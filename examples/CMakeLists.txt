set( EXAMPLES_BIN_DIR /tests )
set( EXAMPLES_LIB_DIR /lib )

add_subdirectory(log)

if ( TAO_FOUND )
  add_subdirectory(ami)
  add_subdirectory(basic)
  add_subdirectory(dynamicCORBA)
  add_subdirectory(hello)
  add_subdirectory(notify)
endif ( TAO_FOUND )

if( MIRO_BUILD_WITH_JSON )
  add_subdirectory(params)
endif( MIRO_BUILD_WITH_JSON )

