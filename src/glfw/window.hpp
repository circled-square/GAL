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
#include <tuple>
#include <glad/glad.h> // this MUST be included before glfw, so this file will include it here even though it does not need to
#include <GLFW/glfw3.h>
#include <scluk/functional.hpp>
#include <glm/glm.hpp>

namespace glfw {
    typedef struct GLFWwindow GLFWwindow;
    using window_t = GLFWwindow*;
    class window {
        static std::int64_t window_count;
        window_t m_window_ptr;
    public:
        void create(glm::ivec2 res, const std::string& title);
        void destroy();
        bool should_close();
        void swap_buffers();
        void make_ctx_current();
        bool get_key(int key);
        bool get_mouse_btn(int btn);

        glm::dvec2 get_cursor_pos();
        
        glm::ivec2 get_framebuf_sz();

        void set_resize_cb (void (*f)(window_t, int, int));
        void set_key_cb (void (*f)(window_t, int, int, int, int));
        void set_key_cb (auto f) {
            set_key_cb(scluk::lambda_to_fnptr<void(window_t,int,int,int,int)>(std::move(f)));
        }
        
        static void  poll_events ();
        
        //constructors/destructors that just resort to calling the previous functions + default constructor
        window () : m_window_ptr(nullptr) {}
        window (glm::ivec2 res, const std::string& title) : window() { this->create(res, title); }
        window (int w, int h, const std::string& title) : window({ w, h }, title) {}
        ~window () { this->destroy(); }
        
        //access and conversion of the internal GLFWwindow*
        operator void*    () { return  m_window_ptr; }
        operator window_t () { return  m_window_ptr; }
        operator bool     () { return  m_window_ptr; }
        bool operator!    () { return !m_window_ptr; }
    };
}
#endif //GLFW_WRAPPER_HPP

