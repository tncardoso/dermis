include_directories(${PROJECT_SOURCE_DIR}/vendor/lua/src)

#add_library(dermis MODULE main.c $<TARGET_OBJECTS:liblua>)
add_library(dermis SHARED main.c)
#target_link_libraries(dermis $<TARGET_OBJECTS:liblua> dl)
#target_link_libraries(dermis liblua dl)
target_link_libraries(dermis liblua dl)
