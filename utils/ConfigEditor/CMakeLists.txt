set( EXEC ConfigEditor )

set( SOURCES
	ConfigDocumentXML.cpp
	ConfigEditor.cpp
	MainWindow.cpp
	ParameterInstance.cpp
	ParameterSingleton.cpp
	Section.cpp
)

set( HEADERS 
	ConfigDocumentXML.h
	MainWindow.h
	ParameterInstance.h
	ParameterSingleton.h
	Section.h
)

set( MOC_HEADERS
	ConfigDocumentXML.h
	MainWindow.h
	ParameterInstance.h
	ParameterSingleton.h
	Section.h
)

qt_wrap_cpp(${EXEC} MOC_FILES ${MOC_HEADERS})

link_libraries(
	miroBase
	miroCore 
	miroXml 
	miroWidgets 
	miroParams 
)

add_executable( ${EXEC}
	${SOURCES}
	${HEADERS}
	${MOC_FILES}
)

add_dependencies( ${EXEC} miroParams miroWidgets )

install_targets( ${UTILS_BIN_DIR}
	${EXEC}
)
