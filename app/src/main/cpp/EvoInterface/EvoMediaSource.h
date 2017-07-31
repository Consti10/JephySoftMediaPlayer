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
	//���ļ�
	//����ֵ:0:�ɹ� !0:ʧ��
	int Open(const char * file);
	//�����ر�
	void Close();
	//��ת,��λ:����
	//����ֵ:0:�ɹ� !0:ʧ��
	int Seek(int millisecond);
	//��ȡ֡
	//����ֵ:0:�ɹ� AVERROR_EOF:�ļ����� !0:ʧ��
	int ReadFrame(EvoFrame** out);
	//��ȡ��չ����
	//����ֵ:��չ���ݴ�С
	int GetExtData(uint8_t * data, int size);
	//PPS
	int GetPPS(uint8_t * data, int size);
	//SPS
	int GetSPS(uint8_t * data, int size);
	//ʱ��
	//����ֵ:��λ:����
	int GetDuration();
	//֡��
	int GetFrameRate();
	//֡��
	int GetFrameCount();
	//��
	int GetWidth();
	//��
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