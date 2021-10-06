#include "window.hpp"
#include <scluk/exception.hpp>
#include <source_location>

#ifdef USE_OPENGL
extern void glViewport(std::int32_t x, std::int32_t y, std::uint32_t w, std::uint32_t h);
#endif

namespace glfw {
#ifndef __clang__
    inline auto throw_on_error(auto res, const char* what, std::source_location l = std::source_location::current()) {
        if(!res) throw scluk::runtime_error("%:%, %\n\t%", l.file_name(), l.line(), l.function_name(), what);
        return res;
    }
#else
    #define throw_on_error(res, what) ([&]{ auto r = res; if(!r) throw scluk::runtime_error("%:%, %\n\t%", __FILE__, __LINE__, __func__, what); return r; }())
#endif
    std::int64_t window::window_count = 0;
    void window::create(std::pair<int,int> res, std::string title) {
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

        if(res.first < 0 || res.second < 0)
            m_window_ptr = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
        else
            m_window_ptr = glfwCreateWindow(res.first, res.second, title.c_str(), nullptr, nullptr);

        throw_on_error(m_window_ptr, "Unable to create GLFW3 window\n");
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

