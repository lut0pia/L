add_module(
  font_tools
  CONDITION ${DEV_DBG}
  SOURCES
	${CMAKE_CURRENT_LIST_DIR}/font_tools.cpp
	${CMAKE_CURRENT_LIST_DIR}/FontPacker.cpp
)
