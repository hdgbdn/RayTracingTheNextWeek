cmake_minimum_required (VERSION 3.8)

project ("RayTracingTheNextWeek")

# add source files
file(GLOB SRC
    "src/*.cpp"
)
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC "include")
