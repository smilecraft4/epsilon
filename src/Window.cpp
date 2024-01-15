#include "Window.h"

#include <spdlog/spdlog.h>

static const char *screen_shader_vert_source = R"(
#version 330 core
const vec2 triangle_vertices[4] = vec2[](
    vec2(-1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0)
);
const vec2 triangle_uvs[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);
out vec2 uv;
void main() {
    gl_Position = vec4(triangle_vertices[gl_VertexID], 0.0, 1.0);
    uv = triangle_uvs[gl_VertexID];
}
)";

static const char *screen_shader_frag_source = R"(
#version 330 core
out vec4 FragColor;
in vec2 uv;
uniform sampler2D canvas;
void main() {
    FragColor = vec4(texture(canvas, uv).xyz, 1.0);
}
)";

Window::Window(int width, int height, std::string name) : width_(width), height_(height), name_(std::move(name)) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfw_window_ = glfwCreateWindow(800, 600, "Software Rasterizer", NULL, NULL);
    if (!glfw_window_) {
        spdlog::critical("Failed to create window");
        glfwTerminate();
    }

    glfwSetWindowUserPointer(glfw_window_, this);
    glfwSetKeyCallback(glfw_window_, Window::GlfwKeyCallback);
    glfwSetDropCallback(glfw_window_, Window::GlfwDropCallback);
    glfwSetScrollCallback(glfw_window_, Window::GlfwScrollCallback);
    glfwSetCursorPosCallback(glfw_window_, Window::GlfwCursorPosCallback);
    glfwSetCursorEnterCallback(glfw_window_, Window::GlfwCursorEnterCallback);
    glfwSetWindowPosCallback(glfw_window_, Window::GlfwWindowPosCallback);
    glfwSetWindowSizeCallback(glfw_window_, Window::GlfwWindowSizeCallback);
    glfwSetMouseButtonCallback(glfw_window_, Window::GlfwMouseButtonCallback);
    glfwSetWindowCloseCallback(glfw_window_, Window::GlfwWindowCloseCallback);
    glfwSetWindowFocusCallback(glfw_window_, Window::GlfwWindowFocusCallback);
    glfwSetWindowRefreshCallback(glfw_window_, Window::GlfwWindowRefreshCallback);
    glfwSetFramebufferSizeCallback(glfw_window_, Window::GlfwFramebufferSizeCallback);

    // FIXME: redondant for more than one window
    glfwMakeContextCurrent(glfw_window_);
    glfwSwapInterval(1);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Dummy vao to allow drawing the screen_quad/triangle
    glGenVertexArrays(1, &screen_vao_);

    // Compile the screen shader
    GLuint screen_shader_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(screen_shader_vert, 1, &screen_shader_vert_source, 0);
    glCompileShader(screen_shader_vert);

    GLuint screen_shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(screen_shader_frag, 1, &screen_shader_frag_source, 0);
    glCompileShader(screen_shader_frag);

    screen_program_ = glCreateProgram();
    glAttachShader(screen_program_, screen_shader_vert);
    glAttachShader(screen_program_, screen_shader_frag);
    glLinkProgram(screen_program_);
    GLint screen_shader_program_success;
    glGetProgramiv(screen_program_, GL_LINK_STATUS, &screen_shader_program_success);
    if (!screen_shader_program_success) {
        char buf[4096];
        glGetProgramInfoLog(screen_program_, 4096, 0, buf);
        spdlog::error("Failed to link shader program:\n {}", buf);
    }

    glDetachShader(screen_program_, screen_shader_vert);
    glDetachShader(screen_program_, screen_shader_frag);

    glDeleteShader(screen_shader_vert);
    glDeleteShader(screen_shader_frag);
}

void Window::RenderScreen(const Screen *screen) {
    // Create the texture to be rendered
    glDeleteTextures(1, &screen_texture_);
    glGenTextures(1, &screen_texture_);
    glBindTexture(GL_TEXTURE_2D, screen_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture_sampling_mode_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture_sampling_mode_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen->Width(), screen->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 screen->Pixels().data());
    glGenerateMipmap(screen_texture_);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(screen_program_);
    glBindTexture(GL_TEXTURE_2D, screen_texture_);
    glBindVertexArray(screen_vao_);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(glfw_window_);
}

void Window::Cleanup() {

    glBindVertexArray(0);
    glUseProgram(0);

    glDeleteTextures(1, &screen_texture_);
    glDeleteProgram(screen_program_);
    glDeleteVertexArrays(1, &screen_vao_);

    glfwDestroyWindow(glfw_window_);
}

void Window::SetTitle(const std::string &title) {
    name_ = title;
    glfwSetWindowTitle(glfw_window_, name_.c_str());
}

bool Window::Running() const { return !glfwWindowShouldClose(glfw_window_); }

void Window::Init(int argc, char **argv) {

    spdlog::info("{}", argv[0]);

    if (!glfwInit()) {
        spdlog::critical("Failed to initialiaze GLFW");
    }

    glfwSetErrorCallback(GlfwErrorCallback);
}

void Window::Terminate() { glfwTerminate(); }

void Window::PollEvents() { glfwPollEvents(); }

void Window::GlfwKeyCallback(GLFWwindow *glfw_window, int key, int scancode, int action, int mods) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (window->texture_sampling_mode_ == GL_NEAREST) {
            window->texture_sampling_mode_ = GL_LINEAR;
        } else {
            window->texture_sampling_mode_ = GL_NEAREST;
        }
    }
}
void Window::GlfwDropCallback(GLFWwindow *glfw_window, int path_count, const char *paths[]) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwScrollCallback(GLFWwindow *glfw_window, double xoffset, double yoffset) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwCursorPosCallback(GLFWwindow *glfw_window, double xpos, double ypos) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwCursorEnterCallback(GLFWwindow *glfw_window, int entered) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwWindowPosCallback(GLFWwindow *glfw_window, int xpos, int ypos) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwWindowSizeCallback(GLFWwindow *glfw_window, int width, int height) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwMouseButtonCallback(GLFWwindow *glfw_window, int button, int action, int mods) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwWindowCloseCallback(GLFWwindow *glfw_window) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwWindowFocusCallback(GLFWwindow *glfw_window, int focused) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwWindowRefreshCallback(GLFWwindow *glfw_window) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
}
void Window::GlfwFramebufferSizeCallback(GLFWwindow *glfw_window, int width, int height) {
    auto window = (Window *)glfwGetWindowUserPointer(glfw_window);
    window->width_ = width;
    window->height_ = height;
    glViewport(0, 0, width, height);
}

void Window::GlfwErrorCallback(int error_code, const char *description) {
    spdlog::error("{}: {}", error_code, description);
}