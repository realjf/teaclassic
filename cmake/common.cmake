set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(DEPS_SRC_FILES "")
set(DEPS_HEADER_FILES "")

if(WIN32)
    set(PYTHON_EXECUTABLE "python")
else()
    set(PYTHON_EXECUTABLE "python3")
endif()

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
