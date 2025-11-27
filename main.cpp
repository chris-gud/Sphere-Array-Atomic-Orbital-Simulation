/*-------------------------- HYDROGEN ATOM ORBITAL SIMULATOR ---------------------------------------
---------------------------------- By Chris Gudmundsen ---------------------------------------------
-------------------------------------- Dec. 2024 ---------------------------------------------------
 
 This program is meant to calculate and display the probability density functions of an electron in
the hydrogen atom. Once the user enters their desired quantum numbers, a window is created with 3 axes
and several spheres arranged in a cube. The origin of the axis represents the nucleus of the
hydrogen atom. The electron probability density at several points in 3D space is represented by the
radius of the sphere at that location. The maximum radius for each orbital is arbitrary - it has
been manually fitted to be visually appealing and clear (this means that it is not recommended to
draw conclusions from radius size when comparing orbitals stemming from different combinations of
quantum numbers). In addition, the scale of the axes change from orbital to orbital. They have been
manually set to display the most interesting parts of each orbital clearly. The viewer can assume
that the probability density decreases to 0 outside of the axis volume. The scale of the axes for
each orbital is printed to terminal for the viewer's reference.

 How is this simulation different from the rest? Most depictions of the orbitals of the hydrogen
atom depict something called an isosurface, where points of constant probability are bridged to form
a surface. However, this misplaces a lot of information; the viewer cannot visualize the probability
densities within the surface. A naive viewer may be led to believe that the probability density is thus
uniform within the surface. This is not the case. This program is meant to prove it.


CONTROLS:

 W : move forwards into the plane of the screen
 S : move backwards away from the plane of the screen
 A : move left upon the plane of the screen
 D : move right upon the plane of the screen
 CNTRL : down (-y)
 SPACE : up (+y)


----------------------------------- ACKNOWLEDGEMENTS -----------------------------------------------
 
OpenGL single sphere indices and vertices generation from "OpenGL Sphere Tutorial" by Song Ho Ahn
https://www.songho.ca/opengl/gl_sphere.html

Texture, Camera, EBO, VAO, VBO, and shaderClass .cpp and .h (basic OpenGL 3D display and movement)
from Youtube "OpenGL Course - Create 3D and 2D Graphics With C++" by freeCodeCamp.org
(Github VictorGordan/opengl-tutorials)
https://www.youtube.com/watch?v=45MIykWJ-C4
https://github.com/VictorGordan/opengl-tutorials

--------------------------------------------------------------------------------------------------*/

#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "glad.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

const unsigned int WIDTH = 1700;
const unsigned int HEIGHT = 1000;
const float PI = M_PI;

// How many sectors and stacks each sphere has graphically
const int sectorCount = 9;
const int stackCount = 9;
const int NUM_VERTICES_PER_SPHERE = 100;
const int NUM_TRIANGLES_PER_SPHERE = 144;

const int numSpheres = pow(11,3);

//-------------------------------------- DEFAULT QUANTUM NUMBERS ----------------------------------------//
int n = 1; // Principal quantum number
int l = 0; // Angular momentum quantum number
int ml = 0; // Magnetic quantum number
//------------------------------------ END DEFAULT QUANTUM NUMBERS --------------------------------------//
//-------------------------------------------------------------------------------------------------------//
//----------------------------------------- FUNCTION HEADERS --------------------------------------------//
GLfloat r_of (GLfloat x, GLfloat y, GLfloat z);
GLfloat theta_of (GLfloat x, GLfloat y, GLfloat z);
GLfloat phi_of (GLfloat x, GLfloat y, GLfloat z);

GLfloat _100_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _200_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _210_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _211_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _300_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _310_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _311_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _320_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _321_eq(GLfloat r, GLfloat theta, GLfloat phi);
GLfloat _322_eq(GLfloat r, GLfloat theta, GLfloat phi);
//-------------------------------------- END FUNCTION HEADERS --------------------------------------------------//

//----------------------------- AXIS ARRAYS ---------------------------------------------------------------//
// AXIS VERTICES
GLfloat xAxisVertices[] = {
    // Bottom Face
    -5.00f, -0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Bottom-left
     5.00f, -0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Bottom-right
     5.00f, -0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Top-right
    -5.00f, -0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Top-left

    // Top Face
    -5.00f,  0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Bottom-left
     5.00f,  0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Bottom-right
     5.00f,  0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Top-right
    -5.00f,  0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Top-left

    // Front Face
    -5.00f, -0.01f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Bottom-left
     5.00f, -0.01f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Bottom-right
     5.00f,  0.01f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Top-right
    -5.00f,  0.01f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Top-left

    // Back Face
    -5.00f, -0.01f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Bottom-left
     5.00f, -0.01f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Bottom-right
     5.00f,  0.01f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Top-right
    -5.00f,  0.01f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Top-left

    // Left Face
    -5.00f, -0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Bottom-back
    -5.00f, -0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Bottom-front
    -5.00f,  0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Top-front
    -5.00f,  0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Top-back

    // Right Face
     5.00f, -0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Bottom-back
     5.00f, -0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Bottom-front
     5.00f,  0.01f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Top-front
     5.00f,  0.01f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Top-back
};
GLfloat yAxisVertices[] = {
    // Bottom Face
    -0.01f, -5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Bottom-left
     0.01f, -5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Bottom-right
     0.01f, -5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Top-right
    -0.01f, -5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Top-left

    // Top Face
    -0.01f,  5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Bottom-left
     0.01f,  5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Bottom-right
     0.01f,  5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Top-right
    -0.01f,  5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Top-left

    // Front Face
    -0.01f, -5.00f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Bottom-left
     0.01f, -5.00f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Bottom-right
     0.01f,  5.00f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Top-right
    -0.01f,  5.00f,  0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Top-left

    // Back Face
    -0.01f, -5.00f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Bottom-left
     0.01f, -5.00f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Bottom-right
     0.01f,  5.00f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Top-right
    -0.01f,  5.00f, -0.01f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Top-left

    // Left Face
    -0.01f, -5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Bottom-back
    -0.01f, -5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Bottom-front
    -0.01f,  5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Top-front
    -0.01f,  5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Top-back

    // Right Face
     0.01f, -5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Bottom-back
     0.01f, -5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Bottom-front
     0.01f,  5.00f,  0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Top-front
     0.01f,  5.00f, -0.01f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Top-back
};
GLfloat zAxisVertices[] = {
    // Back Face
    -0.01f, -0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Bottom-left
     0.01f, -0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Bottom-right
     0.01f,  0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Top-right
    -0.01f,  0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,  // Top-left

    // Front Face
    -0.01f, -0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Bottom-left
     0.01f, -0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Bottom-right
     0.01f,  0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Top-right
    -0.01f,  0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,  // Top-left

    // Bottom Face
    -0.01f, -0.01f, -5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Back-left
     0.01f, -0.01f, -5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Back-right
     0.01f, -0.01f,  5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Front-right
    -0.01f, -0.01f,  5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,  // Front-left

    // Top Face
    -0.01f,  0.01f, -5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Back-left
     0.01f,  0.01f, -5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Back-right
     0.01f,  0.01f,  5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Front-right
    -0.01f,  0.01f,  5.00f,    0.92f, 0.86f, 0.76f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,  // Front-left

    // Left Face
    -0.01f, -0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Bottom-back
    -0.01f, -0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Bottom-front
    -0.01f,  0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Top-front
    -0.01f,  0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,  // Top-back

    // Right Face
     0.01f, -0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Bottom-back
     0.01f, -0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Bottom-front
     0.01f,  0.01f,  5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Top-front
     0.01f,  0.01f, -5.00f,    0.83f, 0.70f, 0.44f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,  // Top-back
};

// AXIS INDICES
GLuint xAxisIndices[] =
{
    // Bottom Face
    0, 1, 2,
    2, 3, 0,
    
    // Top Face
    4, 5, 6,
    6, 7, 4,

    // Front Face
    8, 9, 10,
    10, 11, 8,

    // Back Face
    12, 13, 14,
    14, 15, 12,

    // Left Face
    16, 17, 18,
    18, 19, 16,

    // Right Face
    20, 21, 22,
    22, 23, 20,
};
GLuint yAxisIndices[] = {
    // Bottom Face
    0, 1, 2,
    0, 2, 3,

    // Top Face
    4, 5, 6,
    4, 6, 7,

    // Front Face
    8, 9, 10,
    8, 10, 11,

    // Back Face
    12, 13, 14,
    12, 14, 15,

    // Left Face
    16, 17, 18,
    16, 18, 19,

    // Right Face
    20, 21, 22,
    20, 22, 23,
};
GLuint zAxisIndices[] = {
    // Back Face
    0, 1, 2,
    0, 2, 3,

    // Front Face
    4, 5, 6,
    4, 6, 7,

    // Bottom Face
    8, 9, 10,
    8, 10, 11,

    // Top Face
    12, 13, 14,
    12, 14, 15,

    // Left Face
    16, 17, 18,
    16, 18, 19,

    // Right Face
    20, 21, 22,
    20, 22, 23,
};

GLfloat lightVertices[] =
{ //     COORDINATES     //
    5.95f, 5.95f,  6.05f,  // Front face
    5.95f, 5.95f,  5.95f,
    6.05f, 5.95f,  5.95f,
    6.05f, 5.95f,  6.05f,
    5.95f, 6.05f,  6.05f,  // Back face
    5.95f, 6.05f,  5.95f,
    6.05f, 6.05f,  5.95f,
    6.05f, 6.05f,  6.05f
};

GLuint lightIndices[] =
{
    0, 1, 2,   // Front face
    0, 2, 3,
    0, 4, 7,   // Left face
    0, 7, 3,
    3, 7, 6,   // Right face
    3, 6, 2,
    2, 6, 5,   // Bottom face
    2, 5, 1,
    1, 5, 4,   // Top face
    1, 4, 0,
    4, 5, 6,   // Back face
    4, 6, 7
};
//------------------------- END AXIS ARRAYS ----------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------//
//--------------------------- MAIN METHOD ------------------------------------------------------------------//
int main()
{
    //Relative path
    std::string parentDir = (fs::current_path().fs::path::parent_path()).string();

    //--------------------------- END USER INPUT ---------------------------------------------------------//
    std::cout << "Hydrogen Atom Orbital Simulator.\n";// (Currently only n=1,2,3, l=0,1,2, ml=+/-0,1,2 are supported)\n";
    std::cout << "Enter desired principal quantum number........ n = ";
    std::cin >> n;
    std::cout << "Enter desired angular momentum quantum number. l = ";
    std::cin >> l;
    std::cout << "Enter desired magnetic quantum number........ ml = ";
    std::cin >> ml;
    std::cout << "\n";
    
    // Check if quanutm numbers are allowed
    if (l+1>n || abs(ml)>abs(l) || l<0) {
        std::cout << "This combination of quantum numbers is not allowed.\n";
        return 0;
    }
    //--------------------------- END USER INPUT ---------------------------------------------------------//
    //---------------------------------------------------------------------------------------------------//
    //---------------------------- WINDOW SETUP ---------------------------------------------------------//
    // Initialize GLFW
    glfwInit();

    // Tell GLFW what version of OpenGL we are using
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we are using the CORE profile
    // So that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a window
    //std::char title = "Hydrogen Atom Sim - n = " + std::to_string(n) + ", l = " + std::to_string(l) + ", ml = " + std::to_string(ml);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hydrogen Atom Orbital Simulation", NULL, NULL);
    // Error check if the window fails to create
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    //Load GLAD so it configures OpenGL
    gladLoadGL();
    // Specify the viewport of OpenGL in the Window
    glViewport(0, 0, WIDTH, HEIGHT);
    //------------------------ END WINDOW SETUP ---------------------------------------------------------//
    //---------------------------------------------------------------------------------------------------//
    //-----------------------  MULTIPLE SPHERES ---------------------------------------------------------//
    //----------------------------------------------------------------------------------------------//
    //-------------------- GENERATE TOTAL VERTICES VECTOR ------------------------------------------//

    // allSpheres_VertexVec will be formatted such that each vector stored in it will be a formatted vertex/attributes vector
    std::vector<std::vector<GLfloat>> allSpheres_VertexVec;
    
    // 125 spheres -> 5x5x5 cube // LATER I will make the number of spheres variable based on the largest r of the wavefunction that produces a probability density above some constant
    int numSpheres_per_side = cbrt(numSpheres);
    GLfloat step = 10.00 / (numSpheres_per_side - 1); // 5.00 units = 1 Bohr radius
    
    // Add sphere vectors to allSpheres_VertexVec
    // each sphere represents the probabiility density at that location
    // sphereRadius and sphereColor of each sphere based on the probability density of the wavefunction at that point

    // 1s1 Orbital:
    if (n == 1 && l == 0 && ml == 0) {
        std::cout << "Scale factor: axes extend to 1 Bohr (0.5 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);

                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 5.0 = 1 Bohr radius
                    GLfloat probDensity = _100_eq(r/5.0f,theta,phi);
                    //std::cout << "Prob Density is: " << probDensity << std::endl;

                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // 0.31f is the maximum of the 1s1 prob density
                    GLfloat sphereRadius = step/1.5f * probDensity/0.31f;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity/0.31f); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity/0.31f;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;

                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord

                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi

                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)

                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);

                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);

                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//

                }
            }
        }
        
    } else if (n == 2 && l == 0 && ml == 0) {
        std::cout << "Scale factor: axes extend to 2 Bohr (1.0 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 2.5 = 1 Bohr radius
                    GLfloat probDensity = _200_eq(r/2.5f,theta,phi);
                    //std::cout << "Prob Density is: " << probDensity << std::endl;
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 2 && l == 1 && ml == 0) {
        std::cout << "Scale factor: axes extend to 1 Bohr (0.5 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 5.0 = 1 Bohr radius
                    GLfloat probDensity = _210_eq(r,theta,phi);
                    //std::cout << "Prob Density is: " << probDensity << std::endl;
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 2 && l == 1 && (ml == 1 || ml == -1)) {
        std::cout << "Scale factor: axes extend to 5 Bohr (2.5 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 1.0 = 1 Bohr radius
                    GLfloat probDensity = _211_eq(r/1.0,theta,phi);
                    //std::cout << "Prob Density is: " << probDensity << std::endl;
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 3 && l == 0 && ml == 0) {
        std::cout << "Scale factor: axes extend to 10 Bohr (5.0 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 0.5 = 1 Bohr radius
                    GLfloat probDensity = _300_eq(r/0.5,theta,phi);
                    //std::cout << "Prob Density is: " << probDensity << std::endl;
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 3 && l == 1 && ml == 0) {
        std::cout << "Scale factor: axes extend to 7.5 Bohr (3.75 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 0.666666666 = 1 Bohr radius
                    GLfloat probDensity = _310_eq(r/0.666666666f,theta,phi);
                    //std::cout << "Prob Density is: " << probDensity << std::endl;
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 3 && l == 1 && (ml == 1 || ml == -1)) {
        std::cout << "Scale factor: axes extend to 6.7 Bohr (3.3 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 0.75 = 1 Bohr radius
                    GLfloat probDensity = _311_eq(r/0.666666666666f,theta,phi);
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 3 && l == 2 && ml == 0) {
        std::cout << "Scale factor: axes extend to 10 Bohr (5 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 0.5 = 1 Bohr radius
                    GLfloat probDensity = _320_eq(r/0.4f,theta,phi);
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 3 && l == 2 && (ml == 1 || ml == -1)) {
        std::cout << "Scale factor: axes extend to 10 Bohr (5 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 0.5 = 1 Bohr radius
                    GLfloat probDensity = _321_eq(r/0.5f,theta,phi);
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else if (n == 3 && l == 2 && (ml == 2 || ml == -2)) {
        std::cout << "Scale factor: axes extend to 10 Bohr (5 A).\n";
        // Nested for loops traverse a cube centered on the origin in 3D space
        for (int i = 0; i < numSpheres_per_side; i++) {
            for (int j = 0; j < numSpheres_per_side; j++) {
                for (int k = 0; k < numSpheres_per_side; k++) {
                    // At each point on the cube of traversed values, we create an array of vertices for the probability density sphere at that point
                    // NOTE: r IS THE DISTANCE OF THE SPHERE FROM THE ORIGIN, sphereRadius IS THE SPHERE RADIUS
                    GLfloat x = -5.00f + (i * step); // x,y,x is the center of the probability sphere
                    GLfloat y = -5.00f + (j * step);
                    GLfloat z = -5.00f + (k * step);
                    
                    GLfloat r = r_of(x,y,z);
                    GLfloat theta = theta_of(x,y,z);
                    GLfloat phi = phi_of(x,y,z);
                    
                    // conversion factor: 0.5 = 1 Bohr radius
                    GLfloat probDensity = _322_eq(r/0.5f,theta,phi);
                    
                    // Conversion factor: step/2 => sphereRadius is maximum (subject to change)
                    // maximum of the 2s prob density
                    GLfloat sphereRadius = step/1.5f * probDensity;
                    GLfloat sphereColor_red = 1.0f * (1-probDensity); // More red = lower prob density
                    GLfloat sphereColor_green = 1.0f * probDensity;   // More green = higher prob density
                    GLfloat sphereColor_blue = 0.2f;                        // Blue is constant for now
                    
                    //std::cout << "Color : " << (int)(sphereColor_red*255) << " " << (int)(sphereColor_green*255) << " " << (int)(sphereColor_blue*255) << "\n";
                    
                    //------------------------------ GENERATE SPHERE -----------------------------------//
                    //-------------------------------------------------------------------//
                    //------------ GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    std::vector<GLfloat> sphereVertices;
                    std::vector<GLfloat> sphereNormals;
                    std::vector<GLfloat> sphereTexCoords;
                    
                    std::vector<GLfloat>().swap(sphereVertices);
                    std::vector<GLfloat>().swap(sphereNormals);
                    std::vector<GLfloat>().swap(sphereTexCoords);
                    GLfloat x_local,y_local,z_local,xy_local;               // vertex position
                    GLfloat nx, ny, nz, lengthInv = 1.0f / sphereRadius;    // vertex normal
                    GLfloat s, t;                                           // vertex texCoord
                    
                    GLfloat sectorStep = 2 * PI / sectorCount;
                    GLfloat stackStep = PI / stackCount;
                    GLfloat sectorAngle, stackAngle;
                    
                    for(int i_local = 0; i_local <= stackCount; ++i_local)
                    {
                        stackAngle = PI / 2 - i_local * stackStep;        // starting from pi/2 to -pi/2
                        xy_local = sphereRadius * cosf(stackAngle);             // r * cos(u)
                        z_local = sphereRadius * sinf(stackAngle);              // r * sin(u)
                        // add (sectorCount+1) vertices per stack
                        // first and last vertices have same position and normal, but different tex coords
                        for(int j_local = 0; j_local <= sectorCount; ++j_local)
                        {
                            sectorAngle = j_local * sectorStep;           // starting from 0 to 2pi
                            
                            // vertex position with respect to (x, y, z) is (x_local, y_local, z_local)
                            x_local = xy_local * cosf(sectorAngle);             // r * cos(u) * cos(v)
                            y_local = xy_local * sinf(sectorAngle);             // r * cos(u) * sin(v)
                            
                            sphereVertices.push_back(x_local + x);
                            sphereVertices.push_back(y_local + y);              // + x,y,z
                            sphereVertices.push_back(z_local + z);
                            
                            // normalized vertex normal (nx, ny, nz)
                            nx = x_local * lengthInv;
                            ny = y_local * lengthInv;
                            nz = z_local * lengthInv;
                            sphereNormals.push_back(nx);
                            sphereNormals.push_back(ny);
                            sphereNormals.push_back(nz);
                            
                            // vertex tex coord (s, t) range between [0, 1]
                            s = (GLfloat)i_local / sectorCount;
                            t = (GLfloat)i_local / stackCount;
                            sphereTexCoords.push_back(s);
                            sphereTexCoords.push_back(t);
                        }
                    }
                    //---------- END GENERATE VERTEX,NORMAL,TEXCOORD VECTORS --------------//
                    //---------------------------------------------------------------------//
                    //-------------- GENERATE COMPLETE VERTEX VECTOR ----------------------//
                    std::vector<GLfloat> completeSphereVertexVec;
                    int length_completeSphereVertexVec = (int) ((sphereVertices.size() * 11.0)/3);
                    
                    int j_a = 0;
                    for (int i_a = 0; i_a < length_completeSphereVertexVec;) {
                        // COORDINATES:
                        for (int k_a = j_a; k_a < j_a+3; k_a++) {
                            completeSphereVertexVec.push_back(sphereVertices.at(k_a)); i_a++;
                            //std::cout << sphereVertices.at(k_a) << std::endl;
                        }
                        j_a += 3;
                        // COLORS:
                        completeSphereVertexVec.push_back(sphereColor_red); i_a++;// RED
                        completeSphereVertexVec.push_back(sphereColor_green); i_a++; // GREEN
                        completeSphereVertexVec.push_back(sphereColor_blue); i_a++; // BLUE
                        // TEXCOORD:
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        completeSphereVertexVec.push_back(0.0f); i_a++; // NO TEXTURE
                        // NORMALS:
                        for (int k_a = j_a-3; k_a < j_a; k_a++) {
                            completeSphereVertexVec.push_back(sphereNormals.at(k_a)); i_a++;
                        }
                    }
                    //------------- END GENERATE COMPLETE VERTEX VECTOR ------------------//
                    //--------------------------------------------------------------------//
                    //------- ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec ---------//
                    allSpheres_VertexVec.push_back(completeSphereVertexVec);
                    
                    //----- END ADD COMPLETE VERTEX VECTOR TO allSpheres_VertexVec -------//
                    //--------------------------------------------------------------------//
                    //-----------------------------END GENERATE SPHERE ------------------------------//
                    
                }
            }
        }
    } else {
        std::cout << "The quantum numbers entered are not yet supported.\n";
        return 0;
    }
    //----------------------- END GENERATE TOTAL VERTICES VEC ---------------------------------------//
    //-----------------------------------------------------------------------------------------------//
    //----------------------- CONVERT TOTAL VERTICES VEC TO ARRAY -----------------------------------//

    GLfloat allSpheres_VertexArr[numSpheres * allSpheres_VertexVec.at(0).size()];
    int i_arr = 0;
    for (int i = 0; i < allSpheres_VertexVec.size(); i++) {
        for (int j = 0; j < allSpheres_VertexVec.at(i).size(); j++) {
            allSpheres_VertexArr[i_arr++] = allSpheres_VertexVec.at(i).at(j);
        }
    }
    /*for (int i = 0; i<sizeof(allSpheres_VertexArr)/sizeof(GLfloat); i++) {
        std::cout << "This is allSpheres_VertexArr: " << allSpheres_VertexArr[i] << "\n";
    }*/

    //--------------------- END CONVERT TOTAL VERTICES VEC TO ARRAY ----------------------------------//
    //------------------------------------------------------------------------------------------------//
    //----------------------- GENERATE SINGLE INDICES ARRAY -------------------------------------------//
    // Generate CCW index list of sphere triangles
    // k1--k1+1
    // |  / |
    // | /  |
    // k2--k2+1
    std::vector<int> singleSphere_IndicesVec;
    std::vector<int> lineIndices;
    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                singleSphere_IndicesVec.push_back(k1);
                singleSphere_IndicesVec.push_back(k2);
                singleSphere_IndicesVec.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                singleSphere_IndicesVec.push_back(k1 + 1);
                singleSphere_IndicesVec.push_back(k2);
                singleSphere_IndicesVec.push_back(k2 + 1);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            if(i != 0)  // horizontal lines except 1st stack, k1 => k+1
            {
                lineIndices.push_back(k1);
                lineIndices.push_back(k1 + 1);
            }
        }
    }
    //--------------------- END GENERATE SINGLE INDICES ARRAY ----------------------------------------//
    //------------------------------------------------------------------------------------------------//
    //----------------------- GENERATE TOTAL INDICES ARRAY -------------------------------------------//
    // Create a total indices ARRAY by adding numSpheres number of single sphere indices vectors
    // Length is numSpheres * length of a single sphere indices vector
    GLuint allSpheres_IndicesArr[numSpheres * singleSphere_IndicesVec.size()];
    for (int i = 0; i < sizeof(allSpheres_IndicesArr) / sizeof(GLuint); i++) {
        int mod_i = i % singleSphere_IndicesVec.size();
        int n = i / singleSphere_IndicesVec.size();
        allSpheres_IndicesArr[i] = singleSphere_IndicesVec.at(mod_i) + NUM_VERTICES_PER_SPHERE * n;
    }
    /*std::cout << "Length of singleSphere_IndicesArr: " << singleSphere_IndicesVec.size() << std::endl;

    std::cout << "Length of allSpheres_IndicesArr: " << sizeof(allSpheres_IndicesArr)/sizeof(GLuint) << std::endl;
    std::cout << "This is the length of singleSphere_VertexArr: " <<  sizeof(singleSphere_VertexArr)/sizeof(GLfloat) << std::endl;
    std::cout << "This is the length of allSpheres_VertexArr: " <<  sizeof(allSpheres_VertexArr)/sizeof(GLfloat) << std::endl;*/

    //---------------------- END GENERATE TOTAL INDICES ARRAY ----------------------------------------//
    //------------------------------------------------------------------------------------------------//
    //-------------------- END MULTIPLE SPHERES ---------------------------------------------------------//
    //---------------------------------------------------------------------------------------------------//
    //-------------------- SHADERS ----------------------------------------------------------------------//
    // Generates Shader object using shaders default.vert and default.frag
    std::string vert_path = parentDir + "/Debug/default.vert";
    std::string frag_path = parentDir + "/Debug/default.frag";

    Shader shaderProgram(vert_path, frag_path);
    
    // ----- FOR X AXIS -------- //
    // Generates Vertex Array Object and binds it
    VAO VAO1;
    VAO1.Bind();
    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO1(xAxisVertices, sizeof(xAxisVertices));
    // Generates Element Buffer Object and links it to indices
    EBO EBO1(xAxisIndices, sizeof(xAxisIndices));
    // Links VBO attributes such as coordinates and colors to VAO
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();
    
    // ----- FOR Y AXIS ------- //
    // Generates Vertex Array Object and binds it
    VAO VAO2;
    VAO2.Bind();
    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO2(yAxisVertices, sizeof(yAxisVertices));
    // Generates Element Buffer Object and links it to indices
    EBO EBO2(yAxisIndices, sizeof(yAxisIndices));
    // Links VBO attributes such as coordinates and colors to VAO
    VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO2.LinkAttrib(VBO2, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO2.LinkAttrib(VBO2, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    VAO2.Unbind();
    VBO2.Unbind();
    EBO2.Unbind();
    
    // ----- FOR Z AXIS -------- //
    // Generates Vertex Array Object and binds it
    VAO VAO3;
    VAO3.Bind();
    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO3(zAxisVertices, sizeof(zAxisVertices));
    // Generates Element Buffer Object and links it to indices
    EBO EBO3(zAxisIndices, sizeof(zAxisIndices));
    // Links VBO attributes such as coordinates and colors to VAO
    VAO3.LinkAttrib(VBO3, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO3.LinkAttrib(VBO3, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO3.LinkAttrib(VBO3, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO3.LinkAttrib(VBO3, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    VAO3.Unbind();
    VBO3.Unbind();
    EBO3.Unbind();
    
    // ----- FOR SPHERES -------- //
    // Generates Vertex Array Object and binds it
    VAO VAO4;
    VAO4.Bind();
    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO4(allSpheres_VertexArr, sizeof(allSpheres_VertexArr));
    // Generates Element Buffer Object and links it to indices
    EBO EBO4(allSpheres_IndicesArr, sizeof(allSpheres_IndicesArr));
    // Links VBO attributes such as coordinates and colors to VAO
    VAO4.LinkAttrib(VBO4, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO4.LinkAttrib(VBO4, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO4.LinkAttrib(VBO4, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO4.LinkAttrib(VBO4, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    // Unbind all to prevent accidentally modifying them
    VAO4.Unbind();
    VBO4.Unbind();
    EBO4.Unbind();
    
    // ------------ LIGHT --------------- //
    // Shader for light cube
    Shader lightShader("/Users/chrisgudmundsen/Desktop/Coding Projects/Orbital Simulation/Shaders/light.vert", "/Users/chrisgudmundsen/Desktop/Coding Projects/Orbital Simulation/Shaders/light.frag");
    // Generates Vertex Array Object and binds it
    VAO lightVAO;
    lightVAO.Bind();
    // Generates Vertex Buffer Object and links it to vertices
    VBO lightVBO(lightVertices, sizeof(lightVertices));
    // Generates Element Buffer Object and links it to indices
    EBO lightEBO(lightIndices, sizeof(lightIndices));
    // Links VBO attributes such as coordinates and colors to VAO
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    // Unbind all to prevent accidentally modifying them
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 pyramidModel = glm::mat4(1.0f);
    pyramidModel = glm::translate(pyramidModel, pyramidPos);

    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    shaderProgram.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    // ------------ END LIGHT -----------------------//
    
    // ------------ TEXTURE -----------------------//
    /* NOTE TO READER/FUTURE ME: Don't ask me why I am going to all this trouble to bind this brick
       texture when I don't even end up using it... The code doesn't work when I get rid of it - and
       I don't want to spend time figuring out why. So here it stays.
     */
        
    /* ANOTHER NOTE: Yes, brick.png is in the debug folder. Yes, the reason is because I suck at programming. */
    std::string texPath = parentDir + "/Debug/brick.png";

    Texture brickTex(texPath, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    brickTex.texUnit(shaderProgram, "tex0", 0);
    // ------------ END TEXTURE --------------------//

    // Enables the Depth Buffer
    glEnable(GL_DEPTH_TEST);
    // Creates camera object
    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 6.0f));

    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // Clean the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Handles camera inputs
        camera.Inputs(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.updateMatrix(45.0f, 0.1f, 100.0f);


        // Tells OpenGL which Shader Program we want to use
        shaderProgram.Activate();
        // Exports the camera Position to the Fragment Shader for specular lighting
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        // Export the camMatrix to the Vertex Shader
        camera.Matrix(shaderProgram, "camMatrix");
        // Binds texture so that it appears in rendering
        brickTex.Bind();
        
        
        // Bind VAO1 (bind x axis)
        VAO1.Bind();
        // Draw primitives, number of indices, datatype of indices, index of indices
        glDrawElements(GL_TRIANGLES, sizeof(xAxisIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Bind VAO2 (bind y axis)
        VAO2.Bind();
        // Draw primitives, number of indices, datatype of indices, index of indices
        glDrawElements(GL_TRIANGLES, sizeof(yAxisIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
        
        // Bind VAO3 (bind z axis)
        VAO3.Bind();
        // Draw primitives, number of indices, datatype of indices, index of indices
        glDrawElements(GL_TRIANGLES, sizeof(zAxisIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Bind VAO4 (bind all spheres)
        VAO4.Bind();
        // Draw primitives, number of indices, datatype of indices, index of indices
        glDrawElements(GL_TRIANGLES, sizeof(allSpheres_IndicesArr) / sizeof(int), GL_UNSIGNED_INT, 0);


        
        // Tells OpenGL which Shader Program we want to use
        lightShader.Activate();
        // Export the camMatrix to the Vertex Shader of the light cube
        camera.Matrix(lightShader, "camMatrix");
        // Bind the VAO so OpenGL knows to use it
        lightVAO.Bind();
        // Draw primitives, number of indices, datatype of indices, index of indices
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();
    }



    // ----- Delete all the objects we've created ------- //
    
    // x axis
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    
    // y axis
    VAO2.Delete();
    VBO2.Delete();
    EBO2.Delete();
    
    // z axis
    VAO3.Delete();
    VBO3.Delete();
    EBO3.Delete();
    
    // SPHERES
    VAO4.Delete();
    VBO4.Delete();
    EBO4.Delete();
    
    brickTex.Delete();
    shaderProgram.Delete();
    lightVAO.Delete();
    lightVBO.Delete();
    lightEBO.Delete();
    lightShader.Delete();
    
    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
    return 0;
}





//------------------------------------FUNCTIONS----------------------------------------------------//

GLfloat r_of (GLfloat x, GLfloat y, GLfloat z) {
    return sqrt(x*x + y*y + z*z);
}
GLfloat theta_of (GLfloat x, GLfloat y, GLfloat z) {
    GLfloat r = sqrt(x*x + y*y + z*z);
    return acos(z / r);
}
GLfloat phi_of (GLfloat x, GLfloat y, GLfloat z) {
    return atan2(y, x);
}

//-----------------------------------EQUATIONS-----------------------------------------------------//
// r -> distance from origin, theta -> polar angle, phi -> azimuthal angle
// All return the probability DENSITY (wavefunction squared) as GLfloat

// (n=1,l=0,ml=0)
GLfloat _100_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 1.0f * std::pow(10, 0); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * exp(-r) / sqrt(PI);
    return pow(wavefunction, 2);
}

// (n=2,l=0,ml=0)
GLfloat _200_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 1.0f * std::pow(10, -1); // Calculate manually
    
    GLfloat wavefunction = 1 / max_prob_amp * 1/8 / pow(2.0*PI, 0.5) * (2-r) * exp(-r/2);
    return pow(wavefunction, 2);
}

// (n=2,l=1,ml=0)
GLfloat _210_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 8.7f * std::pow(10, -1); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * r * exp(-r/2) * cos(theta);
    return pow(wavefunction, 2);
}

// (n=2,l=1,ml=+/-1)
GLfloat _211_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 7.5 * std::pow(10, -1); // Calculate manually

    // The imaginary portion is not included here because in the probability density the conplex conjugate cancels it out.
    GLfloat wavefunction = 1 / max_prob_amp * r * exp(-r/2) * sin(theta);
    return pow(wavefunction, 2);
}

GLfloat _300_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 25 * std::pow(10, -1); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * (27 - 18*r + 2*r*r) * exp(-r/2);
    return pow(wavefunction, 2);
}

GLfloat _310_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 4 * std::pow(10, 0); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * (6 - r) * r * exp(-r/3) * cos(theta);
    return pow(wavefunction, 2);
}

GLfloat _311_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 4.5 * std::pow(10, 0); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * (6 - r) * r * exp(-r/3) * sin(theta);
    return pow(wavefunction, 2);
}

GLfloat _320_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 92.f * std::pow(10, -1); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * pow(r,2) * exp(-r/3) * (3 * pow(cos(theta) , 2) - 1);
    return pow(wavefunction, 2);
}

GLfloat _321_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 25.f * std::pow(10, -1); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * pow(r,2) * exp(-r/3) * sin(theta) * cos(theta);
    return pow(wavefunction, 2);
}

GLfloat _322_eq(GLfloat r, GLfloat theta, GLfloat phi) {
    const GLfloat max_prob_amp = 48.f * std::pow(10, -1); // Calculate manually

    GLfloat wavefunction = 1 / max_prob_amp * pow(r,2) * exp(-r/3) * pow(sin(theta) , 2);
    return pow(wavefunction, 2);
}
//-------------------------------END EQUATIONS-----------------------------------------------------//
