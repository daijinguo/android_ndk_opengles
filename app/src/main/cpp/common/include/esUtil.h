#ifndef OPEN_GL_ES_H
#define OPEN_GL_ES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

/*
 * esCreateWindow flag - RGB color buffer
 */
#define  ES_WINDOW_RGB      0

/*
 * esCreateWindow flag - ALPHA color buffer
 */
#define ES_WINDOW_ALPHA     1

/*
 * esCreateWindow flag - depth buffer
 */
#define ES_WINDOW_DEPTH     2

/*
 * esCreateWindow flag - stencil buffer
 */
#define ES_WINDOW_STENCIL   4

/*
 * esCreateWindow flat - multi-sample buffer
 */
#define ES_WINDOW_MULTISAMPLE   8


typedef struct
{
    GLfloat   m[4][4];
} ESMatrix;


typedef struct ESContext ESContext;

struct ESContext
{
    /*
     * put platform specific data here
     */
    void   *platformData;

    /*
     * put user data here
     */
    void   *userData;

    /*
     * window width
     */
    GLint   width;

    /*
     * window height
     */
    GLint   height;

    /*
     * display handle
     */
    EGLNativeDisplayType   eglNativeDisplayType;

    /*
     * window handle
     */
    EGLNativeWindowType    eglNativeWindowType;

    /*
     * EGL display
     */
    EGLDisplay             eglDisplay;

    /*
     * EGL context
     */
    EGLContext             eglContext;

    /*
     * EGL surface
     */
    EGLSurface             eglSurface;


    //
    // callback function
    //
    void (*func_draw)     ( ESContext * );
    void (*func_shutdown) ( ESContext * );
    void (*func_key)      ( ESContext *, unsigned char, int, int );
    void (*func_update)   ( ESContext *, float );
};

/**
 * create a window width the specified parameters
 * @param context
 * @param title
 * @param width
 * @param height
 * @param flags
 * @return GL_TRUE if window creation is successful, GL_FALSE otherwise
 */
GLboolean esCreateWindow( ESContext *context, const char *title, GLint width, GLint height, GLuint flags);


void esRegisterDrawFunc     ( ESContext *context, void (*func_draw    )( ESContext * ) );
void esRegisterShutdownFunc ( ESContext *context, void (*func_shutdown)( ESContext * ) );
void esRegisterUpdateFunc   ( ESContext *context, void (*func_update  )( ESContext *, float ) );
void esRegisterKeyFunc      ( ESContext *context, void (*func_key     )( ESContext *, unsigned char, int, int ) );


void esLogMessage ( const char *tag, const char *formatStr, ... );

GLuint esLoadShader ( GLenum type, const char *shaderSrc );
GLuint esLoadProgram( const char *vSrc, const char *fSrc );
char *esLoadTGA    ( void *ioContext, const char *fileName, int *width, int *height );

int    esGenSphere    ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals,  GLfloat **texCoords, GLuint **indices );
int    esGenCube      ( float scale, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords, GLuint **indices );
int    esGenSquareGrid( int size, GLfloat **vertices, GLuint **indices );

void   esScale             ( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );
void   esTranslate         ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
void   esRotate            ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
void   esFrustum           ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
void   esPerspective       ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ );
void   esOrtho             ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
void   esMatrixMultiply    ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
void   esMatrixLoadIdentity( ESMatrix *result );
void   esMatrixLookAt      ( ESMatrix *result,
                             float posX, float posY, float posZ,
                             float lookAtX, float lookAtY, float lookAtZ,
                             float upX, float upY, float upZ );


extern int esMain( ESContext *esContext );

#ifdef __cplusplus
}
#endif

#endif//OPEN_GL_ES_H




