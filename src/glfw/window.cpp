#include "window.hpp"
#include <scluk/exception.hpp>
#include <source_location>

#ifdef USE_OPENGL
extern void glViewport(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h);
#endif

namespace glfw {
    inline auto throw_on_error(auto res, const char* what, std::source_location l = std::source_location::current()) {
        if(!res) throw scluk::runtime_error("%:%, %\n\t%", l.file_name(), l.line(), l.function_name(), what);
        return res;
    }

    std::int64_t window::window_count = 0;
    
    void window::create(glm::ivec2 res, const std::string& title) {
        if(m_window_ptr)
            this->destroy();
        //Initialize the library 
        if(!window_count)
            throw_on_error(glfwInit(), "Unable to initialize glfwInit");
    
        /* Create a windowed mode window and its OpenGL context */
        GLFWmonitor* monitor = throw_on_error(glfwGetPrimaryMonitor(), "Unable to find primary monitor");
        const GLFWvidmode* mode = throw_on_error(glfwGetVideoMode(monitor), "Unable to get video mode");
        
        glfwWindowHint(GLFW_RED_BITS,     mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS,   mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS,    mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        if(res.x < 0 || res.y < 0)
            m_window_ptr = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
        else
            m_window_ptr = glfwCreateWindow(res.x, res.y, title.c_str(), nullptr, nullptr);

        throw_on_error(m_window_ptr, "Unable to create GLFW3 window\n");

        #ifdef USE_OPENGL
        glfwSetFramebufferSizeCallback(m_window_ptr, [](window_t window, int W, int H) {
            glfwMakeContextCurrent(window);
            glViewport(0, 0, W, H); 
        });
        #endif
        glfwMakeContextCurrent(m_window_ptr);

        window::window_count++;
    }

    void window::destroy() {
        glfwDestroyWindow(m_window_ptr);
        window::window_count--;

        //if no windows are left terminate glfw
        if(!window::window_count)
            glfwTerminate();
        
        m_window_ptr = nullptr;
    }
}

