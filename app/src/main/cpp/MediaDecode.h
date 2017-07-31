//
// Created by jephy on 7/17/17.
//

#ifndef UVCCAMERACTRLDEMO_MEDIADECODE_H
#define UVCCAMERACTRLDEMO_MEDIADECODE_H

#include <pthread.h>
#include "EvoInterface/VideoDecoder.h"
#include "EvoInterface/EvoVideoConvert.h"

class MediaDecode {
public:
    MediaDecode();
    ~MediaDecode();
    int init();
    int decode(uint8_t * data, int32_t size);
    virtual void SendPacket(AVFrame * frame) = 0;
private:
    VideoDecoder *decoder;
    EvoVideoConvert convert;
    AVCodecContext	*codecContent;
    AVRational time_base;
};


#endif //UVCCAMERACTRLDEMO_MEDIADECODE_H
