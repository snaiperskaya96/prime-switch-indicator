project(PrimeSwitchIndicator)
cmake_minimum_required(VERSION 3.0)

set(TARGET_NAME prime-switch-indicator)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(APPINDICATOR REQUIRED appindicator3-0.1)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS} ${APPINDICATOR_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${APPINDICATOR_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(${TARGET_NAME} prime_switch_indicator.cpp)
add_executable(${TARGET_NAME}-select prime_switch_indicator_select.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE ${GTK3_LIBRARIES} ${APPINDICATOR_LIBRARIES})

if(UNIX)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME} DESTINATION /usr/bin/ PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)# PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}-select DESTINATION /usr/bin/ PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE SETUID)
    install(FILES icons/indicator.png DESTINATION /usr/share/icons/${TARGET_NAME})
    install(FILES services/prime-switch-indicator.service DESTINATION /etc/systemd/user)
endif()