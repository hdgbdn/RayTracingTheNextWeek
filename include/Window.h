#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <string>
#include "GLFW/glfw3.h"
#include <functional>
#include <unordered_map>

using namespace std;

namespace hdgbdn
{
    class Window
    {
        using lambda = function<void()>;
        friend Window& operator<<(const Window&, lambda);
    public:
        Window(unsigned int w = 1280, unsigned int h = 720, const string& name = "Untitled Window");
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        static void StartRenderLoop(Window&);

        void PushPreRenderOperation(lambda);
        void PushRenderOperation(lambda);
        void PushPostRenderOperation(lambda);

        void Close() const;

        int GetWidth() const;
        int GetHeight() const;
    	
        operator GLFWwindow* ();
        GLFWwindow* get() const;
        ~Window();
    private:
        bool isInit;
        GLFWwindow* pW;

        lambda preRender;
        lambda render;
        lambda postRender;

        int width;
        int height;
        static unordered_map<GLFWwindow*, Window*> winMap;
    };
}

#endif