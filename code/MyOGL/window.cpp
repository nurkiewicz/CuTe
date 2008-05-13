//---------------------------------------------------------------------------

///@file
///Definitions of a Window and Extensions classes and some constants.
///
///@par License:
///@verbatim
///MyOGL - My OpenGL utility, simple OpenGL Windows framework
///Copyright (C) 2005-06 Tomasz Nurkiewicz
///For full license text see license.txt.
///
///This program is free software; you can redistribute it and/or modify it under the terms of
///the GNU General Public License as published by the Free Software Foundation;
///either version 2 of the License, or (at your option) any later version.
///
///This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
///without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
///See the GNU General Public License for more details.
///
///You should have received a copy of the GNU General Public License along with this program;
///if not, write to the Free Software Foundation, Inc.,
///59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///@endverbatim
///
///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
///@date Jul 2005-Mar 2006

//---------------------------------------------------------------------------

#include <cmath>
#include "window.h"
using namespace std;
using namespace MyOGL;

///WM_MOUSEWHEEL redundant definition.
///Dirty trick: Although this message is recognized in my Windows platform,
///it is not included by preprocessor from my winuser.h...
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

//---------------------------------------------------------------------------

void MyOGL::glColorHSV(float hue, float saturation, float value)
	{
	glColor3fv(hsv2rgb(hue, saturation, value));
	}

void MyOGL::glColorHSV(float hue, float saturation, float value, float alpha)
	{
	float colors[4];
	colors[3] = alpha;		//forth value is alpha
	glColor4fv(hsv2rgb<float>(hue, saturation, value, colors));
	}

//---------------------------------------------------------------------------

Window::MouseData::MouseData(): wheel(0), lButton(false), rButton(false)
	{
	pos.x = 0;
	pos.y = 0;
	}

//---------------------------------------------------------------------------

const bool MyOGL::FULLSCREEN = true;
const bool MyOGL::WINDOWED = false;

Window::KeyData Window::keys[256];
bool Window::wActive = true;
bool Window::created = false;
int Window::width_;
int Window::height_;
Window::MouseData Window::mouseData_;

//---------------------------------------------------------------------------

Window::Window(const std::string &iTitle, int iWidth, int iHeight, bool iFullscreen, int iExtensionsFlags):
	extensionsFlags(iExtensionsFlags), hDC(NULL), hRC(NULL), hWnd(NULL), 
	title(iTitle), fullscreen(iFullscreen)
	{
	if(created)
		throw WinEx("OpenGL Window already created");
	created = true;
	width_ = iWidth;
	height_ = iHeight;
	init();
	}

Window::~Window()
	{
	kill();
	hRC = NULL;
	hDC = NULL;
	hWnd = NULL;
	hInstance = NULL;
	created = false;
	}

void Window::init()
	{
	unsigned int pixelFormat;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT windowRect = {0, 0, width_, height_};
	hInstance = GetModuleHandle(NULL);
	WNDCLASS wc = {CS_HREDRAW | CS_VREDRAW | CS_OWNDC, wndProc, 0, 0, hInstance,
		LoadIcon(NULL, IDI_WINLOGO), LoadCursor(NULL, IDC_ARROW), NULL, NULL, "ogl"};
	if(!RegisterClass(&wc))
		throw WinEx("Can't create Window");
	if(fullscreen)
		{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width_;
		dmScreenSettings.dmPelsHeight = height_;
		dmScreenSettings.dmBitsPerPel = 16;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
			throw WinEx("Can't set specified fullscreen mode");
		}
	if(fullscreen)
		{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(false);
		}
	else
		{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		}
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);
	hWnd = CreateWindowEx(dwExStyle, "ogl", title.c_str(), WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle, 0, 0,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL, hInstance, NULL);
	if(hWnd == NULL)
		{
		kill();
		throw WinEx("Can't create Window");
		}
	static PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
		16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0};
	hDC = GetDC(hWnd);
	if(hDC == NULL)
		{
		kill();
		throw WinEx("Can't create OpenGL Window");
		}
	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if(pixelFormat == 0)
		{
		kill();
		throw WinEx("Can't create OpenGL Window");
		}
	if(!SetPixelFormat(hDC,pixelFormat, &pfd))
		{
		kill();
		throw WinEx("Can't create OpenGL Window");
		}
	hRC = wglCreateContext(hDC);
	if(hRC == NULL)
		{
		kill();
		throw WinEx("Can't create OpenGL Window");
		}
	if(!wglMakeCurrent(hDC, hRC))
		{
		kill();
		throw WinEx("Can't create OpenGL Window");
		}
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	initGL();
	}

void Window::initGL()
	{
	viewport();		//set the default viewport (filling full window)
	extensions_ = new Extensions(extensionsFlags, hDC);	///Enable all desired extensions
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	}

void Window::kill()
	{
	///disable all choosen extensions
	delete extensions_;
	if(fullscreen)
		{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
		}
	bool error = false;
	if(hRC != NULL)
		{
		error |= !wglMakeCurrent(NULL, NULL);
		error |= !wglDeleteContext(hRC);
		}
	if((hDC != NULL) && !ReleaseDC(hWnd, hDC))
		error = true;
	if((hWnd != NULL) && !DestroyWindow(hWnd))
		error = true;
	if(!UnregisterClass("ogl", hInstance))
		error = true;
	if(error)
		throw WinEx("Error destroying OpenGL Window");
	}

void Window::viewport(int left, int right, int top, int bottom, bool ortho)
	{
	if(right == left)
		++right;		//prevent dividing to 0
	glViewport(left, bottom, right - left, top - bottom);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const float aspect = static_cast<float>(right - left) / (top - bottom);
	if(ortho)
		glOrtho(-aspect, aspect, -1.0, 1.0, 3.0, -3.0);
	else
		{
		gluPerspective(45.0, aspect, 0.1, 100.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}

void Window::toggleFullscreen()
	{
	kill();
	fullscreen = !fullscreen;
	init();
	}

void Window::refresh()
	{
	if(extensions().enabled(FPS_COUNTER))
		extensions().fpsCounter().frame();		//count this frame if FPS counter is enabled
	SwapBuffers(hDC);
	}

bool Window::keyPressed(int keyCode)
	{
	KeyData *key = &keys[keyCode];
	if(key->pressed && !key->read)
		{
		key->read = true;
		return true;
		}
	return false;
	}

LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	switch(uMsg)
		{
		case WM_ACTIVATE:
			wActive = HIWORD(wParam) == 0;
			return 0;
		case WM_SYSCOMMAND:
			{
			if((wParam == SC_SCREENSAVE) || (wParam == SC_MONITORPOWER))
				return 0;
			break;
			}
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			keys[wParam].pressed = true;
			return 0;
		case WM_KEYUP:
			keys[wParam].pressed = false;
			keys[wParam].read = false;
			return 0;
		case WM_MOUSEMOVE:		//mouse routines
			mouseData_.pos = MAKEPOINTS(lParam);
			return 0;
		case WM_MOUSEWHEEL:
			mouseData_.wheel += static_cast<short>(HIWORD(wParam));
			return 0;
		case WM_LBUTTONDOWN:
			mouseData_.lButton = true;
			return 0;
		case WM_LBUTTONUP:
			mouseData_.lButton = false;
			return 0;
		case WM_RBUTTONDOWN:
			mouseData_.rButton = true;
			return 0;
		case WM_RBUTTONUP:
			mouseData_.rButton = false;
			return 0;
		}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

//---------------------------------------------------------------------------
