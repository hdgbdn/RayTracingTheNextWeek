#include <iostream>
#include "glad/glad.h"
#include "Window.h"

using namespace std;
using namespace hdgbdn;

std::unordered_map<GLFWwindow*, Window*> Window::winMap = std::unordered_map<GLFWwindow*, Window*>();

Window::Window(unsigned w, unsigned h, const string& name)
		:isInit(false), pW(nullptr), preRender(), render(), postRender(),
			width(w), height(h)
{
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    pW = glfwCreateWindow(w, h, name.c_str(), NULL, NULL);
    glfwMakeContextCurrent(pW);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw(runtime_error("failed to initialize GLAD"));
    }
    isInit = true;
    winMap.insert({ pW, this });

    glfwSetFramebufferSizeCallback(pW, [](GLFWwindow * window, int w, int h)
    {
            Window* win = winMap[window];
    	if(win != nullptr)
    	{
            win->width = w;
            win->height = h;
            glViewport(0, 0, w, h);
    	}
    });
}

void Window::SetPreRenderOperation(lambda func)
{
    preRender = func;
}

void Window::SetRenderOperation(lambda func)
{
    render = func;
}


void Window::SetPostRenderOperation(lambda func)
{
    postRender = func;
}


void Window::StartRenderLoop(Window& win)
{
    if (!win.isInit)
    {
        std::cout << "window not init" << endl;
        return;
    }
    if(win.preRender) win.preRender();
	while(!glfwWindowShouldClose(win.pW))
	{
        if (win.render) win.render();
	}
    if (win.postRender) win.postRender();
}

Window::operator struct GLFWwindow*()
{
    return pW;
}

GLFWwindow* Window::get() const
{
    return pW;
}

void Window::Close() const
{
    glfwSetWindowShouldClose(pW, true);
}

int Window::GetWidth() const
{
    return width;
}


int Window::GetHeight() const
{
    return height;
}

Window::~Window()
{
    if (!isInit) return;
    winMap.erase(pW);
    glfwTerminate();
}
