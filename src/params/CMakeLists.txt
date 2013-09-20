set( LIBNAME miroParams )

set( SOURCES
    Type.cpp
    Parameter.cpp
    Parser.cpp
    Generator.cpp
    TextErrorHandler.cpp
)

set( HEADERS
    Type.h
    Parameter.h
    Parser.h
    Generator.h
    TextErrorHandler.h
    qt_compatibility.h
)

# build both shared and static libraries
# we will link MakeParams against the static lib, 
# and all others will link against the shared.
#------------------------------------------------
add_library( ${LIBNAME}_static STATIC
    ${HEADERS}
    ${SOURCES}
)

# this is a hack to avoid adding the DLL export
# stuff on winows right now. TODO: add exports
#------------------------------------------------
set( SHARED_LIB SHARED )
if( WIN32 )
    set( SHARED_LIB STATIC )
endif( WIN32 )

add_library( ${LIBNAME} ${SHARED_LIB}
    ${HEADERS}
    ${SOURCES}
)

target_link_libraries( ${LIBNAME}
  ${QT_QTXML_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${JSONCPP_LIBRARIES}
)

install_library( ${LIBNAME} )
install_files  ( /include/${LIBNAME} .*\\.h$ )
