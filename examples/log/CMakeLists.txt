link_libraries(
	miroCore
)

set( TARGETS
  log_output
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp
	)
endforeach( TARGET ${TARGETS} )

install_targets(${EXAMPLES_BIN_DIR}
  ${TARGETS}
)
