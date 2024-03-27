#include <iostream>
#include <GLFW/glfw3.h>

#pragma comment(lib, "opengl32.lib")

void ErrorCallback(int error, const char* description)
{
    std::cerr << "GLFW error: " << description << std::endl;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        std::cout << key << " ÀÔ·ÂµÊ." << std::endl;
        glfwSetWindowShouldClose(window, 1);
    }
}

void MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glClearColor(0.f, 1.f, 0.f, 1.f);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glClearColor(1.f, 0.f, 0.f, 1.f);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glClearColor(0.f, 0.f, 1.f, 1.f);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glClearColor(1.f, 0.f, 1.f, 1.f);
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 768, "Ghwa-jae Engine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // glfwSetWindowMonitor(window, monitor, 0, 0, 1280, 768, 1);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}