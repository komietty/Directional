cmake_minimum_required(VERSION 3.16)
project(505_Meshing)

add_executable(${PROJECT_NAME}_bin main.cpp)
target_link_libraries(${PROJECT_NAME}_bin PUBLIC igl::core igl::glfw igl::opengl igl_copyleft::cgal tutorials)
### igl_copy_cgal_dll(${PROJECT_NAME}_bin)
