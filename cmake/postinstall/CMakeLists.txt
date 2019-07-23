install( CODE "message( STATUS \"#### Do the install of Miro_CFG_EXTRAS_FILE in postinstall to make sure it's the last thing executed\") ####" )
install( FILES ${Miro_CFG_EXTRAS_FILE}-install DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/cmake RENAME ${PROJECT_NAME}-extras.cmake )
