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

macro( irg_package_find PACKAGE PACKAGE_DIR BASE_LIB )

message(STATUS "Looking for ${PACKAGE}...")

#
# set up some variables
#--------------------------------------------------
string( TOUPPER "${PACKAGE}"  PACKAGE_UPPER )

set(PACKAGE_FOUND        "${PACKAGE_UPPER}_FOUND"       )
set(PACKAGE_ROOT_DIR     "${PACKAGE_UPPER}_ROOT_DIR"    )
set(PACKAGE_ENV_VAR      "${PACKAGE_UPPER}_ROOT"        )
set(PACKAGE_BASE_LIBRARY "${PACKAGE_UPPER}_BASE_LIBRARY")
set(PACKAGE_INCLUDE_DIR  "${PACKAGE_UPPER}_INCLUDE_DIR" )
set(PACKAGE_LIBRARY_DIR  "${PACKAGE_UPPER}_LIBRARY_DIR" )
set(PACKAGE_LIBRARIES    "${PACKAGE_UPPER}_LIBRARIES"   )

get_package_lib_search_path( "${PACKAGE}" "${PACKAGE_DIR}" "${PACKAGE_ROOT_DIR}" "${PACKAGE_ENV_VAR}" )
#message(STATUS "  (dbg) BASE_LIB=${BASE_LIB}")
#message(STATUS "  (dbg) LIB_SEARCH_PATH=${LIB_SEARCH_PATH}")

# Find the base library
#-----------------------------------------
find_library( ${PACKAGE_BASE_LIBRARY} 
  NAMES ${BASE_LIB}
  PATHS ${LIB_SEARCH_PATH}
  NO_DEFAULT_PATH
  DOC "${PACKAGE} library"
)


if( ${PACKAGE_BASE_LIBRARY} )

  # remove lib name and lib dir to get root
  #-----------------------------------------------
  string(REGEX REPLACE "/[^/]*/[^/]*$" "" _${PACKAGE_ROOT_DIR} ${${PACKAGE_BASE_LIBRARY}} )
  get_filename_component(${PACKAGE_ROOT_DIR} ${_${PACKAGE_ROOT_DIR}} REALPATH)

  message(STATUS "  ${PACKAGE} found in ${${PACKAGE_ROOT_DIR}}")
  set( ${PACKAGE_FOUND} TRUE )
  
  set( ${PACKAGE_LIBRARY_DIR} "${${PACKAGE_ROOT_DIR}}/lib" )
  set( ${PACKAGE_INCLUDE_DIR} "${${PACKAGE_ROOT_DIR}}/include" CACHE PATH "${PACKAGE} include path")
  
#  ## ADDITIONAL LIBRARIES 
#  ## 
#  #####################################################
#  
#  find_library( CSPICE_SUPPORT_LIBRARY csupport  ${${PACKAGE_LIBRARY_DIR}} )
#
#  set( ${PACKAGE_UPPER}_LIBRARY  
#    ${${PACKAGE_BASE_LIBRARY}}
#    ${CSPICE_SUPPORT_LIBRARY}
#    CACHE STRING "all ${PACKAGE} link libraries"
#  )
#  
#  mark_as_advanced( ${PACKAGE_BASE_LIBRARY} CSPICE_SUPPORT_LIBRARY )
  
  
else( ${PACKAGE_BASE_LIBRARY} )
  set( ${PACKAGE_FOUND} FALSE )
  message(STATUS ${LIB_SEARCH_ERROR_MESSAGE})
endif( ${PACKAGE_BASE_LIBRARY} )


endmacro( irg_package_find )
