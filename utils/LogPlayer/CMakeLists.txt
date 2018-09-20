set( EXEC LogPlayer )

set( SOURCES
	ChannelManager.cpp
	EventView.cpp
	FileSet.cpp
	LogFile.cpp
	LogPlayer.cpp
	MainForm.cpp
)

set( HEADERS 
	ChannelManager.h
	EventView.h
	FileSet.h
	LogFile.h
	MainForm.h
)

set( MOC_HEADERS
	LogFile.h
	FileSet.h
	EventView.h
	MainForm.h
)

include_directories(
  ${QT_QT3SUPPORT_INCLUDE_DIR}
)

add_definitions( -DQT3_SUPPORT )

qt_wrap_cpp(${EXEC} MOC_FILES ${MOC_HEADERS})

link_libraries( miro miroParams miroWidgets )
link_libraries(
  ${QT_QTGUI_LIBRARY}
  ${QT_QT3SUPPORT_LIBRARY}
  # Qt3Support also requires the following libs...
  ${QT_QTSQL_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
)

add_executable( ${EXEC}
	${SOURCES}
	${HEADERS}
	${MOC_FILES}
)

add_dependencies( ${EXEC} miroWidgets )

add_executable( time2date 
  time2date.cpp
)

install_targets( ${UTILS_BIN_DIR}
	${EXEC}
  time2date
)
