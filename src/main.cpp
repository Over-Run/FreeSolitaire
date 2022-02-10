#define GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "freesolitaire/stdafx.h"
#include "freesolitaire/tex_mgr.h"
#include "freesolitaire/atlas.h"
#include "freesolitaire/card.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 640;

using FreeSolitaire::Card;
using FreeSolitaire::Sprite;
using FreeSolitaire::SpriteAtlasTexture;
using FreeSolitaire::Suit;
using FreeSolitaire::TextureMgr;
using std::array;
using std::cerr;
using std::cout;
using std::endl;
using std::stack;

GLFWwindow* window;

int cursorX, cursorY;

TextureMgr* textureMgr;
SpriteAtlasTexture* pokers;
array<stack<Card>, 13> slots;

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
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    textureMgr = new TextureMgr();
    textureMgr->loadTexture(L"res/background.png", GL_NEAREST);
    textureMgr->loadTexture(L"res/pokers.png", GL_NEAREST);

    array<Sprite, 53> sprites;
    for (int i = 0; i < 53; ++i) {
        int x = (i % 11) * 108;
        int y = (i / 11) * 144;
        char nm[4] = "\0";
        Card::getStrName(i == 0, (Suit)static_cast<signed char>(i / 13 + 1), static_cast<signed char>((i % 13) + 1), nm);
        sprites[i] = {
            x, y, 108, 144,
            x / 1188.0f, y / 720.0f, (x + 108) / 1188.0f, (y + 144) / 720.0f,
            nm
        };
    }
    pokers = new SpriteAtlasTexture(sprites.data(), sprites.size(), textureMgr->getTexture(L"res/pokers.png"));
}

void init() {
    slots[0].push({ true, Suit::DIAMONDS, 1 });
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

void renderCards() {
    textureMgr->bindTexture(pokers->getGlId());
    slots[0].top().render();
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    renderBackground();
    renderCards();
    glfwSwapBuffers(window);
}

void cleanup() {
    delete textureMgr;
    delete pokers;
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
    init();

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
