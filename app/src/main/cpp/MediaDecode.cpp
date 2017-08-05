//
// Created by jephy on 7/17/17.
//

#include "MediaDecode.h"
#include "EvoInterface/sei_packet.h"
#include "android/log.h"

MediaDecode::MediaDecode()
:decoder(NULL),codecContent(NULL)
{
    time_base.num = 0;
    time_base.den = 1;
}

MediaDecode::~MediaDecode()
{
    if(decoder != NULL)
    {
        delete decoder;
        decoder = NULL;
    }
    if(codecContent != NULL)
    {
        avcodec_close(codecContent);
        avcodec_free_context(&codecContent);
    }
}

int MediaDecode::init()
{
    av_register_all();

    AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(codec == NULL) return -1;
    codecContent = avcodec_alloc_context3(codec);
    int ret = 0;
    if(ret = avcodec_open2(codecContent,codec,NULL) != 0)
    {
        char errbuf[64] = {0};
        av_strerror(ret, errbuf, 64);
        return -1;
    }

    decoder = new VideoDecoder(codecContent);

//    struct EvoVideoInfo info;
//    info.Width = 0;
//    info.Height = 0;
//    info.Format = AV_PIX_FMT_NONE;
//    struct EvoVideoInfo des = info;
//    des.Width = 800;
//    des.Height = 600;
//    des.Format = AV_PIX_FMT_RGBA;
//    convert.Initialize(info,des);
//    decoder->Attach(&convert);
    return 0;
}

int MediaDecode::decode(uint8_t * data, int32_t size)
{
    if (decoder == NULL) return -1;
    AVFrame * evoResult = NULL;
    int ret = 0;
    if(data != NULL && size > 0)
    {
        EvoPacket packet = {0};
        packet.data = data;
        packet.size = size;

        char buffer[256] = {0};
        int count = 256;
        ret = get_sei_content(data,size,buffer,&count);
//        __android_log_print(ANDROID_LOG_DEBUG,"JNI-DECODER","buffer = %s",buffer);

        if(ret > 0)
        {
            int cflags = 0;
            int64_t cpts = 0;
            int64_t cdts = 0;
            int64_t ctimestamp = 0;
            int ctime_base_num = 0;
            int ctime_base_den = 0;

            sscanf(buffer,"flags:%d pts:%llu dts:%llu timestamp:%llu time_base:num:%d den:%d",
                  &cflags,&cpts,&cdts,&ctimestamp,&ctime_base_num,&ctime_base_den);

            time_base.den = ctime_base_den;
            time_base.num = ctime_base_num;
            if(time_base.den == 0)
            {
                time_base.num = 0;
                time_base.den = 1;
            }

            packet.pts = cpts;
            packet.dts = cdts;
            packet.flags = cflags;
//            __android_log_print(ANDROID_LOG_DEBUG,"JNI-DECODER","pts:%lld dts:%lld",cpts,cdts);
        } else
        {
            packet.pts = AV_NOPTS_VALUE;
            packet.dts = AV_NOPTS_VALUE;
        }
        int64_t timeBegin = av_gettime()/1000;
        ret = decoder->DecodePacket(&packet,&evoResult);
        int64_t timeEnd = av_gettime() / 1000;
        __android_log_print(ANDROID_LOG_INFO,"native MediaDecode","use:%lld",timeEnd - timeBegin);

    } else{
        ret = decoder->DecodePacket((EvoPacket*)NULL,&evoResult);
    }
    if(evoResult != NULL)
    {
        int64_t timestamp = (evoResult->pts != AV_NOPTS_VALUE) ? (evoResult->pts * av_q2d(time_base) * 1000) :
                            (evoResult->pkt_pts != AV_NOPTS_VALUE) ? (evoResult->pkt_pts * av_q2d(time_base) * 1000) :
                            (evoResult->pkt_dts != AV_NOPTS_VALUE) ? (evoResult->pkt_dts * av_q2d(time_base) * 1000) : NAN;

        evoResult->pts = timestamp;
        SendPacket(evoResult);
        FreeAVFrame(&evoResult);
    }
    return ret;
}