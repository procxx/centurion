#include "Window.h"
#include "Graphics.h"
#include "Screen.h"
#include "Image.h"
#include "BooleanConverter.h"
#include "Rectangle.h"
#include "Point.h"
#include "Color.h"
#include <stdexcept>

using centurion::Window;
using centurion::Graphics;
using centurion::Screen;
using centurion::Image;
using centurion::Rectangle;
using centurion::Point;
using centurion::Color;

Window::Window(const std::string& title, int width, int height)
{
	Uint32 flags = SDL_WindowFlags::SDL_WINDOW_OPENGL | SDL_WindowFlags::SDL_WINDOW_HIDDEN;
	initComps(title, width, height, flags);
}

Window::Window(const std::string& title)
{
	Uint32 flags = SDL_WindowFlags::SDL_WINDOW_OPENGL |
		SDL_WindowFlags::SDL_WINDOW_FULLSCREEN |
		SDL_WindowFlags::SDL_WINDOW_HIDDEN;
	initComps(title, Screen::getWidth(), Screen::getHeight(), flags);
}

Window::~Window()
{
	SDL_HideWindow(window);
	SDL_Delay(1);
	delete graphics;
	SDL_DestroyWindow(window);
}

void Window::initComps(const std::string& title, int w, int h, Uint32 flags)
{
	if (w < 1 || h < 1) {
		throw std::invalid_argument("Invalid dimensions for window!");
	}
	window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
	graphics = new Graphics(renderer);
}

void Window::show()
{
	SDL_ShowWindow(window);
}

void Window::hide()
{
	SDL_HideWindow(window);
}

void Window::setResizable(bool resizable)
{
	SDL_bool b = BooleanConverter::convert(resizable);
	SDL_SetWindowResizable(window, b);
}

int Window::getWidth()
{
	int w = -1;
	int h = -1;
	SDL_GetWindowSize(window, &w, &h);
	return w;
}

int Window::getHeight()
{
	int w = -1;
	int h = -1;
	SDL_GetWindowSize(window, &w, &h);
	return h;
}

void Window::update()
{
	graphics->update();
}

void centurion::Window::clearWindow()
{
	graphics->clear();
}

void Window::render(Image& img, int x, int y)
{
	graphics->render(img, x, y);
}

void Window::render(Image& img, int x, int y, int w, int h)
{
	graphics->render(img, x, y, w, h);
}

void Window::render(Image& img, Rectangle rect)
{
	graphics->render(img, rect);
}

void Window::renderFilledRect(int x, int y, int w, int h)
{
	graphics->renderFilledRect(x, y, w, h);
}

void Window::renderOutlinedRect(int x, int y, int w, int h)
{
	graphics->renderOutlinedRect(x, y, w, h);
}

void Window::renderLine(int x1, int y1, int x2, int y2)
{
	graphics->renderLine(x1, y1, x2, y2);
}

void Window::renderLine(Point p1, Point p2)
{
	graphics->renderLine(p1, p2);
}

void Window::setRenderingColor(Color color)
{
	graphics->setColor(color);
}

Image* Window::createImage(std::string path)
{
	return Image::create(path, *graphics);
}