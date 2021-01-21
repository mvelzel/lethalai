#include "graphics_handler.h"
#include <iostream>
#include <map>
#include "../helpers/code_injector.h"
#include <GL/GLU.h>
#include <GL/glut.h>

namespace graphics {
    GraphicsHandler* global_graphics_handler;

    HGLRC hook_last_rc_created = NULL;
    std::map<int, HGLRC> hook_pixelformat_to_rc;


    void WINAPI hwglSwapBuffers(_In_ HDC dc) {
        if( !dc )
            return;
        int const pixelformat = GetPixelFormat(dc);
        int init = false;

        HGLRC const host_rc = wglGetCurrentContext();

        HGLRC hook_rc;
        if( 0 == hook_pixelformat_to_rc.count(pixelformat) ) {
            hook_rc = wglCreateContext(dc);
            if( !hook_rc )
                return;

            hook_pixelformat_to_rc[pixelformat] = hook_rc;
            //init_RC();

            if( hook_last_rc_created )
                wglShareLists(hook_last_rc_created, hook_rc);
            else {
                init = true;
                wglMakeCurrent(dc, hook_rc);
                global_graphics_handler->InitGL();
            }
            hook_last_rc_created = hook_rc;
        }
        else {
            hook_rc = hook_pixelformat_to_rc[pixelformat];
        }


        HWND const wnd = WindowFromDC(dc);
        RECT wnd_rect;
        GetClientRect(wnd, &wnd_rect);

        if (!init)
            wglMakeCurrent(dc, hook_rc);

        global_graphics_handler->Draw(&wnd_rect);

        wglMakeCurrent(dc, host_rc);
    }

    DWORD tmp_esp = 0;
    DWORD swap_buffer_return = 0;
    __declspec(naked) void SwapBufferInject(void) {
        __asm {
            pop swap_buffer_return

                push ebx
                mov ebx, [esp+8]
                mov tmp_esp, ebx
                pop ebx

                pushad
                pushfd
                push tmp_esp
                call hwglSwapBuffers
                popfd
                popad

                push ebp
                mov ebp, esp

                push swap_buffer_return
                ret
        }
    }


    GraphicsHandler::GraphicsHandler() {
        global_graphics_handler = this;

        this->Inject();
    }

    void GraphicsHandler::Inject() {
        DWORD swap_address = (DWORD) GetProcAddress(
                GetModuleHandleA("opengl32.dll"),"wglSwapBuffers");
        helpers::CodeInjector::CodeCave(swap_address, SwapBufferInject, 0);
    }

    void GraphicsHandler::Draw(RECT const *rect) {
        for (Drawable* drawable : this->draw_list) {
            drawable->Draw(rect);
        }
    }

    void GraphicsHandler::InitGL() {
        char* t = "";
        int y = 1;
        glutInit( &y, &t);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1280, 720, 0.0, 1.0, -1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 1.0);
    }

    void GraphicsHandler::AddDrawable(Drawable *drawable) {
        this->draw_list.push_back(drawable);
    }
}
