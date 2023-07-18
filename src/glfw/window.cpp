#include "window.hpp"
#include <scluk/exception.hpp>
#include <source_location>
#include <GLFW/glfw3.h>

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


	/*
	//taken care of by glad's generator, only causes problems
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	*/

        if(res.x < 0 || res.y < 0)
            m_window_ptr = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
        else
            m_window_ptr = glfwCreateWindow(res.x, res.y, title.c_str(), nullptr, nullptr);

        throw_on_error(m_window_ptr, "Unable to create GLFW3 window\n");

        /*glfwSetFramebufferSizeCallback(m_window_ptr, [](window_t window, int W, int H) {
            glfwMakeContextCurrent(window);
            glViewport(0, 0, W, H); 
        });*/

        glfwMakeContextCurrent(m_window_ptr);
        glfwSwapInterval(0); // Disable vsync: not locking fps gives us better perspective on the performance of the application during development

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

    bool window::should_close() { return glfwWindowShouldClose(m_window_ptr); }

    void window::swap_buffers() { glfwSwapBuffers(m_window_ptr); }

    void window::make_ctx_current() { glfwMakeContextCurrent(m_window_ptr); }

    bool window::get_key(int key) { return glfwGetKey(m_window_ptr, key) != GLFW_RELEASE; }

    bool window::get_mouse_btn(int btn) { return glfwGetMouseButton(m_window_ptr, btn) != GLFW_RELEASE; }

    void window::set_resize_cb(GLFWwindowsizefun f) { glfwSetWindowSizeCallback(m_window_ptr, f); }

    void window::set_key_cb(GLFWkeyfun f) { glfwSetKeyCallback(m_window_ptr, f); }

    void window::poll_events() { glfwPollEvents(); }

    glm::dvec2 window::get_cursor_pos() {
        glm::dvec2 ret;
        glfwGetCursorPos(m_window_ptr, &ret.x, &ret.y);
        return ret;
    }

    glm::ivec2 window::get_framebuf_sz() {
        glm::ivec2 ret;
        glfwGetFramebufferSize(m_window_ptr, &ret.x, &ret.y);
        return ret;
    }
}

