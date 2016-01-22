include_directories(.)

set(basic_src basic.cpp)
add_executable(basic ${basic_src})
target_link_libraries(basic ltbl ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

