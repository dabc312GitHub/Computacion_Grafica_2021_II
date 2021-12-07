project( final )

message( "Configuring main application - ${PROJECT_NAME} : " )

# OpenGL
find_package(OpenGL REQUIRED)

# shaders reload
# vertex shader
configure_file(shaders/vertexShader.shader ${CMAKE_CURRENT_BINARY_DIR}/shaders/vertexShader.shader)
# fragment shader
configure_file(shaders/fragmentShader.shader ${CMAKE_CURRENT_BINARY_DIR}/shaders/fragmentShader.shader)
# some images used as a texture
configure_file(assets/CuboRubik.png ${CMAKE_CURRENT_BINARY_DIR}/assets/CuboRubik.png COPYONLY)

#configure_file(assets/awesomeface.png ${CMAKE_CURRENT_BINARY_DIR}/assets/awesomeface.png COPYONLY)
#configure_file(assets/wall.jpg ${CMAKE_CURRENT_BINARY_DIR}/assets/wall.jpg COPYONLY)
#configure_file(assets/container.jpg ${CMAKE_CURRENT_BINARY_DIR}/assets/container.jpg COPYONLY)


file(GLOB SOURCES "*.cpp" ${DEPENDENCY_DIR}/include/glad/glad/glad.c )
file(GLOB HEADERS "*.h" )
file(GLOB SHADERS "*.vert" "*.frag" "*.vs" "*.fs" "*.glsl" "*.shader")

include_directories( 
${DEPENDENCY_DIR}/include/glad/ 
${DEPENDENCY_DIR}/include/glfw/
${DEPENDENCY_DIR}/include/freeimage/
${DEPENDENCY_DIR}/include/glew
${DEPENDENCY_DIR}/include/glm
${DEPENDENCY_DIR}/include/freeglut
#${DEPENDENCY_DIR}/include/shaderLibrary/
#${DEPENDENCY_DIR}/include/stbImageLibrary/
)

message( "\nBuild mode test is: ${CMAKE_BUILD_TYPE}" )

if ( CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("\nOpenGL_GLUT_GLFW_Program -- Debug mode ****************")
	link_directories(
			${DEPENDENCY_DIR}/lib/glad/Debug/
			${DEPENDENCY_DIR}/lib/glfw/Debug/
			${DEPENDENCY_DIR}/lib/freeglut/Debug/
			${DEPENDENCY_DIR}/lib/freeimage/Debug/
			${DEPENDENCY_DIR}/lib/glew/Debug/
			${DEPENDENCY_DIR}/lib/glm/Debug/
	)
else()
	message("\nOpenGL_GLUT_GLFW_Program -- Release mode ****************")
	link_directories(
			${DEPENDENCY_DIR}/lib/glad/Release/
			${DEPENDENCY_DIR}/lib/glfw/Release/
			${DEPENDENCY_DIR}/lib/freeglut/Release/
			${DEPENDENCY_DIR}/lib/freeimage/Release/
			${DEPENDENCY_DIR}/lib/glew/Release/
			${DEPENDENCY_DIR}/lib/glm/Release/
	)
endif ()

#add_executable( ${PROJECT_NAME}  ${HEADERS} ${SOURCES} ${SHADERS} ../../dep/include/shaderLibrary/shader.cpp ../../dep/include/shaderLibrary/shader.h ../../dep/include/stbImageLibrary/stb_image.cpp ../../dep/include/stbImageLibrary/stb_image.h )
#add_executable( ${PROJECT_NAME}  ${HEADERS} ${SOURCES} ${SHADERS} )
add_executable(
		${PROJECT_NAME}  ${HEADERS} ${SOURCES} ${SHADERS}
		lib/solver.cpp lib/solver.h
		lib/shader.cpp lib/shader.h
		lib/stb_image.cpp lib/stb_image.h
		lib/camera.h
#		database/CuboUnidad.cpp database/CuboUnidad.hpp
#		database/rubik.hpp
#		database//Solver.cpp database/Solver.hpp
)

if ( CMAKE_BUILD_TYPE STREQUAL "Debug")
	if ( WIN32 )
		message("\nOpenGL_GLUT_GLFW_Program -- Debug mode Windows ****************")
		target_link_libraries( ${PROJECT_NAME}
				opengl32
				freeglut_staticd.lib
				glad.lib
				glfw3.lib
				glew32d.lib
				FreeImaged.lib
				glm_static.lib

				)
	else()
		message("\nOpenGL_GLUT_GLFW_Program -- Debug mode Linux ****************")
		target_link_libraries( ${PROJECT_NAME}
				${OPENGL_LIBRARY}
				-lm
				-lGL
				-lGLU
				-lX11
				-lXi
				-lXxf86vm
				-lXrandr
				-pthread
				-ldl
				-lm
				libglad.a
				libglfw3.a
				libGLEWd.a
				libglut.a
				libfreeimage.a
				libglm_static.a
				)

	endif()
else()
	if ( WIN32 )
		message("\nOpenGL_GLUT_GLFW_Program -- Release mode Windows ****************")
		target_link_libraries( ${PROJECT_NAME}
				opengl32
				freeglut_static.lib
				glad.lib
				glfw3.lib
				glew32.lib
				FreeImage.lib
				glm_static.lib
				)
	else()
		message("\nOpenGL_GLUT_GLFW_Program -- Release mode Linux ****************")
		target_link_libraries( ${PROJECT_NAME}
				${OPENGL_LIBRARY}

				-lm
				-lGL
				-lGLU
				-lX11
				-lXi
				-lXxf86vm
				-lXrandr
				-pthread
				-ldl
				-lm
				libglad.a
				libglfw3.a
				libGLEW.a
				libglut.a
				libfreeimage.a
				libglm_static.a
				)

	endif()
endif ()                      