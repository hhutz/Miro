include( CMakeLists.miroBase.txt )
include( CMakeLists.miroCore.txt )
include( CMakeLists.miroXml.txt )

if ( MIRO_BUILD_WITH_JSON )
  include( CMakeLists.miroJson.txt )
  install_library( ${JSON_LIB_NAME} )
endif ( MIRO_BUILD_WITH_JSON )

if ( TAO_FOUND )
  include( CMakeLists.miro.txt )
  install_library( ${LIB_NAME} )
endif ( TAO_FOUND )

# install library and header targets
#---------------------------------------
install_library( ${BASE_LIB_NAME} )
install_library( ${CORE_LIB_NAME} )
install_library( ${XML_LIB_NAME}  )

## the glob/regular expression is evaluated when cmake runs
## so it doesn't catch generated files
install_files  ( /include/miro FILES ${HEADERS} ${CORE_HEADERS} ${XML_HEADERS} ${JSON_HEADERS} ${BASE_HEADERS} )
install_files  ( /include/miro FILES ${TAO_IDL_GENERATED_HEADERS})
install_files  ( /idl/miro     FILES ${ALL_IDL_FILENAMES} )

## I guess we need to install "*.i" files as well...
install_files  ( /include/miro .*\\.i$ )
