#include "esUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *TAG = "ndk_esShader";

GLuint esLoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint  shader;
    GLint   compiled;

    // create the shader object
    shader = glCreateShader(type);
    if (0 == shader)
    {
        esLogMessage(TAG, "create shader of type=%d failed.", type);
        return 0;
    }

    // load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);

    // compile shader
    glCompileShader(shader);

    // check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint  infoLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
        if (infoLength > 1)
        {
            char *infoLog = (char *)malloc(sizeof(char) * infoLength);
            glGetShaderInfoLog(shader, infoLength, nullptr, infoLog);
            esLogMessage(TAG, "compiling shader failed:\n%s\n", infoLog);
            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


GLuint esLoadProgram( const char *vSrc, const char *fSrc )
{
    GLuint   vShader;
    GLuint   fShader;
    GLuint   program;
    GLint    linked;

    // load the shader
    vShader = esLoadShader(GL_VERTEX_SHADER, vSrc);
    if (0 == vShader)
    {
        esLogMessage(TAG, "load vertex shader failed");
        return 0;
    }

    fShader = esLoadShader(GL_FRAGMENT_SHADER, fSrc);
    if (0 == fShader)
    {
        glDeleteShader(vShader);
        return 0;
    }

    // create the program object
    program = glCreateProgram();
    if (0 == program)
    {
        esLogMessage(TAG, "create program");
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        return 0;
    }

    glAttachShader(program, vShader);
    glAttachShader(program, fShader);

    // link the program
    glLinkProgram(program);

    // check the link status
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        GLint  infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1)
        {
            char *infoLog = (char *)malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog);

            esLogMessage(TAG, "Linking program failed:\n%s\n.", infoLog);

            free(infoLog);
        }

        glDeleteProgram(program);
        return 0;
    }

    // free no longer needed shader resources
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return program;
}


#ifdef __cplusplus
}
#endif

