cmake_minimum_required(VERSION 2.8.3)
project(Miro)

if (${CATKIN_TOPLEVEL})
  find_package(catkin  REQUIRED COMPONENTS
    irg_cmake
)
endif(${CATKIN_TOPLEVEL})

if (catkin_FOUND)
###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS src
  LIBRARIES miroCore miroBase miroParams miroXml
  )
else (catkin_FOUND)
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
endif(catkin_FOUND)

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

set( MIRO_SOVERSION_MAJOR    3 ) # epoch
set( MIRO_SOVERSION_MINOR    2 ) # revision
set( MIRO_SOVERSION_PATCH    0 ) # age
set( MIRO_SOVERSION         "${MIRO_SOVERSION_MAJOR}.${MIRO_SOVERSION_MINOR}.${MIRO_SOVERSION_PATCH}" ) 

## this macro is normally pulled in through the
## FindMiro script
##-----------------------------------------------
include( GenerateMiroMakeParams )
# we need to tell the macro where to find the executable
# because it isn't "installed" yet...
if (${CATKIN_TOPLEVEL})
  set( MIRO_MAKEPARAMS_EXECUTABLE ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION}/MakeParams )
else (${CATKIN_TOPLEVEL})
set( MIRO_MAKEPARAMS_EXECUTABLE ${PROJECT_BINARY_DIR}/src/makeParams/${CMAKE_CFG_INTDIR}/MakeParams )
endif (${CATKIN_TOPLEVEL})


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
if (NOT ${CATKIN_TOPLEVEL})
include( CMakePack.txt )
endif (NOT ${CATKIN_TOPLEVEL})

#------------------------------------
print_build_switches()

message(STATUS "Miro version is ${MIRO_SOVERSION}")
