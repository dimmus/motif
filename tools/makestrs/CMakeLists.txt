add_executable(makestrs makestrs.c)

target_include_directories(makestrs PRIVATE ${X_INCLUDE_DIRS})
target_link_libraries(makestrs ${X_LIBRARIES})
