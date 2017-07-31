#ifndef MEDIACONVERT_H
#define MEDIACONVERT_H

#include "EvoHeader.h"

typedef struct EvoVideoInfo
{
	int Width;
	int Height;
	AVPixelFormat Format;
}EvoVideoInfo;

class EvoVideoConvert
{
public:
	EvoVideoConvert();
    ~EvoVideoConvert();
	bool CheckSource(const EvoVideoInfo &src);
	bool CheckTarget(const EvoVideoInfo &des);

	EvoVideoInfo GetTargetInfo();
	EvoVideoInfo GetSourceInfo();
	unsigned int GetTargetSize();

    bool Initialize(const EvoVideoInfo &src, const EvoVideoInfo &des);

	int Convert(const AVFrame* srcFrame, AVFrame* desFrame);

	static unsigned int GetSize(const EvoVideoInfo &info);
private:

	int Convert(const AVFrame* srcFrame, uint8_t* dstBuf);
	int Convert(unsigned char *srcBuf, unsigned char *dstBuf, bool doRotate);
    int GetYuvBuf(const AVFrame* srcFrame, const EvoVideoInfo &info, uint8_t* dstBuf);
	//ת���ֽ���
	void ChangeEndianPic(unsigned char *image, int w, int h, int bpp);
	void ChangeEndian32(unsigned char *data);
	void ChangeEndian24(unsigned char *data);
private:
    AVFrame * SrcFrame;
    AVFrame * DstFrame;
    struct SwsContext *SwsCtx;

	EvoVideoInfo SrcInfo;
	EvoVideoInfo DesInfo;
};

#endif