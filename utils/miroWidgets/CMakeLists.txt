set( LIB_NAME miroWidgets )
set( EXPORT_NAME "${LIB_NAME}_Export" )
set( EXPORT_FILE "${EXPORT_NAME}.h" )
string( TOUPPER  "${LIB_NAME}_BUILD_DLL" EXPORT_DEFINE )

set( SOURCES
    CompoundParameter.cpp
    ConfigFile.cpp
    DeferredParameterEdit.cpp
    DialogXML.cpp
    DocumentView.cpp
    DocumentXML.cpp
    FileListDialog.cpp
    Item.cpp
    ItemXML.cpp
    OkCancelDialog.cpp
    ParameterDialog.cpp
    ParameterEdit.cpp
    ParameterList.cpp
    ParameterListDialog.cpp
    ParameterXML.cpp
    SimpleParameter.cpp
    SimpleParameterEdit.cpp
    SingleParameterDialog.cpp
    Validators.cpp
)

set( HEADERS 
    CompoundParameter.h
    ConfigFile.h
    DeferredParameterEdit.h
    DialogXML.h
    DocumentView.h
    DocumentXML.h
    FileListDialog.h
    Item.h
    ItemXML.h
    OkCancelDialog.h
    ParameterDialog.h
    ParameterEdit.h
    ParameterList.h
    ParameterListDialog.h
    ParameterXML.h
    SimpleParameter.h
    SimpleParameterEdit.h
    SingleParameterDialog.h
    Validators.h
    qt_compatibility.h
)

set( MOC_HEADERS
    CompoundParameter.h
    DeferredParameterEdit.h
    DialogXML.h
    DocumentView.h
    DocumentXML.h
    FileListDialog.h
    Item.h
    ItemXML.h
    OkCancelDialog.h
    ParameterDialog.h
    ParameterEdit.h
    ParameterList.h
    ParameterListDialog.h
    ParameterXML.h
    SimpleParameter.h
    SimpleParameterEdit.h
    SingleParameterDialog.h
    Validators.h
)

qt_wrap_cpp(${LIB_NAME} MOC_FILES ${MOC_HEADERS})

link_libraries( miroBase miroCore miroXml miroParams )


# *_BUILD_DLL is absolutely required on Windows for proper dll export/import
add_definitions( -D${EXPORT_DEFINE} )

# create a library target
add_library( ${LIB_NAME} SHARED
    ${SOURCES}
    ${HEADERS}
    ${MOC_FILES}
)

install_library( ${LIB_NAME} )
install_files  ( /include/${LIB_NAME} .*\\.h$ )


