# Demo programs
# Individual demo programs that showcase Motif widgets

# Copy pixmaps to build directory for compilation
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pixmaps 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/pixmaps)
endif()

# Find all demo program directories
file(GLOB DEMO_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(DEMO_DIR ${DEMO_DIRS})
    # Skip if it's not a directory or if it's a special directory
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR} AND 
       NOT DEMO_DIR STREQUAL "CMakeFiles" AND
       NOT DEMO_DIR STREQUAL "pixmaps")
        
        # Check if this demo has a C source file
        file(GLOB DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/*.c")
        # Remove duplicate utility files that cause linker errors, but only for Ext18List
        if(DEMO_DIR STREQUAL "Ext18List")
            list(REMOVE_ITEM DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/bxutil-c.c")
        endif()
        # Remove animate.c for animate demo (only build xmanimate)
        if(DEMO_DIR STREQUAL "animate")
            list(REMOVE_ITEM DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/animate.c")
        endif()
        # Handle drag_and_drop demo with multiple programs
        if(DEMO_DIR STREQUAL "drag_and_drop")
            # Build DNDDemo
            set(DNDDEMO_SOURCES 
                "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/DNDDemo.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/DNDDraw.c"
            )
            add_executable(DNDDemo ${DNDDEMO_SOURCES})
            target_link_libraries(DNDDemo Xm Mrm ${X_LIBRARIES} m)
            target_include_directories(DNDDemo PRIVATE
                ${CMAKE_SOURCE_DIR}/lib
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_BINARY_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}
                ${CMAKE_SOURCE_DIR}/demos/lib
                ${X_INCLUDE_DIRS}
            )
            target_compile_options(DNDDemo PRIVATE
                -include ${CMAKE_SOURCE_DIR}/include/motif_system.h
            )
            install(TARGETS DNDDemo RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/${DEMO_DIR})
            
            # Build simpledrop
            set(SIMPLEDROP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/simpledrop.c")
            add_executable(simpledrop ${SIMPLEDROP_SOURCES})
            target_link_libraries(simpledrop Xm Mrm ${X_LIBRARIES} m)
            target_include_directories(simpledrop PRIVATE
                ${CMAKE_SOURCE_DIR}/lib
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_BINARY_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}
                ${CMAKE_SOURCE_DIR}/demos/lib
                ${X_INCLUDE_DIRS}
            )
            target_compile_options(simpledrop PRIVATE
                -include ${CMAKE_SOURCE_DIR}/include/motif_system.h
            )
            install(TARGETS simpledrop RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/${DEMO_DIR})
            
            message(STATUS "Added drag_and_drop demos: DNDDemo, simpledrop")
        elseif(DEMO_SOURCES)
            # Get the demo name (directory name)
            set(DEMO_NAME ${DEMO_DIR})
            
            # Handle special cases where directory name conflicts with existing files
            if(DEMO_NAME STREQUAL "animate")
                set(DEMO_NAME "animate_demo")
            elseif(DEMO_NAME STREQUAL "fileview")
                set(DEMO_NAME "fileview_demo")
            elseif(DEMO_NAME STREQUAL "hellomotif")
                set(DEMO_NAME "hellomotif_demo")
            elseif(DEMO_NAME STREQUAL "hellomotifi18n")
                set(DEMO_NAME "hellomotifi18n_demo")
            elseif(DEMO_NAME STREQUAL "periodic")
                set(DEMO_NAME "periodic_demo")
            endif()
            
            # Create the executable
            add_executable(${DEMO_NAME} ${DEMO_SOURCES})
            
            # Link with Motif libraries and demo libraries
            target_link_libraries(${DEMO_NAME}
                Xm
                Mrm
                ${X_LIBRARIES}
                m
            )
            
            # Link with demo libraries if they exist
            if(TARGET Xmd)
                target_link_libraries(${DEMO_NAME} Xmd)
            endif()
            if(TARGET WsmDemo)
                target_link_libraries(${DEMO_NAME} WsmDemo)
            endif()
            if(TARGET Exm)
                target_link_libraries(${DEMO_NAME} Exm)
            endif()
            if(TARGET ExmMrm)
                target_link_libraries(${DEMO_NAME} ExmMrm)
            endif()
            
            # Set include directories
            target_include_directories(${DEMO_NAME} PRIVATE
                ${CMAKE_SOURCE_DIR}/lib
                ${CMAKE_SOURCE_DIR}/include
                ${CMAKE_BINARY_DIR}/include
                ${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/pixmaps
                ${CMAKE_CURRENT_BINARY_DIR}/pixmaps
                ${CMAKE_SOURCE_DIR}/demos/lib
                ${X_INCLUDE_DIRS}
            )
            
            # Add system includes and force-include motif_system.h
            target_compile_options(${DEMO_NAME} PRIVATE
                -include ${CMAKE_SOURCE_DIR}/include/motif_system.h
            )
            
            # Check for UIL files and process them
            file(GLOB UIL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/*.uil")
            # Remove problematic UIL files
            if(DEMO_DIR STREQUAL "periodic")
                list(REMOVE_ITEM UIL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/periodic_l.uil")
                list(REMOVE_ITEM UIL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/periodic.uil")
            endif()
            if(UIL_FILES)
                # Ensure the output directory exists
                file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DEMO_DIR})
                
                foreach(UIL_FILE ${UIL_FILES})
                    get_filename_component(UIL_NAME ${UIL_FILE} NAME_WE)
                    set(UID_FILE ${CMAKE_CURRENT_BINARY_DIR}/${DEMO_DIR}/${UIL_NAME}.uid)
                    
                    # Create UID file from UIL file using uil compiler
                    add_custom_command(
                        OUTPUT ${UID_FILE}
                        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${DEMO_DIR}
                        COMMAND ${CMAKE_BINARY_DIR}/clients/uil/uil -o ${UID_FILE} ${UIL_FILE} 
                                -I${CMAKE_SOURCE_DIR}/clients/uil 
                                -I${CMAKE_BINARY_DIR}/clients/uil
                        DEPENDS uil ${UIL_FILE}
                        COMMENT "Compiling UIL file ${UIL_FILE}"
                    )
                    
                    # Add UID file as dependency
                    add_custom_target(${DEMO_NAME}_${UIL_NAME}_uid DEPENDS ${UID_FILE})
                    add_dependencies(${DEMO_NAME} ${DEMO_NAME}_${UIL_NAME}_uid)
                endforeach()
            endif()
            
            # Install the demo executable
            install(TARGETS ${DEMO_NAME}
                RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/${DEMO_NAME}
            )
            
            # Install UIL and UID files
            if(UIL_FILES)
                install(FILES ${UIL_FILES}
                    DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/${DEMO_NAME}
                )
                foreach(UIL_FILE ${UIL_FILES})
                    get_filename_component(UIL_NAME ${UIL_FILE} NAME_WE)
                    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DEMO_DIR}/${UIL_NAME}.uid
                        DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/${DEMO_NAME}
                    )
                endforeach()
            endif()
            
            # Install other data files (README, etc.)
            file(GLOB DATA_FILES 
                "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/README*"
                "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/*.txt"
                "${CMAKE_CURRENT_SOURCE_DIR}/${DEMO_DIR}/*.md"
            )
            if(DATA_FILES)
                install(FILES ${DATA_FILES}
                    DESTINATION ${CMAKE_INSTALL_DATADIR}/Xm/${DEMO_NAME}
                )
            endif()
            
            message(STATUS "Added demo: ${DEMO_NAME}")
        endif()
    endif()
endforeach()
