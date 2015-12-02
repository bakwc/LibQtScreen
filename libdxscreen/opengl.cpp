#include "hook.h"
#include "dxscreen.h"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <fstream>
#include <string>


typedef void (APIENTRY *glBegin_t)(GLenum);
typedef void (APIENTRY *glEnd_t)(void);

struct THookCtx {
    void* glBeginFunc = nullptr;
    void* glEndFunc = nullptr;
    TScreenCallback Callback;
};
static THookCtx* HookCtx = new THookCtx();


void Screenshot(int x, int y, int w, int h) {
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    std::string buffer;
    buffer.resize(w * h * (3 * sizeof(uint8_t)));
    glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buffer[0]);
    QImage img = IntArrayToQImage(BF_R8G8B8, &buffer[0], h, w);
    HookCtx->Callback(img);
}

static void MakeScreen() {
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );
    int width = m_viewport[2];
    int height = m_viewport[3];
    Screenshot(0, 0, width, height);
}

DECLSPEC_NOINLINE void APIENTRY Hooked_glBegin(GLenum mode) {
    UnHook(HookCtx->glBeginFunc);
    MakeScreen();
    glBegin(mode);
}

DECLSPEC_NOINLINE void APIENTRY Hooked_glEnd(void) {
    UnHook(HookCtx->glEndFunc);
    //MakeScreen();
    glEnd();
}

void MakeOpenGLScreen(const TScreenCallback& callback) {
    HMODULE m_hOpenGL=GetModuleHandle(L"opengl32.dll");
    HookCtx->Callback = callback;
    HookCtx->glBeginFunc = (void*)GetProcAddress(m_hOpenGL,"glBegin");
    HookCtx->glEndFunc = (void*)GetProcAddress(m_hOpenGL,"glEnd");
    Hook(HookCtx->glBeginFunc, &Hooked_glBegin);
    //Hook(HookCtx->glEndFunc, &Hooked_glEnd);
}
