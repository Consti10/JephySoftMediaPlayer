#include "VideoDecoder.h"

#define LOGA printf


void FreeAVFrame(AVFrame **out)
{
	if (out == NULL) return;
	AVFrame *frame = *out;
	if (frame != NULL)
	{
		if (frame->data[0] != NULL)
		{
			av_freep(&frame->data[0]);
		}
	}
	av_frame_free(out);
	*out = NULL;
}

VideoDecoder::VideoDecoder(AVCodecContext	*codec)
	:VideoCodecCtx(codec)
	, Convert(NULL)
	, KeepIFrame(false)
{
	this->VideoFrame = av_frame_alloc();
	this->Packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	memset(this->Packet,0,sizeof(AVPacket));
	av_init_packet(this->Packet);
}

VideoDecoder::~VideoDecoder()
{
	if (this->VideoFrame != NULL) {
		av_frame_free(&this->VideoFrame);
		this->VideoFrame = NULL;
	}
	if (this->Packet != NULL)
	{
		av_free_packet(this->Packet);
		av_free(this->Packet);
		this->Packet = NULL;
	}
}

void VideoDecoder::Attach(EvoVideoConvert *convert)
{
	Convert = convert;
}

EvoVideoConvert *VideoDecoder::Detach()
{
	EvoVideoConvert *old = Convert;
	Convert = NULL;
	return old;
}

int VideoDecoder::DecodePacket(EvoFrame *packet, AVFrame **evoResult)
{
    int ret = 0;
    if(packet != NULL)
    {
        ret = av_new_packet(this->Packet, packet->size);

        if (ret == 0) {
            memcpy(this->Packet->data, packet->data, packet->size);
            Packet->pts = packet->pts;
            Packet->dts = packet->dts;
            Packet->flags = packet->flags;
            ret = DecodePacket(Packet, evoResult);
        }
        av_packet_unref(this->Packet);
    } else{
        ret = DecodePacket((AVPacket *)NULL, evoResult);
    }

	return ret;
}

int VideoDecoder::DecodePacket(EvoPacket *packet, AVFrame **evoResult)
{
    int ret = 0;
    if(packet != NULL)
    {
        ret = av_new_packet(this->Packet, packet->size);

        if (ret == 0) {
            if(packet->data != NULL)
            {
                memcpy(this->Packet->data, packet->data, packet->size);
            }
            Packet->pts = packet->pts;
            Packet->dts = packet->dts;
            Packet->flags = packet->flags;
            ret = DecodePacket(Packet, evoResult);
        }

	    av_packet_unref(this->Packet);
    } else{
        ret = DecodePacket((AVPacket *)NULL, evoResult);
    }
	return ret;
}

int  VideoDecoder::DecodePacket(AVPacket *packet, AVFrame **evoResult)
{
	if (evoResult != NULL)
	{
		*evoResult = NULL;
	}

	if (packet == NULL) {
		return 0;
	}

	int gotFrame = 0;
	int decoded = avcodec_decode_video2(this->VideoCodecCtx, VideoFrame, &gotFrame, packet);
	if (decoded < 0) {
		if (decoded == AVERROR(EAGAIN)) return 0;
		char errbuf[1024] = { 0 };
		av_strerror(decoded, errbuf, 1024);
		LOGA("VideoDecoder::DecodePacket:avcodec_decode_video2:%d(%s).\n", decoded, errbuf);
		if (decoded == AVERROR_INVALIDDATA) return 0;
		if (decoded == AVERROR_EOF) return -1;
		if (decoded == AVERROR(EINVAL)) return -1;
		if (AVERROR(ENOMEM)) return -1;
		return -1;
	}

	if (gotFrame) {

		if (KeepIFrame)
		{
			if (VideoFrame->pict_type == AV_PICTURE_TYPE_P)
			{
				av_frame_unref(this->VideoFrame);
				return 0;
			}
			else if (VideoFrame->pict_type == AV_PICTURE_TYPE_B)
			{
				av_frame_unref(this->VideoFrame);
				return 0;
			}else if (VideoFrame->pict_type == AV_PICTURE_TYPE_I)
			{
				//����
				KeepIFrame = false;
			}
			else 
			{
				//ֱ�Ӵ����
				KeepIFrame = false;
			}
		}

		struct EvoVideoInfo info = { 0,0,AV_PIX_FMT_NONE };
		EvoVideoConvert *tmpConvert = Convert;
		if (tmpConvert != NULL)
		{
			info = tmpConvert->GetTargetInfo();
		}
		else
		{
			if (this->VideoCodecCtx != NULL)
			{
				info.Width = this->VideoCodecCtx->width;
				info.Height = this->VideoCodecCtx->height;
				info.Format = this->VideoCodecCtx->pix_fmt;
			}
		}

		AVFrame * retData = av_frame_alloc();
		int retSize = CreateFrame(retData, info.Width, info.Height, info.Format);

		if (retSize <= 0)
		{
			LOGA("VideoDecoder::DecodePacket:EvoPacketAllocator::CreateAVFrame(%d,%d,%d)==NULL.\n"
				, info.Width, info.Height, info.Format);

			FreeFrame(&retData);
			av_frame_unref(this->VideoFrame);
			return -1;
		}
		else 
		{
			AVFrame* desFrame = retData;
			if (tmpConvert != NULL)
			{
				tmpConvert->Convert(VideoFrame, desFrame);
			}
			else
			{
				//�����Ƶ��Ϣ
				desFrame->width = info.Width;
				desFrame->height = info.Height;
				desFrame->format = info.Format;
				//��������
				int ret = av_frame_copy(desFrame, VideoFrame);
				
				if (ret < 0)
				{

					LOGA("VideoDecoder::DecodePacket:EvoPacketAllocator::av_frame_copy==(%d).\n"
						, ret);
					FreeFrame(&retData);
					av_frame_unref(this->VideoFrame);
					return -1;
				}
			}
            desFrame->width = info.Width;
            desFrame->height = info.Height;
            desFrame->format = info.Format;

			desFrame->pkt_pts = VideoFrame->pkt_pts;
			desFrame->pkt_dts = VideoFrame->pkt_dts;
			desFrame->pts = VideoFrame->pts;
            desFrame->pkt_size = retSize;

			if (evoResult != NULL)
			{
				*evoResult = retData;
			}
			else
			{
				FreeFrame(&retData);
			}

			av_frame_unref(this->VideoFrame);

			return 1;
		}
	}

	return 0;
}

int VideoDecoder::CreateFrame(AVFrame *out,int Width, int Height, AVPixelFormat Format)
{
	return av_image_alloc(out->data, out->linesize,Width, Height, Format, 1);
}

void VideoDecoder::FreeFrame(AVFrame **out)
{
	FreeAVFrame(out);
}