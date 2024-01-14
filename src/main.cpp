
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Pixel {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;

    Pixel(std::uint32_t color) {
        r = (std::uint8_t)(color >> 24);
        g = (std::uint8_t)(color >> 16);
        b = (std::uint8_t)(color >> 8);
        a = (std::uint8_t)(color);
    }
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {}
};

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

static void GlfwErrorCallback(int error_code, const char *description) {
    spdlog::error("{}: {}", error_code, description);
}

static void GlfwFramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    int start;
    int end;
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[]) {
    spdlog::info("{}", argv[0]);

    if (!glfwInit()) {
        spdlog::critical("Failed to initialiaze GLFW");
        return -1;
    }

    glfwSetErrorCallback(GlfwErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    auto window = glfwCreateWindow(800, 600, "Software Rasterizer", NULL, NULL);
    if (!window) {
        spdlog::critical("Failed to create window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(window, GlfwFramebufferSizeCallback);

    GLuint screen_mesh_vao;
    glGenVertexArrays(1, &screen_mesh_vao);

    // Compile the screen shader
    GLuint screen_shader_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(screen_shader_vert, 1, &screen_shader_vert_source, 0);
    glCompileShader(screen_shader_vert);

    GLuint screen_shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(screen_shader_frag, 1, &screen_shader_frag_source, 0);
    glCompileShader(screen_shader_frag);

    GLuint screen_shader_program = glCreateProgram();
    glAttachShader(screen_shader_program, screen_shader_vert);
    glAttachShader(screen_shader_program, screen_shader_frag);
    glLinkProgram(screen_shader_program);
    GLint screen_shader_program_success;
    glGetProgramiv(screen_shader_program, GL_LINK_STATUS, &screen_shader_program_success);
    if (!screen_shader_program_success) {
        char buf[4096];
        glGetProgramInfoLog(screen_shader_program, 4096, 0, buf);
        spdlog::error("Failed to link shader program:\n {}", buf);
    }

    glDetachShader(screen_shader_program, screen_shader_vert);
    glDetachShader(screen_shader_program, screen_shader_frag);

    // TODO: regenerate on drawing
    auto pixels = std::vector<Pixel>(160 * 144, 0x00000FF);

    for (size_t x = 0; x < 160; x++) {
        for (size_t y = 0; y < 144; y++) {
            size_t index = x + y * 160;
            pixels[index].r = x / 160.0f * 255;
            pixels[index].g = y / 144.0f * 255;
        }
    }

    GLuint screen_texture;
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 160, 144, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(screen_texture);

    glfwSwapInterval(1);
    float last_time = 0, current_time = 0, total_time = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(screen_shader_program);
        glBindTexture(GL_TEXTURE_2D, screen_texture);
        glBindVertexArray(screen_mesh_vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
    }

    glDeleteProgram(screen_shader_program);
    glDeleteShader(screen_shader_frag);
    glDeleteShader(screen_shader_vert);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &screen_mesh_vao);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}