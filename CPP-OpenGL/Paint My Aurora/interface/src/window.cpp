//
//  window.cpp
//  Paint My Aurora
//
//  Created by Pujun Lun on 4/15/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <iostream>
#include <stdexcept>
#include <glm/glm.hpp>

#include "drawpath.hpp"
#include "window.hpp"

using std::runtime_error;
using glm::vec2;

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;

glm::vec2 originalSize, currentSize, retinaRatio;
glm::vec2 posOffset, clickNDC;

DrawPath *pathEditor;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    currentSize = vec2(width, height);
    // keep earth in center after resizing window
    glViewport((currentSize.x - originalSize.x) * 0.5f,
               (currentSize.y - originalSize.y) * 0.5f,
               originalSize.x, originalSize.y);
    posOffset = (originalSize - currentSize) * 0.5f;
}

void mouseClickCallback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT)
        pathEditor->didClickMouse(button == GLFW_MOUSE_BUTTON_LEFT, action == GLFW_PRESS);
}

void mouseScrollCallback(GLFWwindow *window, double xPos, double yPos) {
    pathEditor->didScrollMouse(yPos);
}

const glm::vec2& Window::getOriginalSize() {
    return originalSize;
}

const glm::vec2& Window::getClickNDC() {
    return clickNDC;
}

void Window::updateMousePos() {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    vec2 clickScreen = (vec2(xPos, yPos) * retinaRatio + posOffset) / originalSize; // [0.0, 1.0]
    clickNDC = clickScreen * 2.0f - 1.0f; // [-1.0, 1.0]
    clickNDC.y *= -1.0f; // flip y coordinate
}

void Window::processKeyboardInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            pathEditor->didPressUpOrDown(true);
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            pathEditor->didPressUpOrDown(false);
        
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
            pathEditor->didPressNumber(0);
        else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            pathEditor->didPressNumber(1);
    }
}

Window::Window(DrawPath *drawPath) {
    pathEditor = drawPath;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // ------------------------------------
    // window
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Paint My Aurora", NULL, NULL);
    if (window == NULL) throw runtime_error("Failed to create window");
    
    glfwMakeContextCurrent(window);
    // called when window is resized by the user
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    
    // ------------------------------------
    // GLAD (function pointer loader)
    // do this after context is created, and before calling any OpenGL function!
    
    if (!gladLoadGL()) throw runtime_error("Failed to init GLAD");
    
    // screen size is different from the input width and height on retina screen
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height); // specify render area
    originalSize = currentSize = vec2(width, height);
    retinaRatio = originalSize / vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Window::renderFrame() {
    glfwSwapBuffers(window); // use color buffer to draw
    glfwPollEvents(); // check events (keyboard, mouse, ...)
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}
