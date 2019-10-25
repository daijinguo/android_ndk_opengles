
#include "esUtil.h"

#include <android_native_app_glue.h>
#include <android/log.h>

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static const char *TAG = "ndk_esUtil_Android";

static float getCurrentTime()
{
    struct timespec clockRealTime;
    clock_gettime ( CLOCK_MONOTONIC, &clockRealTime );
    double curTimeInSeconds = clockRealTime.tv_sec + ( double ) clockRealTime.tv_nsec / 1e9;
    return ( float ) curTimeInSeconds;
}


static void handleCommand(struct android_app *app, int32_t cmd)
{
    auto *context = (ESContext *)app->userData;

    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            // the OS asked us to save the state of the app
            break;

        case APP_CMD_INIT_WINDOW:
            {
                context->eglNativeDisplayType = EGL_DEFAULT_DISPLAY;
                context->eglNativeWindowType  = app->window;

                if (GL_TRUE != esMain(context))
                {
                    esLogMessage(TAG, "call esMain return false");
                    exit(0);
                }
                break;
            }

        // clean up on shutdown
        case APP_CMD_TERM_WINDOW:
            {
                if (nullptr != context->func_shutdown)
                {
                    context->func_shutdown(context);
                }

                if (nullptr != context->userData)
                {
                    free(context->userData);
                }

                memset(context, 0x00, sizeof(ESContext));

                break;
            }

        case APP_CMD_LOST_FOCUS:
            {
                break;
            }


        case APP_CMD_GAINED_FOCUS:
            {
                break;
            }
    }
}


void android_main( struct android_app *app )
{
    esLogMessage(TAG, "android_main");

    ESContext context;
    float     lastTime;

    // Make sure glue isn't stripped.
    app_dummy();

    // init the context
    memset( &context, 0x00, sizeof(ESContext) );

    context.platformData = (void *)app->activity->assetManager;

    app->onAppCmd = handleCommand;
    app->userData = &context;

    lastTime      = getCurrentTime();

    while(true)
    {
        int ident;
        int events;
        struct android_poll_source *source;
        while( (ident = ALooper_pollAll(0, nullptr, &events, (void **)&source)) >= 0)
        {
            if (nullptr != source)
            {
                source->process(app, source);
            }

            if (app->destroyRequested != 0)
            {
                esLogMessage(TAG, "destroyRequested");
                return;
            }
        }

        if(nullptr == context.eglNativeWindowType)
        {
            // esLogMessage(TAG, "empty ANativeWindow");
            continue;
        }

        // call app update function
        if (nullptr != context.func_update)
        {
            float currentTime = getCurrentTime();
            float deltaTime   = (currentTime - lastTime);
            lastTime          = currentTime;
            context.func_update(&context, deltaTime);
        }

        if (nullptr != context.func_draw)
        {
            context.func_draw(&context);
            eglSwapBuffers(context.eglDisplay, context.eglSurface);
        }
    }
}


void esLogMessage ( const char *tag, const char *formatStr, ... )
{
    va_list params;
    char buffer[BUFSIZ];

    va_start(params, formatStr);
    vsprintf(buffer, formatStr, params);
    __android_log_print ( ANDROID_LOG_INFO, tag, "%s", buffer );
    va_end(params);
}

