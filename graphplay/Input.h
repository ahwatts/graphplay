// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_INPUT_H_
#define _GRAPHPLAY_GRAPHPLAY_INPUT_H_

#include "graphplay.h"

// #include <vector>
#include <boost/circular_buffer.hpp>

struct GLFWwindow;

namespace graphplay {
    struct InputState {
        InputState();

        double mouse_x;
        double mouse_y;
        bool rotating;
        bool escape_key;
    };

    class Input {
    public:
        Input();
        Input(unsigned int num_states);

        void initCallbacks(GLFWwindow *window) const;

        unsigned int numStates() const;

        const InputState& currentState() const;
        const InputState& prevState() const;
        const InputState& prevState(unsigned int n) const;

        void cloneCurrentState();

        void handleKeypress(GLFWwindow *window, int key, int scancode, int action, int mods);
        void handleMouseClick(GLFWwindow *window, int button, int action, int mods);
        void handleMouseScroll(GLFWwindow *window, double xoffset, double yoffset);
        void handleMouseMove(GLFWwindow *window, double xpos, double ypos);

        static void keypress(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void mouse_click(GLFWwindow *window, int button, int action, int mods);
        static void mouse_scroll(GLFWwindow *window, double xoffset, double yoffset);
        static void mouse_move(GLFWwindow *window, double xpos, double ypos);

    private:
        // std::vector<InputState> m_states;
        // unsigned int m_current_index;

        boost::circular_buffer<InputState> m_states;
    };

    extern Input GP_INPUT;
}

#endif
