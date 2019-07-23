cmake_minimum_required(VERSION 2.8.3)
project(Miro)

###################################
## catkin                        ##
###################################
if (CATKIN_DEVEL_PREFIX)
  find_package(catkin  REQUIRED COMPONENTS
    irg_cmake
    qt_gui_cpp
)
endif(CATKIN_DEVEL_PREFIX)

if(NOT catkin_FOUND)
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
endif(NOT catkin_FOUND)

add_build_var( WITH_TAO          BUILD_DEFAULT_TRUE )
add_build_var( WITH_JSON         BUILD_DEFAULT_TRUE )

if("${qt_gui_cpp_USE_QT_MAJOR_VERSION} " STREQUAL "5 ")
  message(STATUS "FOUND QT 5")
  find_package(Qt5Widgets REQUIRED)
  find_package(Qt5Xml REQUIRED)
  if (Qt5_POSITION_INDEPENDENT_CODE)
    SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()
  include_directories(SYSTEM "${Qt5Widgets_INCLUDE_DIRS}")
  include_directories(SYSTEM "${Qt5Xml_INCLUDE_DIRS}")
  set(QT_QTXML_LIBRARY Qt5::Xml)
  set(QT_QTCORE_LIBRARY Qt5::Widgets)
  add_build_var( WITH_QT4         BUILD_DEFAULT_FALSE )
  add_build_var( WITH_QT5         BUILD_DEFAULT_TRUE )
else()
  message(STATUS "FOUND QT 4")
  find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
  include(${QT_USE_FILE})
  include_directories(SYSTEM "${QT_INCLUDE_DIR}")
  include_directories(SYSTEM "${QT_QTXML_INCLUDE_DIR}")
  include_directories(SYSTEM "${QT_QTCORE_INCLUDE_DIR}")
  add_build_var( WITH_QT4    BUILD_DEFAULT_TRUE )
  add_build_var( WITH_QT5    BUILD_DEFAULT_FALSE )
endif()

find_package( ACE )
find_package_if( TAO     MIRO_BUILD_WITH_TAO  )
find_package_if( JsonCpp MIRO_BUILD_WITH_JSON )

build_with_var( WITH_TAO          TAO_FOUND )
build_with_var( WITH_JSON         JSONCPP_FOUND )

## if we don't have the following components, 
## we cannot continue
##-----------------------------------------------
minimum_requirements( ACE_FOUND )

if (catkin_FOUND)
###################################
## catkin specific configuration ##
###################################
  set(MIRO_CATKIN_DEPENDS ACE )
  if(MIRO_BUILD_WITH_TAO)
    set(MIRO_CATKIN_DEPENDS ${MIRO_CATKIN_DEPENDS} TAO)
  endif(MIRO_BUILD_WITH_TAO)
  if(MIRO_BUILD_WITH_JSON)
    set(MIRO_CATKIN_DEPENDS ${MIRO_CATKIN_DEPENDS} JSONCPP)
  endif(MIRO_BUILD_WITH_JSON)

  catkin_package(
    INCLUDE_DIRS src
    LIBRARIES miroCore miroBase miroParams miroXml
    CFG_EXTRAS Miro-extras.cmake
    DEPENDS ${MIRO_CATKIN_DEPENDS}
  )
  ## set variable with full path of extras file so we can append library info
  set_cfg_extras_file()
  
  # add devel/include - this should add to persistent catkin metadata
  list(APPEND ${PROJECT_NAME}_INCLUDE_DIRS ${CATKIN_DEVEL_PREFIX}/include)

endif(catkin_FOUND)

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
if (CATKIN_DEVEL_PREFIX)
  set( MIRO_MAKEPARAMS_EXECUTABLE ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_BIN_DESTINATION}/MakeParams )
else (CATKIN_DEVEL_PREFIX)
set( MIRO_MAKEPARAMS_EXECUTABLE ${PROJECT_BINARY_DIR}/src/makeParams/${CMAKE_CFG_INTDIR}/MakeParams )
endif (CATKIN_DEVEL_PREFIX)


## add in and out of source dirs to include path
##-----------------------------------------------
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${CATKIN_DEVEL_PREFIX}/include
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

# always build these
#----------------------
add_subdirectory( src )
add_subdirectory( utils )
add_build_switch( examples BUILD_DEFAULT_TRUE )
add_build_switch( tests    BUILD_DEFAULT_TRUE )

# install the export file
#------------------------------------
install_export()

# package script
#------------------------------------
if (NOT CATKIN_DEVEL_PREFIX)
include( CMakePack.txt )
endif (NOT CATKIN_DEVEL_PREFIX)

#------------------------------------
print_build_switches()

message(STATUS "Miro version is ${MIRO_SOVERSION}")

###############################################
## We deviate from the standard usage of catkin's CFG_EXTRAS because 
## that must run before targets are added, but we want this to run 
## after targets are added (so we have a list of exported libraries 
## that is consistent with the build switches). 
## I"M PRETTY SURE THIS WILL BREAK WHEN BUILDING INSTALLSPACE
## installation will be tricky. For now, we just acknowledge its
## probably broken and carry on...
if( Miro_CFG_EXTRAS_FILE )
  # copy the develspace extras to the one that will go into installspace
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Miro_CFG_EXTRAS_FILE} ${Miro_CFG_EXTRAS_FILE}-install)

  file( APPEND ${Miro_CFG_EXTRAS_FILE} "\n#-- Executables -----\n" )
  file( APPEND ${Miro_CFG_EXTRAS_FILE} "set( MIRO_MAKEPARAMS_EXECUTABLE ${MIRO_MAKEPARAMS_EXECUTABLE} )\n" )
  
  # install file
  ## there is not a consistent ordering of installation if install is called
  ## at this level. Therefore, we create a cmake/postinstall subdirectory and make the 
  ## call there. The last subdirectory added will have its install commands exectued last. 
  file( APPEND ${Miro_CFG_EXTRAS_FILE}-install "\n#-- Executables -----\n" )
  file( APPEND ${Miro_CFG_EXTRAS_FILE}-install "set( MIRO_MAKEPARAMS_EXECUTABLE ${CMAKE_INSTALL_PREFIX}/bin/MakeParams )\n" )
  add_subdirectory( cmake/postinstall )
    
endif( Miro_CFG_EXTRAS_FILE )
###############################################

