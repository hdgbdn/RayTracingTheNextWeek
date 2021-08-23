cmake_minimum_required (VERSION 3.8)

project ("RayTracingTheNextWeek")

#libs
find_package(OpenGL REQUIRED)
add_subdirectory(3rd/glad)
add_subdirectory(3rd/glm)

# add source files
file(GLOB SRC
    "src/*.cpp"
)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} OpenGL::GL glad ${CMAKE_CURRENT_LIST_DIR}/lib/glfw3.lib glm::glm)
target_include_directories(${PROJECT_NAME} PUBLIC "include")


set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install")
install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION "${PROJECT_NAME}"
            LIBRARY DESTINATION "${PROJECT_NAME}/lib"
            ARCHIVE DESTINATION "${PROJECT_NAME}/lib/static"
        )
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/res"
            DESTINATION "${PROJECT_NAME}")