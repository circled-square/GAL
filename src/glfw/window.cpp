#include "window.hpp"
#include <stdexcept>

#ifdef USE_OPENGL
extern void glViewport(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h);
#endif

namespace glfw {
    std::int64_t window::window_count = 0;
    void window::create(std::pair<int,int> res, std::string title) {
        if(m_window_ptr)
            this->destroy();
        //Initialize the library 
        if(!window_count) {
            if(!glfwInit()) {
                throw std::runtime_error("Error initialising GLFW3\n");
            } else {
                //make sure your system has opengl 4.4 before using these hints
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            }
        }
        /* Create a windowed mode window and its OpenGL context */
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!monitor || !mode)
            throw std::runtime_error("Error in finding primary monitor (GLFW3) or associated video mode\n");
        
        glfwWindowHint(GLFW_RED_BITS,     mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS,   mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS,    mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        if(res.first < 0 || res.second < 0)
            m_window_ptr = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
        else
            m_window_ptr = glfwCreateWindow(res.first, res.second, title.c_str(), nullptr, nullptr);

        if (!m_window_ptr)
            throw std::runtime_error("Error creating GLFW3 window\n");
        #ifdef USE_OPENGL
        glfwSetFramebufferSizeCallback(m_window_ptr, [](window_t, int W, int H) { glViewport(0, 0, W, H); } );
        #endif
        glfwMakeContextCurrent(m_window_ptr);

        window::window_count++;
    }

    void window::destroy() {
        glfwDestroyWindow(m_window_ptr);
        //if no windows are left terminate glfw
        if(!--window::window_count)
            glfwTerminate();
        m_window_ptr = nullptr;
    }
}

