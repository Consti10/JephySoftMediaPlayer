//
// Created by li on 16/4/1.
//
// Get from medium-iOS project zhuyuanxuan 2016/11/18.
// Modify some code by zhuyuanxuan.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef FTPLISTLOADING_RENDER_H
#define FTPLISTLOADING_RENDER_H

#include <vector>
#include <string>

#ifdef __ANDROID__

#include <GLES3/gl3.h>

#else
#include <OpenGLES/ES3/gl.h>
#endif

namespace evo {

    using namespace std;

    class GLProgram {
    private:
        bool compileShader(GLuint *shader, GLenum type, const char *shaderSource);

        GLuint _vertShader;
        GLuint _fragShader;
        GLuint _program;
        vector<string> _attributes;

    public:
        GLProgram();

        ~GLProgram();

        GLuint getVertShader() const {
            return _vertShader;
        }

        GLuint getFragShader() const {
            return _fragShader;
        }

        GLuint getProgram() const {
            return _program;
        }

        // return false if OK\nTrue if error.
        bool buildProgram(const char *vertexShaderSource, const char *fragmentShaderSource);

        void use();

        void unuse();

        // return false if OK\nTrue if error.
        bool link();

        void validateProgram();

        void init();

        void release();

        void addAtributeButton(const char *attributeName);

        GLuint attributeIndex(const char *attributeName);

        GLint uniformIndex(const char *uniformName);

        GLuint setArrayBuffer(float *bufferArray, int bufferSize);

        GLuint setElementBuffer(GLuint *bufferArray, int bufferSize);

        GLint attributePosition(const char *attributeName);
    };

    // return true if get error info\n
    // false if no error
    bool CheckGlError(const char *funcName);

}

#endif
// FTPLISTLOADING_RENDER_H
