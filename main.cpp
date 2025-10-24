#pragma comment(lib, "legacy_stdio_definitions")
#include "framework.h"
#include "scene.h"
#include "globals.h"

// Variables
Scene scene;
bool mouseDown = false;

// Forward declarations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main(int, char**) {
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
        });

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Marching Cubes", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // VSync: 1 = ON, 0 = OFF

    // Set viewport and OpenGL options
    int display_width, display_height;
    glfwGetFramebufferSize(window, &display_width, &display_height);
    glViewport(0, 0, display_width, display_height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set GLFW callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        return -1;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");

    // Build the scene
    scene.Build();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render
        glClearColor(CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {                                     
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS)   KEYDOWN_W = true;
        if (action == GLFW_RELEASE) KEYDOWN_W = false;
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS)   KEYDOWN_S = true;
        if (action == GLFW_RELEASE) KEYDOWN_S = false;
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS)   KEYDOWN_A = true;
        if (action == GLFW_RELEASE) KEYDOWN_A = false;
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS)   KEYDOWN_D = true;
        if (action == GLFW_RELEASE) KEYDOWN_D = false;
    }
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS)   KEYDOWN_SPACE = true;
        if (action == GLFW_RELEASE) KEYDOWN_SPACE = false;
    }
    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS)   KEYDOWN_SHIFT = true;
        if (action == GLFW_RELEASE) KEYDOWN_SHIFT = false;
    }
    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS)   KEYDOWN_SHIFT = true;
        if (action == GLFW_RELEASE) KEYDOWN_SHIFT = false;
    }
    if (key == GLFW_KEY_LEFT_CONTROL) {
        if (action == GLFW_PRESS)   KEYDOWN_CTRL = true;
        if (action == GLFW_RELEASE) KEYDOWN_CTRL = false;
    }
    if (key == GLFW_KEY_X) {
        if (action == GLFW_PRESS)   KEYDOWN_X = true;
        if (action == GLFW_RELEASE) KEYDOWN_X = false;
    }
    if (key == GLFW_KEY_E) {
        if (action == GLFW_PRESS)   KEYDOWN_E = true;
        if (action == GLFW_RELEASE) KEYDOWN_E = false;
    }
    if (key == GLFW_KEY_Q) {
        if (action == GLFW_PRESS)   KEYDOWN_Q = true;
        if (action == GLFW_RELEASE) KEYDOWN_Q = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    if (mouseDown) scene.rotateCamera(x, y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        scene.setCameraFirstMouse();
        mouseDown = true;
    } else {
        mouseDown = false;
    }
}
