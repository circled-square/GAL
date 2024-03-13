#ifndef GLFW_WRAPPER_HPP
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
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace gal::application::window_glfw {
    class window {
        static std::int64_t window_count;
        GLFWwindow* m_window_ptr;
    public:
        void create(glm::ivec2 res, const std::string& title);
        void destroy();
        bool should_close();
        void swap_buffers();
        void make_ctx_current();
        bool get_key(int key);
        bool get_mouse_btn(int btn);

        glm::dvec2 get_cursor_pos();
        
        glm::ivec2 get_framebuf_size();

        void set_resize_cb (void (*f)(GLFWwindow*, int, int));
        void set_key_cb (void (*f)(GLFWwindow*, int, int, int, int));
        void set_user_ptr(void* p);
        static void* get_user_ptr(GLFWwindow* w);

        static void  poll_events ();
        
        //constructors/destructors that just resort to calling the previous functions + default constructor
        window () : m_window_ptr(nullptr) {}
        window (glm::ivec2 res, const std::string& title) : window() { this->create(res, title); }
        window (int w, int h, const std::string& title) : window({ w, h }, title) {}
        ~window () { this->destroy(); }
        
        //access and conversion of the internal GLFWwindow*
        operator GLFWwindow* () { return  m_window_ptr; }
        operator bool     () { return  m_window_ptr; }
        bool operator!    () { return !m_window_ptr; }
    };

    namespace key_codes {
        constexpr int SPACE = GLFW_KEY_SPACE;
        constexpr int APOSTROPHE = GLFW_KEY_APOSTROPHE;
        constexpr int COMMA = GLFW_KEY_COMMA;
        constexpr int MINUS = GLFW_KEY_MINUS;
        constexpr int PERIOD = GLFW_KEY_PERIOD;
        constexpr int SLASH = GLFW_KEY_SLASH;
        constexpr int _0 = GLFW_KEY_0;
        constexpr int _1 = GLFW_KEY_1;
        constexpr int _2 = GLFW_KEY_2;
        constexpr int _3 = GLFW_KEY_3;
        constexpr int _4 = GLFW_KEY_4;
        constexpr int _5 = GLFW_KEY_5;
        constexpr int _6 = GLFW_KEY_6;
        constexpr int _7 = GLFW_KEY_7;
        constexpr int _8 = GLFW_KEY_8;
        constexpr int _9 = GLFW_KEY_9;
        constexpr int SEMICOLON = GLFW_KEY_SEMICOLON;
        constexpr int EQUAL = GLFW_KEY_EQUAL;
        constexpr int A = GLFW_KEY_A;
        constexpr int B = GLFW_KEY_B;
        constexpr int C = GLFW_KEY_C;
        constexpr int D = GLFW_KEY_D;
        constexpr int E = GLFW_KEY_E;
        constexpr int F = GLFW_KEY_F;
        constexpr int G = GLFW_KEY_G;
        constexpr int H = GLFW_KEY_H;
        constexpr int I = GLFW_KEY_I;
        constexpr int J = GLFW_KEY_J;
        constexpr int K = GLFW_KEY_K;
        constexpr int L = GLFW_KEY_L;
        constexpr int M = GLFW_KEY_M;
        constexpr int N = GLFW_KEY_N;
        constexpr int O = GLFW_KEY_O;
        constexpr int P = GLFW_KEY_P;
        constexpr int Q = GLFW_KEY_Q;
        constexpr int R = GLFW_KEY_R;
        constexpr int S = GLFW_KEY_S;
        constexpr int T = GLFW_KEY_T;
        constexpr int U = GLFW_KEY_U;
        constexpr int V = GLFW_KEY_V;
        constexpr int W = GLFW_KEY_W;
        constexpr int X = GLFW_KEY_X;
        constexpr int Y = GLFW_KEY_Y;
        constexpr int Z = GLFW_KEY_Z;
        constexpr int LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;
        constexpr int BACKSLASH = GLFW_KEY_BACKSLASH;
        constexpr int RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;
        constexpr int GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;
        constexpr int WORLD_1 = GLFW_KEY_WORLD_1;
        constexpr int WORLD_2 = GLFW_KEY_WORLD_2;
        constexpr int ESCAPE = GLFW_KEY_ESCAPE;
        constexpr int ENTER = GLFW_KEY_ENTER;
        constexpr int TAB = GLFW_KEY_TAB;
        constexpr int BACKSPACE = GLFW_KEY_BACKSPACE;
        constexpr int INSERT = GLFW_KEY_INSERT;
        constexpr int DELETE = GLFW_KEY_DELETE;
        constexpr int RIGHT = GLFW_KEY_RIGHT;
        constexpr int LEFT = GLFW_KEY_LEFT;
        constexpr int DOWN = GLFW_KEY_DOWN;
        constexpr int UP = GLFW_KEY_UP;
        constexpr int PAGE_UP = GLFW_KEY_PAGE_UP;
        constexpr int PAGE_DOWN = GLFW_KEY_PAGE_DOWN;
        constexpr int HOME = GLFW_KEY_HOME;
        constexpr int END = GLFW_KEY_END;
        constexpr int CAPS_LOCK = GLFW_KEY_CAPS_LOCK;
        constexpr int SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;
        constexpr int NUM_LOCK = GLFW_KEY_NUM_LOCK;
        constexpr int PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;
        constexpr int PAUSE = GLFW_KEY_PAUSE;
        constexpr int F1 = GLFW_KEY_F1;
        constexpr int F2 = GLFW_KEY_F2;
        constexpr int F3 = GLFW_KEY_F3;
        constexpr int F4 = GLFW_KEY_F4;
        constexpr int F5 = GLFW_KEY_F5;
        constexpr int F6 = GLFW_KEY_F6;
        constexpr int F7 = GLFW_KEY_F7;
        constexpr int F8 = GLFW_KEY_F8;
        constexpr int F9 = GLFW_KEY_F9;
        constexpr int F10 = GLFW_KEY_F10;
        constexpr int F11 = GLFW_KEY_F11;
        constexpr int F12 = GLFW_KEY_F12;
        constexpr int F13 = GLFW_KEY_F13;
        constexpr int F14 = GLFW_KEY_F14;
        constexpr int F15 = GLFW_KEY_F15;
        constexpr int F16 = GLFW_KEY_F16;
        constexpr int F17 = GLFW_KEY_F17;
        constexpr int F18 = GLFW_KEY_F18;
        constexpr int F19 = GLFW_KEY_F19;
        constexpr int F20 = GLFW_KEY_F20;
        constexpr int F21 = GLFW_KEY_F21;
        constexpr int F22 = GLFW_KEY_F22;
        constexpr int F23 = GLFW_KEY_F23;
        constexpr int F24 = GLFW_KEY_F24;
        constexpr int F25 = GLFW_KEY_F25;
        constexpr int KP_0 = GLFW_KEY_KP_0;
        constexpr int KP_1 = GLFW_KEY_KP_1;
        constexpr int KP_2 = GLFW_KEY_KP_2;
        constexpr int KP_3 = GLFW_KEY_KP_3;
        constexpr int KP_4 = GLFW_KEY_KP_4;
        constexpr int KP_5 = GLFW_KEY_KP_5;
        constexpr int KP_6 = GLFW_KEY_KP_6;
        constexpr int KP_7 = GLFW_KEY_KP_7;
        constexpr int KP_8 = GLFW_KEY_KP_8;
        constexpr int KP_9 = GLFW_KEY_KP_9;
        constexpr int KP_DECIMAL = GLFW_KEY_KP_DECIMAL;
        constexpr int KP_DIVIDE = GLFW_KEY_KP_DIVIDE;
        constexpr int KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
        constexpr int KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
        constexpr int KP_ADD = GLFW_KEY_KP_ADD;
        constexpr int KP_ENTER = GLFW_KEY_KP_ENTER;
        constexpr int KP_EQUAL = GLFW_KEY_KP_EQUAL;
        constexpr int LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;
        constexpr int LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;
        constexpr int LEFT_ALT = GLFW_KEY_LEFT_ALT;
        constexpr int LEFT_SUPER = GLFW_KEY_LEFT_SUPER;
        constexpr int RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;
        constexpr int RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;
        constexpr int RIGHT_ALT = GLFW_KEY_RIGHT_ALT;
        constexpr int RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;
        constexpr int MENU = GLFW_KEY_MENU;
        constexpr int LAST = GLFW_KEY_LAST;
    }

    namespace modkey_codes {
        constexpr int SHIFT = GLFW_MOD_SHIFT;
        constexpr int CONTROL = GLFW_MOD_CONTROL;
        constexpr int ALT = GLFW_MOD_ALT;
        constexpr int SUPER = GLFW_MOD_SUPER;
        constexpr int CAPS_LOCK = GLFW_MOD_CAPS_LOCK;
        constexpr int NUM_LOCK = GLFW_MOD_NUM_LOCK;
    }

    namespace key_action_codes {
        constexpr int RELEASE = GLFW_RELEASE;
        constexpr int PRESS = GLFW_PRESS;
        constexpr int REPEAT = GLFW_REPEAT;
    }
}

#endif //GLFW_WRAPPER_HPP
