// main.cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "PointsObject.hpp"

// Function prototypes
int initWindow(void);
static void mouseCallback(GLFWwindow*, int, int, int);
int getPickedIndex();
glm::vec3 getWorldPosition(const glm::mat4& viewMatrix,
                           const glm::mat4& projectionMatrix);



const GLuint windowWidth = 1024, windowHeight = 768;
GLFWwindow* window;

// The point that is currently selected.
int currSelected =-1;
bool justPressed = false;
bool showBezier = false;
bool justPressed2 = false;
bool justPressedShift = false;
bool editingZ = false;
bool justPressed4 = false;
bool doubleView = false;
bool justPressed5 = false;
int loopsElapsed = 0;

int main() {
    // ATTN: REFER TO https://learnopengl.com/Getting-started/Creating-a-window
    // AND https://learnopengl.com/Getting-started/Hello-Window to familiarize yourself with the initialization of a window in OpenGL
    
    if (initWindow() != 0) return -1;


    glm::mat4 projectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates
    glm::mat4 projectionMatrixBottom = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f);

    
    //TODO: P2aTask1 - Display 8 points on the screen each of a different color and arranged uniformly on a circle.
    PointsObject pointsObj({
        {1.0f, 0.0f, 0.0f},  // First point position
        {sqrt(2)/2, sqrt(2)/2, 0.0f},
        {0.0f, 1.0f, 0.0f},  // Second point position
        {-sqrt(2) / 2, sqrt(2) / 2, 0.0f},
        {-1.0f, 0.0f, 0.0f}, // Third point position
        {-sqrt(2) / 2, -sqrt(2) / 2, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {sqrt(2) / 2, -sqrt(2) / 2, 0.0f},


    }, {
        {1.0f, 0.0f, 0.0f},  // First point color (red)
        {1.0f, 0.5f, 0.0f},   // (orange)
        {1.0f, 1.0f, 0.0f},   // (yellow)
        {0.0f, 1.0f, 0.0f},  // Second point color (green)
        {0.0f, 1.0f, 1.0f},   // (cyan)
        {0.0f, 0.0f, 1.0f},  // Third point color (blue)
        {0.5f, 0.0f, 0.5f},   // (purple)
        {1.0f, 0.0f, 1.0f},   // (magenta)

    });

    PointsObject pointsObjZ({
        {0.0f, 0.0f, 0.0f},  // First point position
        {0.0f, sqrt(2)/2, 0.0f},
        {0.0f, 1.0f, 0.0f},  // Second point position
        {0.0f, sqrt(2) / 2, 0.0f},
        {0.0f, 0.0f, -1.0f}, // Third point position
        {0.0f, -sqrt(2) / 2,0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -sqrt(2) / 2, 0.0f},


    }, {
        {1.0f, 0.0f, 0.0f},  // First point color (red)
        {1.0f, 0.5f, 0.0f},   // (orange)
        {1.0f, 1.0f, 0.0f},   // (yellow)
        {0.0f, 1.0f, 0.0f},  // Second point color (green)
        {0.0f, 1.0f, 1.0f},   // (cyan)
        {0.0f, 0.0f, 1.0f},  // Third point color (blue)
        {0.5f, 0.0f, 0.5f},   // (purple)
        {1.0f, 0.0f, 1.0f},   // (magenta)

    });
    
    double lastTime = glfwGetTime();
    int nbFrames = 0;
    do {
        loopsElapsed++;
        // Timing
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }
        
        
        glm::mat4 viewMatrix = glm::mat4(1.0f); // Identity matrix

        if (pointsObj.animatingPoint && loopsElapsed > 20) {
            pointsObj.currentPoint = (pointsObj.currentPoint + 1) % pointsObj.bezierPositions.size();
            pointsObjZ.currentPoint = (pointsObjZ.currentPoint + 1) % pointsObjZ.bezierPositions.size();
            loopsElapsed = 0;
        }
        if(currSelected >= 0){
            // Dragging for P2aTask3
            glm::vec3 worldPos = getWorldPosition(viewMatrix, projectionMatrix);
            pointsObj.updatePoint(currSelected, glm::vec3(worldPos.x, worldPos.y, 0));
            pointsObjZ.updatePoint(currSelected, glm::vec3(worldPos.z, worldPos.y, 0));
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){
            // Draw picking for P2aTask2
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            pointsObj.drawPicking(viewMatrix, projectionMatrix);
            currSelected = getPickedIndex();
            
            std::cout << "Picked id: " << currSelected << std::endl;
                        
        }
        else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !justPressed) {
            pointsObj.k = (pointsObj.k + 1) % 6;
            pointsObj.SubdivideStart();

            pointsObjZ.k = (pointsObjZ.k + 1) % 6;
            pointsObjZ.SubdivideStart();
            justPressed = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !justPressed2) {
            showBezier = !showBezier;
            pointsObj.drawBezier = showBezier;
            pointsObjZ.drawBezier = showBezier;

            justPressed2 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !justPressed4) {
            doubleView = !doubleView;

            int windowWidth;
            int windowHeight;
            glfwGetWindowSize(window, &windowWidth, &windowHeight);
            pointsObjZ.yOffset = -1.5;

            justPressed4 = true;
            if (doubleView) {
                pointsObj.yOffset = 1.5;
                // Draw top view
                glViewport(0, windowHeight, windowWidth, windowHeight);
                // Draw bottom view
                glViewport(0, 0, windowWidth, windowHeight);

            }
            else {
                pointsObj.yOffset = 0;
                glViewport(0, 0, windowWidth, windowHeight);

            }
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !justPressedShift) {
            editingZ = !editingZ;
            pointsObj.editingZ = editingZ;
            justPressedShift = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !justPressed5) {
            pointsObj.animatePoint();
            pointsObjZ.animatePoint();
            justPressed5 = true;
        }

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            justPressed = false;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
            justPressed2 = false;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
            justPressedShift = false;
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
            justPressed4 = false;
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) {
            justPressed5 = false;
        }

        if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
            currSelected = -1;
        }

        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // DRAWING the SCENE
        
        if (doubleView) {
            //glOrtho(0,windowWidth,windowHeight,0,-10,10);
            pointsObj.draw(viewMatrix, projectionMatrix);
            pointsObjZ.draw(viewMatrix, projectionMatrixBottom);

        }
        else {
            pointsObj.draw(viewMatrix, projectionMatrix);

        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

    glfwTerminate();
    return 0;
}

// Initialize GLFW and create a window
int initWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC

    window = glfwCreateWindow(windowWidth, windowHeight, "Pham, Hailey (2475-2485)", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Set up inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    // Make points big
    glPointSize(20.0f);
    
    return 0;
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Left mouse button pressed" << std::endl;
    }
}

int getPickedIndex(){
    glFlush();
    // --- Wait until all the pending drawing commands are really done.
    // Ultra-mega-over slow !
    // There are usually a long time between glDrawElements() and
    // all the fragments completely rasterized.
    glFinish();
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned char data[4];

    //TODO: P2aTask2 - Use glfwGetCursorPos to get the x and y value of the cursor.
    double x_read;
    double y_read;
    glfwGetCursorPos(window, &x_read, &y_read);

    //TODO: P2aTask2 - Use glfwGetFramebufferSize and glfwGetWindowSize to get the frame buffer size and window size. On high resolution displays, these sizes might be different.
    int bufferWidth;
    int bufferHeight;

    int windowWidth;
    int windowHeight;

    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int bufferSize = bufferWidth * bufferHeight;
    int windowSize = windowWidth * windowHeight;

    int multiplier = bufferSize / windowSize;

    glReadPixels(x_read*multiplier, (windowHeight-y_read)*multiplier, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //TODO: P2aTask2 - Use glReadPixels(x_read, y_read, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data) to read the pixel data.
    // Note that y position has to be flipped as glfwGetCursorPos gives the cursor position relative to top left of the screen. The read location must also be multiplied by (buffer size / windowSize) for some displays.
    
    
    int pickedId = data[0] - 1;
    return pickedId;
}
glm::vec3 getWorldPosition(const glm::mat4& viewMatrix,
                           const glm::mat4& projectionMatrix)
{
    int bufferWidth;
    int bufferHeight;

    int windowWidth;
    int windowHeight; 
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int bufferSize = bufferWidth * bufferHeight;
    int windowSize = windowWidth * windowHeight;

    //TODO: P2aTask3 - Use glfwGetFramebufferSize and glfwGetWindowSize to get the frame buffer size and window size. On high resolution displays, these sizes might be different.
    
    //TODO: P2aTask2 - Use glfwGetCursorPos to get the x and y value of the cursor.
    // Note that y position has to be flipped as glfwGetCursorPos gives the cursor position relative to top left of the screen. The read location must also be multiplied by (buffer size / windowSize) for some displays.
    double x_screen;
    double y_screen;
    
    glfwGetCursorPos(window, &x_screen, &y_screen);
    
    x_screen = x_screen*(bufferSize / windowSize);
    y_screen = (windowHeight-y_screen)*(bufferSize / windowSize);

    //std::cout << "X: " << x_screen << std::endl;
    //std::cout << "Y: " << y_screen << std::endl;
    //TODO: P2aTask2 - glGetIntegerv(GL_VIEWPORT, viewport) to get the viewport

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    //TODO: P2Task3 - use glm::unProject(screenPos, viewMatrix, projectionMatrix, vp) to obtaint the world position.
    // Note that vp is viewport converted to vec4; screenPos is vec3(x_screen, y_screen, 0)
    glm::vec3 worldPos = glm::unProject(glm::vec3(x_screen, y_screen, 0), viewMatrix, projectionMatrix, glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]));

    //glm::vec3 maxX = glm::unProject(glm::vec3(windowWidth, y_screen, 0), viewMatrix, projectionMatrix, glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]));

    if (editingZ) {
        float temp = worldPos.x;
        worldPos.x = worldPos.z;
        worldPos.z = (temp);
    }
    //printf("\X_screen: %f\n", x_screen);

    printf("\nWorldPos: %f, %f, %f\n", worldPos.x, worldPos.y, worldPos.z);

    return worldPos;
}
