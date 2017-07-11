// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "Input.h"

#include <iostream>

#include "Driver.h"
#include "opengl.h"

namespace graphplay {
    InputState::InputState()
        : mouse_x{0.0},
          mouse_y{0.0},
          rotating{false},
          escape_key{false}
    {}

    Input::Input()
        : Input(2)
    {}

    Input::Input(unsigned int num_states)
        : m_states{num_states, InputState{}}
    {}

    unsigned int Input::numStates() const {
        return static_cast<unsigned int>(m_states.size());
    }

    const InputState& Input::currentState() const {
        return m_states.front();
    }

    const InputState& Input::prevState() const {
        return prevState(1);
    }

    const InputState& Input::prevState(unsigned int n) const {
        if (m_states.size() > n) {
            return m_states[n];
        } else {
            return m_states.back();
        }
    }

    void Input::cloneCurrentState() {
        m_states.push_front(currentState());
    }

    void Input::initCallbacks(GLFWwindow *window) const {
        glfwSetKeyCallback(window, Input::keypress);
        glfwSetCursorPosCallback(window, Input::mouse_move);
        glfwSetMouseButtonCallback(window, Input::mouse_click);
        glfwSetScrollCallback(window, Input::mouse_scroll);
    }

    void Input::handleKeypress(GLFWwindow *window, int key, int scancode, int action, int mods) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS || action == GLFW_REPEAT) {
                m_states.front().escape_key = true;
            } else {
                m_states.front().escape_key = false;
            }
            std::cout << "escape key = " << currentState().escape_key << std::endl;
            break;
        default:
            std::cout << "key: " << key
                      << " scancode: " << scancode
                      << " action: " << action
                      << " mods: " << mods
                      << std::endl;
        }
    }

    void Input::handleMouseClick(GLFWwindow *window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                m_states.front().rotating = true;
            } else if (action == GLFW_RELEASE) {
                m_states.front().rotating = false;
            }
        }
    }

    void Input::handleMouseMove(GLFWwindow *window, double xpos, double ypos) {
        m_states.front().mouse_x = xpos;
        m_states.front().mouse_y = ypos;
    }

    void Input::handleMouseScroll(GLFWwindow *window, double xoffset, double yoffset) {
        // SCENE.getCamera().zoom(yoffset);
        std::cout << "mouse scroll: xoffset = " << xoffset << " yoffset = " << yoffset << std::endl;
    }

    void Input::keypress(GLFWwindow *window, int key, int scancode, int action, int mods) {
        Context *ctx = (Context *)glfwGetWindowUserPointer(window);
        if (ctx) {
            ctx->input->handleKeypress(window, key, scancode, action, mods);
        } else {
            std::cerr << "No input handler for keypress:"
                      << " key = " << key
                      << " scancode = " << scancode
                      << " action = " << action
                      << " mods = " << mods
                      << std::endl;
        }
    }

    void Input::mouse_click(GLFWwindow *window, int button, int action, int mods) {
        Context *ctx = (Context *)glfwGetWindowUserPointer(window);
        if (ctx) {
            ctx->input->handleMouseClick(window, button, action, mods);
        } else {
            std::cerr << "No input handler for mouse click:"
                      << " button = " << button
                      << " action = " << action
                      << " mods = " << mods
                      << std::endl;
        }
    }

    void Input::mouse_move(GLFWwindow *window, double xpos, double ypos) {
        Context *ctx = (Context *)glfwGetWindowUserPointer(window);
        if (ctx) {
            ctx->input->handleMouseMove(window, xpos, ypos);
        } else {
            std::cerr << "No input handler for mouse move:"
                      << " xpos = " << xpos
                      << " ypos = " << ypos
                      << std::endl;
        }
    }

    void Input::mouse_scroll(GLFWwindow *window, double xoffset, double yoffset) {
        Context *ctx = (Context *)glfwGetWindowUserPointer(window);
        if (ctx) {
            ctx->input->handleMouseScroll(window, xoffset, yoffset);
        } else {
            std::cerr << "No input handler for mouse scroll:"
                      << " xoffset = " << xoffset
                      << " yoffset = " << yoffset
                      << std::endl;
        }
    }
}
