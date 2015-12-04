#include "hook.h"
#include "dxscreen.h"

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <string>


typedef void (APIENTRY *glBegin_t)(GLenum);
typedef void (APIENTRY *glEnd_t)(void);
typedef void (APIENTRY *glGetIntegerv_t)(GLenum pname, GLint *params);
typedef void (APIENTRY *glPixelStorei_t)(GLenum pname, GLint param);
typedef void (APIENTRY *glReadBuffer_t)(GLenum mode);
typedef void (APIENTRY *glReadPixels_t)(GLint x, GLint y, GLsizei width,
                                        GLsizei height, GLenum format,
                                        GLenum type, GLvoid *pixels);

struct THookCtx {
    glBegin_t glBeginFunc = nullptr;
    glEnd_t glEndFunc = nullptr;
    glGetIntegerv_t glGetIntegerv = nullptr;
    glPixelStorei_t glPixelStorei = nullptr;
    glReadBuffer_t glReadBuffer = nullptr;
    glReadPixels_t glReadPixels = nullptr;
    TScreenCallback Callback;
};
static THookCtx* HookCtx = new THookCtx();


void Screenshot(int x, int y, int w, int h) {
    HookCtx->glPixelStorei(GL_PACK_ALIGNMENT, 1);
    HookCtx->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    HookCtx->glReadBuffer(GL_FRONT);
    std::string buffer;
    buffer.resize(w * h * (3 * sizeof(uint8_t)));
    HookCtx->glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buffer[0]);
    QByteArray screen = PackImageData(NQtScreen::BF_R8G8B8, &buffer[0], h, w);
    HookCtx->Callback(screen);
}

static void MakeScreen() {
    GLint m_viewport[4];
    HookCtx->glGetIntegerv( GL_VIEWPORT, m_viewport );
    int width = m_viewport[2];
    int height = m_viewport[3];
    Screenshot(0, 0, width, height);
}

DECLSPEC_NOINLINE void APIENTRY Hooked_glBegin(GLenum mode) {
    UnHook(HookCtx->glBeginFunc);
    MakeScreen();
    HookCtx->glBeginFunc(mode);
}

bool MakeOpenGLScreen(const TScreenCallback& callback) {
    HMODULE hOpenGL = GetSystemModule("opengl32.dll");
    if (!hOpenGL) {
        return false;
    }
    HookCtx->Callback = callback;
    HookCtx->glBeginFunc = (glBegin_t)GetProcAddress(hOpenGL, "glBegin");
    HookCtx->glEndFunc = (glEnd_t)GetProcAddress(hOpenGL, "glEnd");
    HookCtx->glGetIntegerv = (glGetIntegerv_t)GetProcAddress(hOpenGL, "glGetIntegerv");
    HookCtx->glPixelStorei = (glPixelStorei_t)GetProcAddress(hOpenGL, "glPixelStorei");
    HookCtx->glReadBuffer = (glReadBuffer_t)GetProcAddress(hOpenGL, "glReadBuffer");
    HookCtx->glReadPixels = (glReadPixels_t)GetProcAddress(hOpenGL, "glReadPixels");

    Hook(HookCtx->glBeginFunc, &Hooked_glBegin);
    return true;
}
