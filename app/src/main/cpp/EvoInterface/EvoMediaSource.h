#ifndef EVOMEDIASOURCE_H
#define EVOMEDIASOURCE_H

#include "EvoHeader.h"
#include <string>


struct EvoFrame {
	int64_t pts;
	int64_t dts;
	int64_t timestamp;
	int flags;
	int size;
	uint8_t data[0];
};

void EvoFreeFrame(EvoFrame ** frame);

class EvoMediaSource
{
public:
	EvoMediaSource();
	~EvoMediaSource();
	//打开文件
	//返回值:0:成功 !0:失败
	int Open(const char * file);
	//结束关闭
	void Close();
	//跳转,单位:毫秒
	//返回值:0:成功 !0:失败
	int Seek(int millisecond);
	//读取帧
	//返回值:0:成功 AVERROR_EOF:文件结束 !0:失败
	int ReadFrame(EvoFrame** out);
	//获取扩展数据
	//返回值:扩展数据大小
	int GetExtData(uint8_t * data, int size);
	//PPS
	int GetPPS(uint8_t * data, int size);
	//SPS
	int GetSPS(uint8_t * data, int size);
	//时长
	//返回值:单位:毫秒
	int GetDuration();
	//帧率
	int GetFrameRate();
	//帧数
	int GetFrameCount();
	//宽
	int GetWidth();
	//高
	int GetHeight();
	AVStream * GetVideoStream();
	void SetVideoCodecName(const char * codec);
private:
	int AnalysisVideoPPSSPS();
	AVCodec *GetBestVideoDecoder(AVCodecID id);
private:
	AVFormatContext * context_;
	AVPacket * packet_;
	int videoIndex_;
	AVStream *videoStream_;
	AVBitStreamFilterContext* filter;
	uint8_t * pps_data_;
	int pps_size_;
	uint8_t * sps_data_;
	int sps_size_;
	std::string VideoDecoderName;
};


#endif