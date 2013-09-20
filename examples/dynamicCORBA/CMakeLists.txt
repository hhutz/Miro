set(ALL_IDL_FILENAMES
	CORBA_Types.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

add_library(dynamicCORBA STATIC
  ${TAO_IDL_GENERATED}
)

link_libraries(
  dynamicCORBA
	miro
)

set( TARGETS
  dynBasic
  generic_consumer
  parse_any
)

foreach( TARGET ${TARGETS} )
	add_executable( ${TARGET} 
		${TARGET}.cpp
	)
endforeach( TARGET ${TARGETS} )

install_targets(${EXAMPLES_BIN_DIR}
	${TARGETS}
)
