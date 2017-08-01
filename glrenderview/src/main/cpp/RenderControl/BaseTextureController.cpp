//
// Created by zhuyuanxuan on 22/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "BaseTextureController.h"

/**
 * Base Texture Controller
 */

evo::BaseTextureController::BaseTextureController(BaseRenderController *renderer) :
        m_renderer{renderer} {
    m_texture_ok = false;
    for (int i = 0; i < 10; ++i) {
        m_texture_list[i] = 0;
    }
    m_texture_num = 0;
    m_create_texture_myself = true;
}

evo::BaseTextureController::~BaseTextureController() {
    m_renderer = 0;
}

void evo::BaseTextureController::SetRender(BaseRenderController *render) {
    m_renderer = render;
}

