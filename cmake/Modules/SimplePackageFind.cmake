include(GetPackageLibSearchPath)

# FIXME this macro should be deprecated and replaced with 
#       something better... 
#         * Eliminate the *_BASE_LIBRARY naming
#         * check for at least one header
#         * more flexible library naming (for alternate plaforms, versions, etc

# This is a reasonably generic macro for packages
# located in standard irg places. If the <package>_ROOT_DIR
# is set before calling this scipt, we search for the base library 
# in that path and that path only (i.e. <package>_ROOT_DIR/lib ).
# If <package>_ROOT_DIR is not set, we will search for it
# in the paths definied by the GetPackageLibSearchPath macro. 
#-----------------------------------------------------------
include( GetLibraryList )

macro( simple_package_find PACKAGE_NAME PACKAGE_DIRS PACKAGE_REQ_LIBRARY )

message(STATUS "Looking for ${PACKAGE_NAME}...")

#
# set up some variables
#--------------------------------------------------
string( TOUPPER "${PACKAGE_NAME}"  PACKAGE_UPPER )

set(PACKAGE_FOUND        "${PACKAGE_UPPER}_FOUND"       )
set(PACKAGE_ROOT_DIR     "${PACKAGE_UPPER}_ROOT_DIR"    )
set(PACKAGE_ENV_VAR      "${PACKAGE_UPPER}_ROOT"        )
set(PACKAGE_BASE_LIBRARY "${PACKAGE_UPPER}_BASE_LIBRARY")
set(PACKAGE_INCLUDE_DIR  "${PACKAGE_UPPER}_INCLUDE_DIR" )
set(PACKAGE_LIBRARY_DIR  "${PACKAGE_UPPER}_LIBRARY_DIR" )
set(PACKAGE_LIBRARIES    "${PACKAGE_UPPER}_LIBRARIES"   )

get_package_lib_search_path( "${PACKAGE_NAME}" "${PACKAGE_DIRS}" "${PACKAGE_ROOT_DIR}" "${PACKAGE_ENV_VAR}" )
#message(STATUS "  (dbg) BASE_LIB=${BASE_LIB}")
#message(STATUS "  (dbg) LIB_SEARCH_PATH=${LIB_SEARCH_PATH}")

# Find the base library
#-----------------------------------------
find_library( ${PACKAGE_BASE_LIBRARY} 
  NAMES ${PACKAGE_REQ_LIBRARY}
  PATHS ${LIB_SEARCH_PATH}
  NO_DEFAULT_PATH
  DOC "${PACKAGE_NAME} library"
)


if( ${PACKAGE_BASE_LIBRARY} )

  # remove lib name and lib dir to get root
  #-----------------------------------------------
  string(REGEX REPLACE "/[^/]*/[^/]*$" "" _${PACKAGE_ROOT_DIR} ${${PACKAGE_BASE_LIBRARY}} )
  if( _${PACKAGE_ROOT_DIR} ) # only resolve if non-empty
    get_filename_component(${PACKAGE_ROOT_DIR} "${_${PACKAGE_ROOT_DIR}}" REALPATH)
  else ( _${PACKAGE_ROOT_DIR} )
    set( ${PACKAGE_ROOT_DIR} "/" )
  endif( _${PACKAGE_ROOT_DIR} )
  
  message( STATUS "${PACKAGE_ROOT_DIR}=${${PACKAGE_ROOT_DIR}}" )

  set( ${PACKAGE_LIBRARY_DIR} "${${PACKAGE_ROOT_DIR}}/lib" )
  
  set( ROOT_INCLUDE_DIR "${${PACKAGE_ROOT_DIR}}/include" )
  #set( ${PACKAGE_INCLUDE_DIR} "${${PACKAGE_ROOT_DIR}}/include" CACHE PATH "${PACKAGE_NAME} include path")
  
  if( PACKAGE_REQ_INCLUDE )
    find_file( TEMP_INCLUDE_FILE 
      NAMES ${PACKAGE_REQ_INCLUDE}
      PATHS ${ROOT_INCLUDE_DIR}
      PATH_SUFFIXES "" ${PACKAGE_REQ_INCLUDE_DIRS}
      NO_DEFAULT_PATH
    )
    
    if ( TEMP_INCLUDE_FILE ) 
    
      set( ${PACKAGE_FOUND} TRUE )
      message(STATUS "  ${PACKAGE_NAME} found in ${${PACKAGE_ROOT_DIR}}")
      string(REGEX REPLACE "/[^/]*$" "" FOUND_INCLUDE_DIR ${TEMP_INCLUDE_FILE} )
      #set( ${PACKAGE_INCLUDE_DIR} "${FOUND_INCLUDE_DIR}" CACHE PATH "${PACKAGE_NAME} include path")
      if( FOUND_INCLUDE_DIR STREQUAL ROOT_INCLUDE_DIR )
        set( ${PACKAGE_INCLUDE_DIR} "${ROOT_INCLUDE_DIR}" CACHE PATH "${PACKAGE_NAME} include path")
      else (FOUND_INCLUDE_DIR STREQUAL ROOT_INCLUDE_DIR )
        set( ${PACKAGE_INCLUDE_DIR} "${ROOT_INCLUDE_DIR};${FOUND_INCLUDE_DIR}" CACHE PATH "${PACKAGE_NAME} include path")
      endif(FOUND_INCLUDE_DIR STREQUAL ROOT_INCLUDE_DIR )
      
    else ( TEMP_INCLUDE_FILE ) 
    
      set( ${PACKAGE_FOUND} FALSE )
      message( STATUS "  Could not find required header file: ${PACKAGE_REQ_INCLUDE}")
      set( ${PACKAGE_INCLUDE_DIR} "NOT_FOUND" CACHE PATH "${PACKAGE_NAME} include path")
      
    endif( TEMP_INCLUDE_FILE ) 
    
    unset( TEMP_INCLUDE_FILE CACHE )
    
  else( PACKAGE_REQ_INCLUDE )
  
    set( ${PACKAGE_FOUND} TRUE )
    message(STATUS "  ${PACKAGE_NAME} found in ${${PACKAGE_ROOT_DIR}}")
    set( ${PACKAGE_INCLUDE_DIR} "${ROOT_INCLUDE_DIR}" CACHE PATH "${PACKAGE_NAME} include path")
    
  endif( PACKAGE_REQ_INCLUDE )
  
else( ${PACKAGE_BASE_LIBRARY} )

  set( ${PACKAGE_FOUND} FALSE )
  message(STATUS ${LIB_SEARCH_ERROR_MESSAGE})
  
endif( ${PACKAGE_BASE_LIBRARY} )

unset( ${PACKAGE_BASE_LIBRARY} CACHE )

endmacro( simple_package_find )
