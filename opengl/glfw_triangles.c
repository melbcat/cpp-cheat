/*
Reuse vertices on multiple triangles via vertex indices.
*/

#include <stdio.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "common.h"

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;
/* ourColor is passed on to the fragment shader. */
static const GLchar *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 color;\n"
    "out vec3 ourColor;\n"
    "void main() {\n"
    "    gl_Position = vec4(position, 1.0f);\n"
    "    ourColor = color;\n"
    "}\n";
static const GLchar *fragment_shader_source =
    "#version 330 core\n"
    "in vec3 ourColor;\n"
    "out vec4 color;\n"
    "void main() {\n"
    "    color = vec4(ourColor, 1.0f);\n"
    "}\n";
static const GLfloat vertices[] = {
/*   Positions            Colors */
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
};
static const GLuint indices[] = {
    /* First triangle. */
    0, 1, 3,
    /* Second triangle. */
    1, 2, 3,
};

int main(void) {
    GLint shader_program;

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, __FILE__, NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    shader_program = common_get_shader_program(vertex_shader_source, fragment_shader_source);

    GLuint vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    /*
    Position attribute.
    - 3: length
    - 6: how many bytes to skip until next one == 3 (position) + 3 (color)
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    /* Color attribute */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glfwTerminate();
    return EXIT_SUCCESS;
}
