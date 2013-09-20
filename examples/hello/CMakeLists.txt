set(ALL_IDL_FILENAMES
	HelloWorld.idl
)

tao_wrap_idl( ${ALL_IDL_FILENAMES} )

add_library(hello_world STATIC
  ${TAO_IDL_GENERATED}
  HelloWorldImpl.h
  HelloWorldImpl.cpp
)

link_libraries(
	miro
  hello_world
)

set( HELLO_TARGETS
  hello_world_client
  hello_world_server
  hello_world_colocated
)

foreach( HELLO_TARGET ${HELLO_TARGETS} )
	add_executable( ${HELLO_TARGET} 
		${HELLO_TARGET}.cpp
	)
endforeach( HELLO_TARGET ${HELLO_TARGETS} )

install_targets(${EXAMPLES_BIN_DIR}
  ${HELLO_TARGETS}
)
