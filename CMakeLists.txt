project(Miro)

## target-specific defines in src/miro require version 2.6
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0003 NEW)  

## set up some defaults
set( CMAKE_MODULE_PATH  ${PROJECT_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})

include( SetArchitecture )
include( SetupBuildSwitch )
include( SetupConfigureFile )
include( SetupInstall )
include( SetupRPATH )
include( SetupUtilities )
include( UserOptions )
include( SetupWarnLevel )

## we have to play some tricks to get install
## path to "stick" in Windows
##------------------------------------------------
set( MIRO_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../${ARCHITECTURE}" CACHE PATH "Miro install path")
set( CMAKE_INSTALL_PREFIX ${MIRO_INSTALL_PREFIX} CACHE INTERNAL "" )

add_build_var( WITH_TAO          BUILD_DEFAULT_TRUE )
add_build_var( WITH_JSON         BUILD_DEFAULT_TRUE )

find_package( ACE )
find_package( Qt4 )

find_package_if( TAO     MIRO_BUILD_WITH_TAO  )
find_package_if( JsonCpp MIRO_BUILD_WITH_JSON )

build_with_var( WITH_TAO          TAO_FOUND )
build_with_var( WITH_JSON         JSONCPP_FOUND )

## if we don't have the following components, 
## we cannot continue
##-----------------------------------------------
minimum_requirements( ACE_FOUND QT4_FOUND QT_QT3SUPPORT_FOUND )

## this macro is normally pulled in through the
## FindMiro script
##-----------------------------------------------
include( GenerateMiroMakeParams )
# we need to tell the macro where to find the executable
# because it isn't "installed" yet...
set( MIRO_MAKEPARAMS_EXECUTABLE ${PROJECT_BINARY_DIR}/src/makeParams/${CMAKE_CFG_INTDIR}/MakeParams )


## add in and out of source dirs to include path
##-----------------------------------------------
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}/src
)

## Set up RPATH and *Config.h
## Call these after we've made all our 
## find_package calls
##--------------------------------------------
setup_rpath()
setup_configure_file()

# everything needs to link to TAO, so set it
# up at the top level for convinience
##-----------------------------------------------
include_directories(
  ${ACE_INCLUDE_DIR}
  ${TAO_INCLUDE_DIRS}
)

# everything needs to link to Qt, so set it
# up at the top level for convinience
##-----------------------------------------------
include_directories(
  ${QT_INCLUDE_DIR}
  ${QT_QT_INCLUDE_DIR}
  ${QT_QTXML_INCLUDE_DIR}
  ${QT_QTCORE_INCLUDE_DIR}
)

# always build these
#----------------------
add_subdirectory( src )
add_build_switch( utils    BUILD_DEFAULT_TRUE )
add_build_switch( examples BUILD_DEFAULT_TRUE )
add_build_switch( tests    BUILD_DEFAULT_TRUE )

# install the export file
#------------------------------------
install_export()

# package script
#------------------------------------
include( CMakePack.txt )

#------------------------------------
print_build_switches()
