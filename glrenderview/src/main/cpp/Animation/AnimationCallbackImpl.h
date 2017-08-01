//
// Created by zhuyuanxuan on 02/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_ANIMATIONCALLBACKIMPL_H
#define EVOCLIENTFORANDROID_ANIMATIONCALLBACKIMPL_H

#include "../RenderControl/BaseRenderController.h"
#include "../RenderControl/SphereRenderController.h"

namespace evo {
    class AnimationCallbackResetImpl {
    public:
        AnimationCallbackResetImpl(BaseRenderController *m_renderer);

        virtual ~AnimationCallbackResetImpl();

        int Start();

        int Stop();

    private:
        BaseRenderController *m_renderer;
    };

    class AnimationCallbackInertiaImpl {
    public:
        AnimationCallbackInertiaImpl(BaseRenderController *renderer);

        virtual ~AnimationCallbackInertiaImpl();

        int OnStart();

        int OnStop();

    private:
        BaseRenderController *m_renderer;
    };

    class AnimationCallbackTransformImpl {
    public:
        AnimationCallbackTransformImpl(SphereRenderController *renderer,
                DisplayMode mode);

        virtual ~AnimationCallbackTransformImpl();

        int OnStart();

        int OnStop();

    private:
        SphereRenderController *m_renderer;
        DisplayMode m_next_mode;
    };
}

#endif //EVOCLIENTFORANDROID_ANIMATIONCALLBACKIMPL_H
