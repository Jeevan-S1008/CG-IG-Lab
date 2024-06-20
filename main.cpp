#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const GLfloat BALL_RADIUS = 0.05f;

class Ball {
public:
    Ball(GLfloat radius, GLfloat xPos, GLfloat yPos, GLfloat xVelocity, GLfloat yVelocity)
        : radius(radius), xPos(xPos), yPos(yPos), xVelocity(xVelocity), yVelocity(yVelocity),
          scale(1.0f), rotation(0.0f) {}

    void draw() {
        glPushMatrix();
        glTranslatef(xPos, yPos, 0.0f);
        glScalef(scale, scale, 1.0f);
        glRotatef(rotation, 0.0f, 0.0f, 1.0f);

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 360; i++) {
            float theta = i * 3.14159f / 180.0f;
            glVertex2f(radius * cos(theta), radius * sin(theta));
        }
        glEnd();

        glPopMatrix();
    }

    void update(GLfloat deltaTime) {
        // Update position based on velocity
        xPos += xVelocity * deltaTime;
        yPos += yVelocity * deltaTime;

        // Collision detection with window boundaries
        if (xPos + radius * scale >= 1.0f || xPos - radius * scale <= -1.0f) {
            xVelocity = -xVelocity;
        }
        if (yPos + radius * scale >= 1.0f || yPos - radius * scale <= -1.0f) {
            yVelocity = -yVelocity;
        }
    }

    void scaleUp() {
        scale += 0.1f;
    }

    void scaleDown() {
        if (scale > 0.1f) {
            scale -= 0.1f;
        }
    }

    void rotateLeft() {
        rotation += 10.0f;
    }

    void rotateRight() {
        rotation -= 10.0f;
    }

    void setPosition(float x, float y) {
        xPos = x;
        yPos = y;
    }

private:
    GLfloat radius, xPos, yPos, xVelocity, yVelocity, scale, rotation;
};

Ball ball(BALL_RADIUS, 0.0f, 0.0f, 0.2f, 0.2f);

void handleInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        ball.scaleUp();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        ball.scaleDown();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        ball.rotateLeft();
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        ball.rotateRight();
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Convert screen coordinates to OpenGL coordinates
        xpos = (xpos / WIDTH) * 2 - 1;
        ypos = 1 - (ypos / HEIGHT) * 2;

        ball.setPosition(xpos, ypos);
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT);
    ball.draw();
}

void checkOpenGLError() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return EXIT_FAILURE;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Bouncing Ball", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW initialization failed: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // Check OpenGL version
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    // Set viewport size
    glViewport(0, 0, WIDTH, HEIGHT);

    // Set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Set up orthographic projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Time tracking
    GLfloat lastTime = glfwGetTime();

    // Set mouse button callback
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        GLfloat currentTime = glfwGetTime();
        GLfloat deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glfwPollEvents();
        handleInput(window);

        // Rendering
        renderScene();
        checkOpenGLError();  // Check for errors after rendering
        ball.update(deltaTime);

        glfwSwapBuffers(window);
    }

    // Clean up
    glfwTerminate();

    return EXIT_SUCCESS;
}
