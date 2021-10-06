#ifndef GLFW_WRAPPER_HPP
#define GLFW_WRAPPER_HPP
/*
glfw::window, a RAII and OO wrapper for glfw windows, thread unsafe because glfw is (unfortunately).
Construction and destruction may only be invoked in the main thread but some other functions *might* be 
callable from other threads. 
*/
#include <string>
#include <array>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <scluk/functional.hpp>

namespace glfw {
    using window_t = GLFWwindow*;
    class window {
        static std::int64_t window_count;
        window_t m_window_ptr;
    public:
        void create(std::pair<int,int> res, std::string title);
        void destroy();
        bool should_close     () { return glfwWindowShouldClose(m_window_ptr); }
        void swap_buffers     () { glfwSwapBuffers(m_window_ptr); }
        void make_ctx_current () { glfwMakeContextCurrent(m_window_ptr); }
        bool get_key          (int key) { return glfwGetKey        (m_window_ptr, key) != GLFW_RELEASE; }
        bool get_mouse_btn    (int btn) { return glfwGetMouseButton(m_window_ptr, btn) != GLFW_RELEASE; }
        void get_cursor_xy    (double *x, double *y) { glfwGetCursorPos(m_window_ptr, x, y); }
        void get_framebuf_sz  (int *w, int *h) { glfwGetFramebufferSize(m_window_ptr, w, h); }

        #ifdef GLM_VERSION
        glm::dvec2 get_cursor_pos() {
            glm::dvec2 ret;
            glfwGetCursorPos(m_window_ptr, &ret.x, &ret.y);
            return ret;
        }
        
        glm::ivec2 get_framebuf_sz() {
            glm::ivec2 ret;
            glfwGetFramebufferSize(m_window_ptr, &ret.x, &ret.y);
            return ret;
        }
        #endif

        void set_resize_cb (GLFWwindowsizefun f) { glfwSetWindowSizeCallback(m_window_ptr, f); }
        void set_key_cb (GLFWkeyfun f) { glfwSetKeyCallback(m_window_ptr, f); }
        void set_key_cb (auto f) {
            set_key_cb(scluk::lambda_to_fnptr<void(window_t,int,int,int,int)>(f));
        }
        
        static void  poll_events () { glfwPollEvents(); }
        
        //constructors/destructors that just resort to calling the previous functions + default constructor
        window () : m_window_ptr(nullptr) {}
        window (std::pair<int,int> res, std::string title) :window() { this->create(res,std::move(title)); }
        window (int w, int h, std::string title) : window({ w, h }, std::move(title) ) {}
        ~window () { this->destroy(); }
        
        //access and conversion of the internal GLFWwindow*
        operator void*    () { return  m_window_ptr; }
        operator window_t () { return  m_window_ptr; }
        operator bool     () { return  m_window_ptr; }
        bool operator!    () { return !m_window_ptr; }
    };
}
#endif //GLFW_WRAPPER_HPP

