/*
Copyright (C) 2011 by Ladislav Hrabcak

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "base.h"
#include "app.h"

#include "gl/glew.h"
#include "gl/wglew.h"

#include <Windows.h>
#include <WinSock2.h>
#include <Windowsx.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

LRESULT WINAPI MsgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static base::app *myapp=0;

	switch(msg) {
	case WM_CREATE:
		myapp=reinterpret_cast<base::app*>(
			reinterpret_cast<const CREATESTRUCT*>(lParam)->lpCreateParams);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		ValidateRect(hwnd,0);
		break;

	case WM_MOUSEMOVE:
		myapp->mouse_move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		else {
			if((lParam & (1<<30)) == 0)
				myapp->key(int(wParam), true);
		}
		break;

	case WM_KEYUP:
		myapp->key(int(wParam), false);
		break;

	case WM_ACTIVATEAPP:
		myapp->activated(wParam == TRUE);
		break;
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void create_window_win(
	base::app *a,
	HWND *hwnd,
	const char *window_name,
	const char *class_name,
	int width,
	int height)
{
	WNDCLASSEX wc={ 
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		MsgProc,
		0L,
		0L,
		GetModuleHandle(0),
		0,0,0,0,
		class_name,
		0 
	};

	if(RegisterClassEx(&wc)==0)
		throw base::exception(SRC_LOCATION_STR)
			<< "RegisterClassEx failed! (err:" << GetLastError() << ")";

	RECT wr={0,0,width,height};
	DWORD exstyle=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU;

	if(!AdjustWindowRectEx(&wr,exstyle,FALSE,0))
		throw base::exception(SRC_LOCATION_STR)
			<< "Cannot adjust window rect (err:" << GetLastError() << ")";

    width=abs(wr.left)+wr.right;
    height=abs(wr.top)+wr.bottom;

	*hwnd = CreateWindow(
		class_name,
		window_name,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		0,0,
		width,height,
		GetDesktopWindow(),
		0,
		wc.hInstance,
		a);
	if(*hwnd == 0)
		throw base::exception(SRC_LOCATION_STR)
		<< "CreateWindow failed! (err:" << GetLastError() << ")";

	ShowWindow(*hwnd,SW_SHOWDEFAULT);
	UpdateWindow(*hwnd);

	ShowCursor(0);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

HWND __hwnd = 0;
HDC __hdc = 0;
HGLRC __hrc;
HGLRC __shared_ctx = 0;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::get_window_handle() { return __hwnd; }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::swap_buffers() { SwapBuffers(__hdc); }

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::init_opengl_win()
{
	HDC hdc = GetWindowDC(__hwnd);
	if(hdc == 0)
		throw base::exception(SRC_LOCATION_STR)
			<< "GetWindowDC failed! (err:" << GetLastError() << ")";

	int pf;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,								// version number
		PFD_DRAW_TO_WINDOW |			// support window
		PFD_SUPPORT_OPENGL |			// support OpenGL
		PFD_DOUBLEBUFFER |				// double buffered
		PFD_GENERIC_ACCELERATED |
		PFD_SWAP_COPY,					// dont copy just exchange
		PFD_TYPE_RGBA,					// RGBA type
		32,								// 24-bit color depth
		0, 0, 0, 0, 0, 0,				// color bits ignored
		0,								// no alpha buffer
		0,								// shift bit ignored
		0,								// no accumulation buffer
		0, 0, 0, 0,						// accum bits ignored
		24,								// 32-bit z-buffer
		8,								// 8-bit stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main layer
		0,								// reserved
		0, 0, 0							// layer masks ignored
	};

	if((pf = ChoosePixelFormat(hdc, &pfd)) == 0) 
		throw base::exception(SRC_LOCATION_STR) << "ChoosePixelFormat failed!";

	if(SetPixelFormat(hdc, pf, &pfd) == 0) 
		throw base::exception(SRC_LOCATION_STR) << "SetPixelFormat failed!";

	if((__hrc=wglCreateContext(hdc)) == 0) 
		throw base::exception(SRC_LOCATION_STR) << "wglCreateContext failed!";

	if(wglMakeCurrent(hdc,__hrc) == 0)
		throw base::exception(SRC_LOCATION_STR) << "wglMakeCurrent failed!";

    glewExperimental=GL_TRUE;

	const GLenum err=glewInit();
	if(GLEW_OK != err)
		throw base::exception(SRC_LOCATION_STR) << "glewInit failed!";

    if(wglewIsSupported("WGL_ARB_create_context") != 1)
		throw base::exception(SRC_LOCATION_STR)
			<< "WGL_ARB_create_context is not supported! (is it OpenGL 3.2 capable card?)";

    wglMakeCurrent(0,0);
	wglDeleteContext(__hrc);

	int attribs[]={
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3, 
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, cfg().use_debug_context ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
		0
	};

	if(0 == (__hrc = wglCreateContextAttribsARB(hdc, 0, attribs)))
        throw base::exception(SRC_LOCATION_STR) << "wglCreateContext failed!";

	set_main_rc();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::set_main_rc()
{
    if(!wglMakeCurrent(__hdc, __hrc))
		throw base::exception(SRC_LOCATION_STR) << "wglMakeCurrent main context failed!";

    int sw = wglGetSwapIntervalEXT();
    if(sw != 0) {
        wglSwapIntervalEXT(0);

        sw = wglGetSwapIntervalEXT();
        if(sw != 0)
            throw base::exception(SRC_LOCATION_STR) << "Unable to disable monitor V-SYNC";
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static void WINAPI gl_debug_msg_proc_arb(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
	userParam;
	//message;
	length;
	severity;
	id;
	type;
	source;
	
	printf("%s\n",message);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::init_opengl_dbg_win()
{
	if(cfg().use_debug_context) {
		glDebugMessageControlARB=
			(PFNGLDEBUGMESSAGECONTROLARBPROC)wglGetProcAddress("glDebugMessageControlARB");
		glDebugMessageCallbackARB=
			(PFNGLDEBUGMESSAGECALLBACKARBPROC)wglGetProcAddress("glDebugMessageCallbackARB");
		glDebugMessageInsertARB=
			(PFNGLDEBUGMESSAGEINSERTARBPROC)wglGetProcAddress("glDebugMessageInsertARB");

        if(glDebugMessageControlARB 
			&& glDebugMessageCallbackARB 
			&& glDebugMessageInsertARB) {
			glDebugMessageCallbackARB(&gl_debug_msg_proc_arb,0);
			glDebugMessageControlARB(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE,0,0,GL_TRUE);

			if(cfg().use_debug_context && cfg().use_debug_sync)
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		}
		else {
			std::cout<<"No OpenGL debug extension..."<<std::endl;
		}
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::run_app_win(base::app * const a, const bool initgl)
{
	create_window_win(
		a,
		&__hwnd,
		a->get_wnd_name(),
		a->get_wnd_cls(),
		a->get_wnd_width(),
		a->get_wnd_height());

	__hdc = GetWindowDC(__hwnd);
	if(!__hdc)
		throw base::exception(SRC_LOCATION_STR)
			<< "Cannot get device context!";

	MSG msg;

	a->start();

	hptimer timer;
	timer.start();

	for( ;; ) {
		bool exit = false;
		while(PeekMessage(&msg, 0, 0U, 0U, PM_REMOVE) != 0) {
			if(msg.message==WM_QUIT)
				exit = true;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if(exit || a->is_shutdown())
            break;

		a->draw_frame();
		a->inc_frame_number();
	}

	a->stop();
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

glm::ivec2 base::get_mouse_pos()
{
	POINT pos;
	GetCursorPos(&pos);
	return glm::ivec2(pos.x,pos.y);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::set_mouse_pos(const glm::ivec2 &pos)
{
	SetCursorPos(int(pos.x),int(pos.y));
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::sleep_ms(const int time)
{
	Sleep(time);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::get_shared_context() 
{
	return __shared_ctx;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void* base::get_window_hdc()
{
	return __hdc;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::set_win_title(const char* const str)
{
    SetWindowText(__hwnd, str);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void base::get_display_ven_dev_id(unsigned short & vendor_id, unsigned short & device_id, unsigned short & rev_id){
	DISPLAY_DEVICE dd, ddd;
	dd.cb = ddd.cb = sizeof(DISPLAY_DEVICE);

	auto hex_char_to_val = [](char c){
		if (c >= '0' && c <= '9'){
			return c - '0';
		}

		if (c >= 'a' && c <= 'f'){
			return 10 + (c - 'a');
		}

		if (c >= 'A' && c <= 'F'){
			return 10 + (c - 'A');
		}

		return -1;
	};

	int i = 0;
	bool found = false, set = false;

	while (EnumDisplayDevices(0, i++, &dd, 0))
	{
		bool primary = (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

		if (!primary){
			continue;
		}
 
		vendor_id = (0x1000 * (hex_char_to_val(dd.DeviceID[8]))) +
					(0x100 * (hex_char_to_val(dd.DeviceID[9]))) +
					(0x10 * (hex_char_to_val(dd.DeviceID[10]))) +
					(0x1 * (hex_char_to_val(dd.DeviceID[11])));
		
		device_id = (0x1000 * (hex_char_to_val(dd.DeviceID[17]))) +
					(0x100 * (hex_char_to_val(dd.DeviceID[18]))) +
					(0x10 * (hex_char_to_val(dd.DeviceID[19]))) +
					(0x1 * (hex_char_to_val(dd.DeviceID[20])));

		rev_id = (0x10 * (hex_char_to_val(dd.DeviceID[42]))) +
			(0x1 * (hex_char_to_val(dd.DeviceID[43])));;


		break;
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool send_data(const char* header, size_t size, std::ifstream& ifs)
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return false;

	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	/*struct hostent *host;
	host = gethostbyname("perf.outerra.com");
    if(!host)
        return false;*/

	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	//SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    SockAddr.sin_addr.S_un.S_un_b.s_b1 = 95;
    SockAddr.sin_addr.S_un.S_un_b.s_b2 = 85;
    SockAddr.sin_addr.S_un.S_un_b.s_b3 = 63;
    SockAddr.sin_addr.S_un.S_un_b.s_b4 = 201;

	char data[1024];
	bool succ = false;

	do {
		if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0)
			break;

		if (send(Socket, &header[0], int(strlen(&header[0])), 0) != int(strlen(&header[0])))
			break;

		ifs.seekg(0, ifs.beg);

		succ = true;

		while (!ifs.eof()) {
			memset(data, 0, 1024);
			ifs.read(data, 512);
			int len = (int)strlen(data);
			if (send(Socket, data, len, 0) != len) {
				succ = false;
				break;
			}
		}
	} while (0);

	closesocket(Socket);
	WSACleanup();
	return succ;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

bool base::send_test_data(const char * request_header, 
	int header_len, 
	const std::string & test_name,
	const std::string & gpu_name,
	const std::string & gpu_driver,
	int best_score,
	std::ifstream & ifs)
{
	char str_buf[1024];
	memset(&str_buf[0], 0, 1024);
	sprintf(&str_buf[0],
		"Test: %s\n"
		"GPU: %s\n"
		"Drivers: %s\n"
		"\nScore: %d Mtris/s\n"
		"\nDo you want to upload the test results?", test_name.c_str(),gpu_name.c_str(), gpu_driver.c_str(), best_score);

	int res = MessageBox(0, str_buf, "Perf test results", MB_YESNO);

	if (res == IDNO)
		return true;

	bool op = send_data(request_header, header_len, ifs);

	if (op)
		MessageBox(0, "Test results uploaded successfully", "Thank you!", MB_OK | MB_ICONINFORMATION);
	else
		MessageBox(0, "Failed to upload test results", "Error", MB_OK | MB_ICONWARNING);

	return op;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
