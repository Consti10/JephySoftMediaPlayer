#include "EvoVideoConvert.h"

unsigned int EvoVideoConvert::GetSize(const EvoVideoInfo &info)
{
	unsigned int dstSize = avpicture_get_size(
		info.Format,
		info.Width,
		info.Height);
	return dstSize;
}

EvoVideoConvert::EvoVideoConvert()
	:SwsCtx(NULL), SrcFrame(NULL), DstFrame(NULL)
	, SrcInfo({ 0,0,AV_PIX_FMT_NONE })
	, DesInfo({ 0,0,AV_PIX_FMT_NONE })
{
}

EvoVideoConvert::~EvoVideoConvert()
{
    if (NULL != this->SrcFrame){
        av_frame_free(&(this->SrcFrame));
    }
	this->SrcFrame = NULL;

    if (NULL != this->DstFrame){
        av_frame_free(&(this->DstFrame));
    }
	this->DstFrame = NULL;

    if (NULL != this->SwsCtx){
		sws_freeContext(SwsCtx);
    }
	this->SwsCtx = NULL;
}

bool EvoVideoConvert::CheckSource(const EvoVideoInfo &src)
{
	if (SrcInfo.Format != src.Format) return false;
	if (SrcInfo.Width != src.Width) return false;
	if (SrcInfo.Height != src.Height) return false;
	return true;
}

bool EvoVideoConvert::CheckTarget(const EvoVideoInfo &des)
{
	if (DesInfo.Format != des.Format) return false;
	if (DesInfo.Width != 0 && DesInfo.Width != des.Width) return false;
	if (DesInfo.Height != 0 && DesInfo.Height != des.Height) return false;
	return true;
}

EvoVideoInfo EvoVideoConvert::GetTargetInfo()
{
	return this->DesInfo;
}

EvoVideoInfo EvoVideoConvert::GetSourceInfo()
{
	return this->SrcInfo;
}

unsigned int EvoVideoConvert::GetTargetSize()
{
	return GetSize(DesInfo);
}

bool EvoVideoConvert::Initialize(const EvoVideoInfo &src, const EvoVideoInfo &des)
{
    this->SrcInfo.Format = src.Format;
    this->SrcInfo.Height = src.Height;
    this->SrcInfo.Width = src.Width;
    this->DesInfo.Format = des.Format;
    this->DesInfo.Height = des.Height;
    this->DesInfo.Width = des.Width;

	if (SrcInfo.Format == AV_PIX_FMT_NONE
		|| SrcInfo.Width <= 0
		|| SrcInfo.Height <= 0)
	{
		return false;
	}

    if(DesInfo.Width == 0)
    {
        DesInfo.Width = SrcInfo.Width;
    }
    if(DesInfo.Height == 0)
    {
        DesInfo.Height = SrcInfo.Height;
    }

    this->SrcFrame = av_frame_alloc();
    if (NULL == this->SrcFrame){
        return false;
    }

    this->DstFrame = av_frame_alloc();
    if (NULL == this->DstFrame){
        return false;
    }

    this->SwsCtx = sws_getContext(
        this->SrcInfo.Width,
        this->SrcInfo.Height,
        this->SrcInfo.Format,
        this->DesInfo.Width,
        this->DesInfo.Height,
        this->DesInfo.Format,
        SWS_BICUBIC,
        NULL,
        NULL,
        NULL);
    if (NULL == this->SwsCtx){
        return false;
    }
    return true;
}


int EvoVideoConvert::Convert(const AVFrame* srcFrame, AVFrame*desFrame)
{
	return Convert(srcFrame,(uint8_t*)desFrame->data[0]);
}

int EvoVideoConvert::Convert(const AVFrame* srcFrame, uint8_t* dstBuf)
{
    if(this->SwsCtx == NULL)
    {
        if(this->SrcInfo.Format != srcFrame->format ||
                this->SrcInfo.Width != srcFrame->width ||
                this->SrcInfo.Height != srcFrame->height)
        {
            this->SrcInfo.Format = (AVPixelFormat)srcFrame->format;
            this->SrcInfo.Width = srcFrame->width;
            this->SrcInfo.Height = srcFrame->height;
            Initialize(this->SrcInfo,this->DesInfo);
        }
    }
    if(this->SwsCtx == NULL)
    {
        return -1;
    }

    if (this->DesInfo.Format == AV_PIX_FMT_YUV420P){
        if (this->SrcInfo.Format == this->DesInfo.Format 
			&& this->SrcInfo.Height == this->DesInfo.Height 
			&& this->SrcInfo.Width == this->DesInfo.Width)
		{
            this->GetYuvBuf(srcFrame,SrcInfo, dstBuf);
            return 1;
        }
    }

    int dstDataLen = 0;
    do
    {
        unsigned int dstSize = GetSize(DesInfo);

		av_image_fill_arrays(
			this->DstFrame->data,
			this->DstFrame->linesize,
			dstBuf,
			this->DesInfo.Format,
			this->DesInfo.Width,
			this->DesInfo.Height,
			1
			);

        memset(dstBuf, 0, sizeof(char)* dstSize);

        int ret = sws_scale(
            this->SwsCtx,
            (const uint8_t* const*)srcFrame->data,
            srcFrame->linesize,
            0,
            this->SrcInfo.Height,
            this->DstFrame->data,
            this->DstFrame->linesize
            );
        if (this->DesInfo.Height == ret){
            dstDataLen = dstSize;
        }

    } while (false);

    return dstDataLen;
}

int EvoVideoConvert::Convert(unsigned char *srcBuf, unsigned char *dstBuf, bool doRotate)
{
    int dstDataLen = 0;
    do
    {
		av_image_fill_arrays(
			this->SrcFrame->data,
			this->SrcFrame->linesize,
			srcBuf,
			this->SrcInfo.Format,
			this->SrcInfo.Width,
			this->SrcInfo.Height,
			1
			);

        this->SrcFrame->width = this->SrcInfo.Width;
        this->SrcFrame->height = this->SrcInfo.Height;

		unsigned int dstSize = GetSize(DesInfo);

		av_image_fill_arrays(
			this->DstFrame->data,
			this->DstFrame->linesize,
			dstBuf,
			this->DesInfo.Format,
			this->DesInfo.Width,
			this->DesInfo.Height,
			1
			);

        memset(dstBuf, 0, sizeof(char)* dstSize);

        //��תͼ��:���ҷ�ת
        if (doRotate){
            this->SrcFrame->data[0] += this->SrcFrame->linesize[0] * (this->SrcInfo.Height - 1);
            this->SrcFrame->linesize[0] *= -1;
            this->SrcFrame->data[1] += this->SrcFrame->linesize[1] * (this->SrcInfo.Height / 2 - 1);
            this->SrcFrame->linesize[1] *= -1;
            this->SrcFrame->data[2] += this->SrcFrame->linesize[2] * (this->SrcInfo.Height / 2 - 1);
            this->SrcFrame->linesize[2] *= -1;
        }

        int ret = sws_scale(
            this->SwsCtx,
            (const uint8_t* const*)this->SrcFrame->data,
            this->SrcFrame->linesize,
            0,
            this->SrcInfo.Height,
            this->DstFrame->data,
            this->DstFrame->linesize
            );
        if (this->DesInfo.Height == ret){
            dstDataLen = dstSize;
        }

    } while (false);

    return dstDataLen;
}

int EvoVideoConvert::GetYuvBuf(const AVFrame* srcFrame,const EvoVideoInfo &info, uint8_t* dstBuf)
{
    int height_half = info.Height / 2, width_half = info.Width/ 2;
    int y_wrap = srcFrame->linesize[0];
    int u_wrap = srcFrame->linesize[1];
    int v_wrap = srcFrame->linesize[2];

    uint8_t* y_buf = srcFrame->data[0];
    uint8_t* u_buf = srcFrame->data[1];
    uint8_t* v_buf = srcFrame->data[2];

    uint8_t* temp = dstBuf;
    memcpy(temp, y_buf, info.Width * info.Height);
    temp += info.Width * info.Height;
    memcpy(temp, u_buf, height_half * width_half);
    temp += height_half * width_half;
    memcpy(temp, v_buf, height_half * width_half);
    return 1;
}

void EvoVideoConvert::ChangeEndianPic(unsigned char *image, int w, int h, int bpp)
{
    unsigned char *pixeldata = NULL;
    for (int i = 0; i<h; i++)
    for (int j = 0; j<w; j++){
        pixeldata = image + (i*w + j)*bpp / 8;
        if (bpp == 32){
            ChangeEndian32(pixeldata);
        }
        else if (bpp == 24){
            ChangeEndian24(pixeldata);
        }
    }
}

//change endian of a pixel (32bit)  
void EvoVideoConvert::ChangeEndian32(unsigned char *data)
{
    char temp3, temp2;
    temp3 = data[3];
    temp2 = data[2];
    data[3] = data[0];
    data[2] = data[1];
    data[0] = temp3;
    data[1] = temp2;
}

void EvoVideoConvert::ChangeEndian24(unsigned char *data)
{
    char temp2 = data[2];
    data[2] = data[0];
    data[0] = temp2;
}