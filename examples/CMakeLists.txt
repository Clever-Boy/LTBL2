include_directories(.)

set(normals_src normals.cpp)
add_executable(normals ${normals_src})
target_link_libraries(normals ltbl ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

set(basic_src basic.cpp)
add_executable(basic ${basic_src})
target_link_libraries(basic ltbl ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

set(stressShapes_src stressShapes.cpp)
add_executable(stressShapes ${stressShapes_src})
target_link_libraries(stressShapes ltbl ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

set(stressPointLights_src stressPointLights.cpp)
add_executable(stressPointLights ${stressPointLights_src})
target_link_libraries(stressPointLights ltbl ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})

