######################################################################
# Find script for jsoncpp
#
# Output Variables:
# -----------------
# JSONCPP_FOUND           : TRUE if search succeded
# JSONCPP_INCLUDE_DIR     : include path
# JSONCPP_LIBRARIES       : all libraries in one variable (use this in your CMakeLists)
# JSONCPP_LIBRARY_DIR     : library path
#
######################################################################

include( IrgPackageFind )
include( GetLibraryList )

set(     PACKAGE jsoncpp )
set( PACKAGE_DIR jsoncpp )
set(    BASE_LIB json )

irg_package_find( "${PACKAGE}" "${PACKAGE_DIR}" "${BASE_LIB}" )

##
## If additional libraries need to be found, do
## so here
################################################
if( ${PACKAGE_FOUND} )

  # json-c uses the same library name as jsoncpp and the same
  # include dir. Do a simple check for a unique jsoncpp header
  if( EXISTS ${${PACKAGE_INCLUDE_DIR}}/json/value.h ) 
  
    add_definitions( -DJSONCPP_FOUND )
    set( LIBRARY_NAMES 
      json
    )
    get_library_list(JSONCPP ${JSONCPP_LIBRARY_DIR} "d" "${LIBRARY_NAMES}")
    
  else( EXISTS ${${PACKAGE_INCLUDE_DIR}}/json/value.h ) 
  
    message( STATUS "  ** No, jsoncpp was NOT found (json-c was found, but that's not what we want)" )
    message( STATUS "  ** JSONCPP_FOUND is FALSE" )
    set( ${PACKAGE_FOUND} FALSE )
    unset( ${PACKAGE_LIBRARY_DIR} )
    unset( ${PACKAGE_INCLUDE_DIR} CACHE )
    unset( ${PACKAGE_BASE_LIBRARY} CACHE )
    
  endif( EXISTS ${${PACKAGE_INCLUDE_DIR}}/json/value.h ) 
  
endif( ${PACKAGE_FOUND} )
