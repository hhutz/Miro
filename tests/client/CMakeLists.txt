link_libraries(
	miro
)

set( TARGETS
  client_path_test
  multiple_clients_test
  roundtriptimeout
  validateconnection
  client_parameters
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp
	)
	# Add target to DartTestfile.txt - [db] 6/4/08
	add_test(${TARGET} ${CTEST_BIN_PATH}/${TARGET})
endforeach( TARGET ${TARGETS} )

install_targets(${TESTS_BIN_DIR}
  ${TARGETS}
)
