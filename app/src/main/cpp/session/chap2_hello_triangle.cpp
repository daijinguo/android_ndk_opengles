#include "esUtil.h"

static const char *TAG = "ndk_HelloTriangle";


typedef struct
{
    // Handle to a program object
    GLuint programObject;

} UserData;




static int Init( ESContext *context )
{
    auto userData = (UserData *)context->userData;
    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "}                                        \n";

    char fShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "}                                            \n";

    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(program == 0)
    {
        esLogMessage(TAG, "init load program failed");
        return GL_FALSE;
    }

    // Store the program object
    userData->programObject = program;

    glClearColor ( 1.0F, 1.0F, 1.0F, 0.0F );
    return GL_TRUE;
}


static void Draw( ESContext *context )
{
    // esLogMessage(TAG, "draw begin");

    auto userData = (UserData *)context->userData;
    GLfloat vVertices[] =
    {
        0.0F,  0.5F, 0.0F,
        -0.5F, -0.5F, 0.0F,
        0.5f, -0.5F, 0.0F
    };

    // Set the viewport
    glViewport ( 0, 0, context->width, context->height );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    // Use the program object
    glUseProgram ( userData->programObject );

    // Load the vertex data
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
    glEnableVertexAttribArray ( 0 );

    glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

static void Shutdown( ESContext *context )
{
    esLogMessage(TAG, "shutdown program");

    auto userData = (UserData *)context->userData;
    glDeleteProgram ( userData->programObject );
}

int esMain ( ESContext *context )
{
    context->userData = malloc( sizeof ( UserData ) );
    esCreateWindow( context, "Hello Triangle", 320, 240, ES_WINDOW_RGB );

    if ( Init( context ) != GL_TRUE )
    {
        esLogMessage(TAG, "init failed");
        return GL_FALSE;
    }

    esRegisterShutdownFunc( context, Shutdown );
    esRegisterDrawFunc    ( context, Draw );

    return GL_TRUE;
}


