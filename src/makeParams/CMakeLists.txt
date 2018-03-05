set( SOURCES
  MakeParams.cpp
)

set( HEADERS
  qt_compatibility.h
)

add_executable( MakeParams 
  ${SOURCES}
  ${HEADERS}
)

target_link_libraries( MakeParams
  miroParams_static
)

add_dependencies( MakeParams miroParams_static )

install_targets( /bin MakeParams )

target_link_libraries( MakeParams
  ${QT_QTXML_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${ACE_LIBRARIES}
  ${JSONCPP_LIBRARIES}
)
