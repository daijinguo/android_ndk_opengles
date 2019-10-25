
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

#include "esUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *TAG = "ndk_esUtil";

EGLint getContextRenderType(EGLDisplay eglDisplay)
{
#ifdef EGL_KHR_create_context
    const char *extensions = eglQueryString ( eglDisplay, EGL_EXTENSIONS );
    if ( extensions != nullptr && strstr( extensions, "EGL_KHR_create_context" ) )
    {
        // extension is supported
        return EGL_OPENGL_ES3_BIT_KHR;
    }
#endif
    return EGL_OPENGL_ES2_BIT;
}

GLboolean
esCreateWindow(ESContext *context, const char *title, GLint width, GLint height, GLuint flags)
{
    esLogMessage(TAG, "EGL create window: title=%s, width=%d, height=%d, flags=%d.", title, width, height, flags);

    EGLConfig  config;
    EGLint     majorVersion;
    EGLint     minorVersion;
    EGLint     contextAttributeList[] =
    {
        EGL_CONTEXT_CLIENT_VERSION,
        3,
        EGL_NONE
    };

    if (nullptr == context)
    {
        return GL_FALSE;
    }

    context->width  = ANativeWindow_getWidth (context->eglNativeWindowType);
    context->height = ANativeWindow_getHeight(context->eglNativeWindowType);

    context->eglDisplay = eglGetDisplay(context->eglDisplay);
    if (EGL_NO_DISPLAY == context->eglDisplay)
    {
        esLogMessage(TAG, "get GL display failed" );
        return GL_FALSE;
    }

    // init EGL
    if( !eglInitialize(context->eglDisplay, &majorVersion, &minorVersion) )
    {
        esLogMessage(TAG, "EGL init failed" );
        return GL_FALSE;
    }
    esLogMessage(TAG, "EGL version: major=%d, minor=%d.", majorVersion, minorVersion);

    {
        EGLint  numConfigs      = 0;
        EGLint  attributeList[] =
        {
            EGL_RED_SIZE,       5,
            EGL_GREEN_SIZE,     6,
            EGL_BLUE_SIZE,      5,
            EGL_ALPHA_SIZE,     ( flags & ES_WINDOW_ALPHA )       ? 8 : EGL_DONT_CARE,
            EGL_DEPTH_SIZE,     ( flags & ES_WINDOW_DEPTH )       ? 8 : EGL_DONT_CARE,
            EGL_STENCIL_SIZE,   ( flags & ES_WINDOW_STENCIL )     ? 8 : EGL_DONT_CARE,
            EGL_SAMPLE_BUFFERS, ( flags & ES_WINDOW_MULTISAMPLE ) ? 1 : 0,
            // if EGL_KHR_create_context extension is supported, then we will use
            // EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
            EGL_RENDERABLE_TYPE, getContextRenderType(context->eglDisplay),
            EGL_NONE
        };

        // choose config
        if ( !eglChooseConfig( context->eglDisplay, attributeList, &config, 1, &numConfigs ) )
        {
            esLogMessage(TAG, "choose config failed");
            return GL_FALSE;
        }

        esLogMessage(TAG, "config number: %d.", numConfigs);

        if (numConfigs < 1)
        {
            esLogMessage(TAG, "No use choose config");
            return GL_FALSE;
        }
    }

    // For Android, need to get the EGL_NATIVE_VISUAL_ID and set it using ANativeWindow_setBuffersGeometry
    EGLint format = 0;
    eglGetConfigAttrib( context->eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format );
    ANativeWindow_setBuffersGeometry( context->eglNativeWindowType, 0, 0, format );


    // Create a surface
    context->eglSurface = eglCreateWindowSurface( context->eglDisplay, config, context->eglNativeWindowType, nullptr );
    if(context->eglSurface == EGL_NO_SURFACE)
    {
        esLogMessage(TAG, "create surface failed");
        return GL_FALSE;
    }

    // create GL context
    context->eglContext = eglCreateContext ( context->eglDisplay, config, EGL_NO_CONTEXT, contextAttributeList );
    if (context->eglContext == EGL_NO_CONTEXT)
    {
        esLogMessage(TAG, "create EGL context failed");
        return GL_FALSE;
    }


    // Make the context current
    if ( !eglMakeCurrent( context->eglDisplay, context->eglSurface,  context->eglSurface, context->eglContext ) )
    {
        esLogMessage(TAG, "egl mark failed");
        return GL_FALSE;
    }

    return GL_TRUE;
}

void esRegisterDrawFunc( ESContext *context, void (*func_draw    )( ESContext * ) )
{
    context->func_draw = func_draw;
}

void esRegisterShutdownFunc( ESContext *context, void (*func_shutdown)( ESContext * ) )
{
    context->func_shutdown = func_shutdown;
}

void esRegisterUpdateFunc( ESContext *context, void (*func_update  )( ESContext *, float ) )
{
    context->func_update = func_update;
}

void esRegisterKeyFunc( ESContext *context, void (*func_key     )( ESContext *, unsigned char, int, int ) )
{
    context->func_key = func_key;
}


#ifdef __cplusplus
}
#endif

