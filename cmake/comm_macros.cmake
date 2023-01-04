macro(SETUP_GROUPS src_files)
    foreach(FILE ${src_files})
        get_filename_component(PARENT_DIR "${FILE}" PATH)
        set(GROUP "${PARENT_DIR}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endmacro()

macro(SET_OUTPUT_NAMES projname)
    set_target_properties(${projname} PROPERTIES OUTPUT_NAME_DEBUG ${projname}_Debug)
    set_target_properties(${projname} PROPERTIES OUTPUT_NAME_RELEASE ${projname}_Release)
    set_target_properties(${projname} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO ${projname}_ReleaseDebInfo)

    # On Linux/macOS the binaries go to <root>/bin folder
    if(UNIX)
        set_target_properties(${projname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
    endif()
endmacro()

macro(SETUP_APP projname version)
    set(PROJ_NAME ${projname})
    set(PROJ_VER ${version})
    project(${PROJ_NAME} LANGUAGES C CXX VERSION ${PROJ_VER})
    file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.c??)
    file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.h??)

    include_directories(src)
    add_executable(${PROJ_NAME} ${SRC_FILES} ${HEADER_FILES} ${DEPS_HEADER_FILES} ${DEPS_SRC_FILES})

    if(MSVC)
        add_definitions(-D_CONSOLE)
    endif()

    SETUP_GROUPS("${SRC_FILES}")
    SETUP_GROUPS("${HEADER_FILES}")

    SET_OUTPUT_NAMES(${PROJECT_NAME})

    set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD 20)
    set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

    if(MSVC)
        set_property(TARGET ${PROJ_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    endif()
endmacro()
