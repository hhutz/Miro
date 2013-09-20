set(ALL_IDL_FILENAMES
	Payload.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

link_libraries(
	miro
)

add_executable( notify_consumer
	notify_consumer.cpp
	PayloadS.h
	PayloadS.cpp
	PayloadC.h
	PayloadC.cpp
)

add_executable( notify_supplier
	notify_supplier.cpp
	PayloadC.h
	PayloadC.cpp
)

add_executable( basic_consumer
	basic_consumer.cpp
)

add_executable( basic_supplier
	basic_supplier.cpp
)

# Add targets to DartTestfile.txt - [db] 6/4/08
set( TARGETS
	notify_consumer
	notify_supplier
	basic_consumer
	basic_supplier
	)
foreach( TARGET ${TARGETS})
	add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS})
# End of DartTestfile.txt code

install_targets(${EXAMPLES_BIN_DIR}
	basic_consumer
	basic_supplier
	notify_consumer
	notify_supplier
)
