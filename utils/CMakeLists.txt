set( UTILS_BIN_DIR /bin )



link_libraries(
  ${QT_QTGUI_LIBRARY}
  ${QT_QT3SUPPORT_LIBRARY} 
  # Qt3Support also requires the following libs...
  ${QT_QTSQL_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
)

include_directories(
  ${PROJECT_SOURCE_DIR}/utils
  ${QT_QT3SUPPORT_INCLUDE_DIR}
)

add_definitions( -DQT3_SUPPORT ) 

add_subdirectory( miroWidgets )
add_subdirectory( ConfigEditor )

if ( TAO_FOUND )
  add_subdirectory( LogPlayer )
endif ( TAO_FOUND )

