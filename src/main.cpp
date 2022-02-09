#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "freesolitaire/stdafx.h"
#include "freesolitaire/tex_mgr.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 640;

using FreeSolitaire::TextureMgr;
using std::cerr;
using std::cout;
using std::endl;

GLFWwindow* window;

int cursorX, cursorY;

TextureMgr* textureMgr;

void onError(int code, const char* description) {
    cerr << "GLFW Error code " << code << ": " << description << endl;
}

void onMouseBtn(GLFWwindow* window, int btn, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (btn == GLFW_MOUSE_BUTTON_LEFT) {
        }
    }
}

void onCursorPos(GLFWwindow* window, double xpos, double ypos) {
    cursorX = (int)std::floor(xpos);
    cursorY = (int)std::floor(ypos);
}

void onResize(GLFWwindow* window, int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    textureMgr = new TextureMgr();
    textureMgr->loadTexture(L"res/background.png", GL_LINEAR);
}

void update() {
}

void renderBackground() {
    textureMgr->bindTexture(L"res/background.png");
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2i(0, 0);
    glTexCoord2f(0, 1);
    glVertex2i(0, WINDOW_HEIGHT);
    glTexCoord2f(1, 1);
    glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
    glTexCoord2f(1, 0);
    glVertex2i(WINDOW_WIDTH, 0);
    glEnd();
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    renderBackground();
    glfwSwapBuffers(window);
}

void cleanup() {
    delete textureMgr;
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {
    glfwSetErrorCallback(onError);
    if (!glfwInit()) {
        MessageBox(nullptr, L"Unable to initialze GLFW", L"Error", MB_OK);
        return 0;
    }
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "FreeSolitaire", nullptr, nullptr);
    if (window == nullptr) {
        MessageBox(nullptr, L"Failed to create the GLFW window", L"Error", MB_OK);
        return 0;
    }

    glfwSetMouseButtonCallback(window, onMouseBtn);
    glfwSetCursorPosCallback(window, onCursorPos);

    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (vidmode != nullptr) {
        glfwSetWindowPos(window, (vidmode->width - WINDOW_WIDTH) >> 1, (vidmode->height - WINDOW_HEIGHT) >> 1);
    }

    glfwMakeContextCurrent(window);
    onResize(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    initGL();

    glfwShowWindow(window);
    while (!glfwWindowShouldClose(window)) {
        update();
        render();
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
