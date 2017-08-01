#include "GLProgram.h"

#include "LogUtils.h"

#include <algorithm>
#include <functional>
#include <cstdlib>  // ios project need this header

const char TAG[] = "log utils";

GLuint evo::GLProgram::setArrayBuffer(float *bufferArray, int bufferSize) {

    GLuint bufferID = 0;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferArray, GL_STATIC_DRAW);

    return bufferID;
}

void evo::GLProgram::init() {
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
}

bool evo::GLProgram::buildProgram(const char *vertexShaderSource,
                                  const char *fragmentShaderSource) {
    _program = glCreateProgram();
    if (!compileShader(&_vertShader, GL_VERTEX_SHADER, vertexShaderSource)) {
        LOGE(TAG, "Failed to compile vertex shader \n");
        return true;
    }
    if (!compileShader(&_fragShader, GL_FRAGMENT_SHADER, fragmentShaderSource)) {
        LOGE(TAG, " Failed to compile fragment shader \n");
        return true;
    }

    glAttachShader(_program, _vertShader);
    glAttachShader(_program, _fragShader);

    return false;
}

bool evo::GLProgram::compileShader(GLuint *shader, GLenum type, const char *shaderSource) {
    GLint status;
    if (!shaderSource) {
        LOGE(TAG, "Failed to load vertex shader");
        return false;
    }
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &shaderSource, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            GLchar *log = (GLchar *) malloc(logLength);
            glGetShaderInfoLog(*shader, logLength, &logLength, log);
            LOGW(TAG, "Shadr souce %s \n", shaderSource);
            LOGW(TAG, "Shader compile log:\n%s \n", log);
            free(log);
        }
    }
    return GL_TRUE;
}

bool evo::GLProgram::link() {
    GLint status;
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        LOGE(TAG, "link program fail!");
        GLint length;
        glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);
        if (length > 0) {
            char *infolog = (char *) malloc(sizeof(GLchar));;
            glGetProgramInfoLog(_program, length, NULL, infolog);
            fprintf(stderr, "%s", infolog);
            if (infolog) {
                free(infolog);
            }
        }
        return true;
    }
    if (_vertShader) {
        glDeleteShader(_vertShader);
        _vertShader = 0;
    }
    if (_fragShader) {
        glDeleteShader(_fragShader);
        _fragShader = 0;
    }

    return false;
}

GLint evo::GLProgram::attributePosition(const char *attributeName) {
    return glGetAttribLocation(_program, attributeName);
}

void evo::GLProgram::use() {
    glUseProgram(_program);
}

void evo::GLProgram::unuse() {
    glUseProgram(0);
}

void evo::GLProgram::validateProgram() {
    GLint logLength;
    glValidateProgram(_program);
    glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *) malloc(logLength);
        glGetProgramInfoLog(_program, logLength, &logLength, log);
        LOGD(TAG, "Program validate log:\n%s \n", log);
        free(log);
    }
}

void evo::GLProgram::release() {
    if (_vertShader) {
        glDeleteShader(_vertShader);
        _vertShader = 0;
    }
    if (_fragShader) {
        glDeleteShader(_fragShader);
        _fragShader = 0;
    }
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }

}

evo::GLProgram::GLProgram() {
    _vertShader = 0;
    _fragShader = 0;
    _program = 0;
    _attributes = {};
}


GLuint evo::GLProgram::setElementBuffer(GLuint *bufferArray, int bufferSize) {
    GLuint bufferID = 0;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, bufferArray, GL_STATIC_DRAW);

    return bufferID;
}

void evo::GLProgram::addAtributeButton(const char *attributeName) {
    string obj(attributeName);
    if (std::find(_attributes.begin(), _attributes.end(), obj) == _attributes.end()) {
        auto iterator = _attributes.insert(_attributes.end(), obj);
        int index = (int) distance(_attributes.begin(), iterator);
        glBindAttribLocation(_program, index, attributeName);
    }
}


GLuint evo::GLProgram::attributeIndex(const char *attributeName) {
    string obj(attributeName);
    auto iterator = std::find(_attributes.begin(), _attributes.end(), obj);
    if (iterator == _attributes.end()) {
        return UINT32_MAX;
    }
    int index = (int) distance(_attributes.begin(), iterator);
    return index;
}


GLint evo::GLProgram::uniformIndex(const char *uniformName) {
    return glGetUniformLocation(_program, uniformName);
}

// do same thing as the release() function
// added by zhuyuanxuan
evo::GLProgram::~GLProgram() {
    if (_vertShader) {
        glDeleteShader(_vertShader);
        _vertShader = 0;
    }
    if (_fragShader) {
        glDeleteShader(_fragShader);
        _fragShader = 0;
        LOGI(TAG, "Delete shader");
    }
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
        LOGI(TAG, "Delete program");
    }
}

/**
 * OpenGL common functions
 */

bool evo::CheckGlError(const char *funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("check error", "GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }

    return false;
}











