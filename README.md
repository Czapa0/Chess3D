# Chess3D

## Installing
For the project to work additional packages shall be installed (preferably using vcpkg):
`vcpkg install glfw3 glad[gl-api-41,loader] glm assimp imgui[glfw-binding,opengl3-binding] stb``

## Camera movement
To operate camera WSAD shall be used for up-down and left-right movement. Additionally, SHIFT may be used to move with higher speed.

Right mouse button is responsible for rotation of camera. Additionally, SHIFT may be used to roatate around scene.

When clicking C button, user enters 'no cursor' mode, in which there is no need for right-clicking mouse to rotate.

## Bezier Surface
Surface is located under the main scene (under chessboard). It is rendered only using phong shading, which means that changing shading type for main scene does not apply for Bezier surface.