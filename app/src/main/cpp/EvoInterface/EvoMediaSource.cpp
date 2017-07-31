#include "EvoMediaSource.h"

void EvoFreeFrame(EvoFrame ** frame)
{
	if (frame != NULL)
	{
		if (*frame != NULL)
		{
			av_free(*frame);
		}
		*frame = NULL;
	}
}

EvoMediaSource::EvoMediaSource()
	:context_(NULL),
	packet_(NULL),
	videoIndex_(-1),
	videoStream_(NULL),
	filter(NULL),
	pps_data_(NULL),
	pps_size_(0),
	sps_data_(NULL),
	sps_size_(0)
{
}

EvoMediaSource::~EvoMediaSource()
{
	Close();
}

int EvoMediaSource::Open(const char * file)
{
	av_register_all();
	filter = av_bitstream_filter_init("h264_mp4toannexb");

	context_ = avformat_alloc_context();

	AVDictionary* options = NULL;
	//调整探测头避免过长的等待
	av_dict_set(&options, "max_analyze_duration", "100", 0);
	av_dict_set(&options, "probesize", "1024", 0);
	int ret = avformat_open_input(&context_,file,NULL, &options);
	if (ret != 0)
	{
		if (context_ != NULL)
		{
			avformat_close_input(&context_);
		}
		context_ = NULL;
		return ret;
	}

	for (size_t i = 0; i < context_->nb_streams; i++)
	{
		if (context_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream_ = context_->streams[i];
			AVCodec * codec = GetBestVideoDecoder(videoStream_->codec->codec_id);
			if (codec != NULL)
			{
				videoStream_->codec->codec = codec;
			}
		}
	}

	context_->probesize = 1024;
	context_->max_analyze_duration = 5 * AV_TIME_BASE;
	if (avformat_find_stream_info(context_, NULL))
	{
		if (context_ != NULL)
		{
			avformat_close_input(&context_);
		}
		context_ = NULL;
		return -1;
	}

	videoIndex_ = -1;
	for (size_t i = 0; i < context_->nb_streams; i++)
	{
		if (context_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoIndex_ = (int)i;
			videoStream_ = context_->streams[i];
			if (videoStream_ != NULL && videoStream_->codec != NULL && videoStream_->codec->codec != NULL)
			{
				printf("EvoMediaSource DECODE:%s\n", videoStream_->codec->codec->name);
			}
		}
	}
	
	if (videoIndex_ == -1 || AV_CODEC_ID_H264 != videoStream_->codec->codec_id)
	{
		if (context_ != NULL) 
		{
			avformat_close_input(&context_);
		}
		context_ = NULL;
		videoStream_ = NULL;
		return -1;
	}

	packet_ = (AVPacket*)av_malloc(sizeof(AVPacket));
	memset(packet_,0,sizeof(AVPacket));
	av_init_packet(packet_);

	AnalysisVideoPPSSPS();
	return 0;
}

void EvoMediaSource::Close()
{
	if (context_ != NULL)
	{
		avformat_close_input(&context_);
	}
	if (packet_ != NULL)
	{
		av_free_packet(packet_);
		av_freep(packet_);
	}
	context_ = NULL;
	packet_ = NULL;
	videoIndex_ = -1;
	videoStream_ = NULL;

	if (filter != NULL) 
	{
		av_bitstream_filter_close(filter);
		filter = NULL;
	}
	if (pps_data_ != NULL)
	{
		av_free(pps_data_);
		pps_data_ = NULL;
	}
	if (sps_data_ != NULL)
	{
		av_free(sps_data_);
		sps_data_ = NULL;
	}
	pps_size_ = 0;
	sps_size_ = 0;
}

int EvoMediaSource::Seek(int millisecond)
{
	if (context_ == NULL) return -1;
	int64_t timeStamp = millisecond * 1000;
	int ret = av_seek_frame(context_,-1, timeStamp, AVSEEK_FLAG_BACKWARD);
	if (ret >= 0)
	{
		avformat_flush(this->context_);
		return 0;
	}
	return ret;
}

int EvoMediaSource::ReadFrame(EvoFrame** out)
{
	if (context_ == NULL) return -1;
	bool getPacket = false;
	do {
		int ret = av_read_frame(context_, packet_);
		if (ret == 0)
		{
			if (packet_->stream_index == videoIndex_)
			{
				//读取成功
				if (out != NULL)
				{
					//av_bitstream_filter_filter(filter, videoStream_->codec, NULL, &packet_->data, &packet_->size, packet_->data, packet_->size, 0);

					EvoFrame * frame = (EvoFrame*)av_malloc(sizeof(EvoFrame) + packet_->size);
					frame->pts = packet_->pts;
					frame->dts = packet_->dts;
					frame->timestamp = (packet_->pts != AV_NOPTS_VALUE) ? (packet_->pts * av_q2d(videoStream_->time_base) * 1000) :
						(packet_->dts != AV_NOPTS_VALUE) ? (packet_->dts * av_q2d(videoStream_->time_base) * 1000) : NAN;
					frame->flags = packet_->flags;
					frame->size = packet_->size;
					memcpy(frame->data, packet_->data, frame->size);
					*out = frame;
					getPacket = true;
				}
			}
		}

		if (packet_ != NULL) 
		{
			av_packet_unref(packet_);
		}
		if (getPacket)
		{
			break;
		}
		if (ret == AVERROR_EOF)
		{
			//文件结束
			return ret;
		}
	} while (true);
	return 0;
}

int EvoMediaSource::GetExtData(uint8_t * data, int size)
{
	if (videoStream_ == NULL) return 0;
	if (videoStream_->codec == NULL) return 0;

	if (data == NULL)
	{
		return videoStream_->codec->extradata_size;
	}
	if (size < videoStream_->codec->extradata_size)
	{
		return videoStream_->codec->extradata_size;
	}
	memcpy(data, videoStream_->codec->extradata, videoStream_->codec->extradata_size);
	
	return videoStream_->codec->extradata_size;
}

//PPS
int EvoMediaSource::GetPPS(uint8_t * data, int size)
{
	if (videoStream_ == NULL) return 0;
	if (pps_size_ <= size && pps_data_ != NULL && data != NULL)
	{
		memcpy(data,pps_data_,pps_size_);
	}
	return pps_size_;
}

//SPS
int EvoMediaSource::GetSPS(uint8_t * data, int size)
{
	if (videoStream_ == NULL) return 0;
	if (sps_size_ <= size && sps_data_ != NULL && data != NULL)
	{
		memcpy(data, sps_data_, sps_size_);
	}
	return sps_size_;
}

int EvoMediaSource::GetDuration()
{
	if (context_ == NULL) return 0;
	return (int)context_->duration/1000;
}

int EvoMediaSource::GetFrameRate()
{
	if (videoStream_ == NULL) return 0;
	return videoStream_->r_frame_rate.num;
}

int EvoMediaSource::GetFrameCount()
{
	if (videoStream_ == NULL) return 0;
	return videoStream_->nb_frames;
}

//宽
int EvoMediaSource::GetWidth()
{
	if (videoStream_ == NULL) return 0;
	if (videoStream_->codec == NULL) return 0;
	return videoStream_->codec->width > 0 ?
		videoStream_->codec->width : videoStream_->codec->coded_width;
}

//高
int EvoMediaSource::GetHeight()
{
	if (videoStream_ == NULL) return 0;
	if (videoStream_->codec == NULL) return 0;
	return videoStream_->codec->height > 0 ?
		videoStream_->codec->height : videoStream_->codec->coded_height;
}

AVStream * EvoMediaSource::GetVideoStream()
{
	return videoStream_;
}

int EvoMediaSource::AnalysisVideoPPSSPS()
{
	if (videoStream_ == NULL) return -1;
	if (videoStream_->codec == NULL) return -1;

	int extradata_size = videoStream_->codec->extradata_size;
	uint8_t * extradata = videoStream_->codec->extradata;

	/* retrieve sps and pps NAL units from extradata */
	{
		uint16_t unit_size;
		uint64_t total_size = 0;
		uint8_t unit_nb, sps_done = 0, sps_seen = 0, pps_seen = 0;
		int unit_type = 0;
		extradata = extradata + 4;  //跳过前4个字节  

		/* retrieve length coded size */
		int length_size = (*extradata++ & 0x3) + 1;    //用于指示表示编码数据长度所需字节数  
		if (length_size == 3)
			return AVERROR(EINVAL);

		if (sps_data_ != NULL)
		{
			av_free(sps_data_);
			sps_data_ = NULL;
		}
		sps_size_ = 0;
		if (pps_data_ != NULL)
		{
			av_free(pps_data_);
			pps_data_ = NULL;
		}
		pps_size_ = 0;
		
		/* retrieve sps and pps unit(s) */
		unit_nb = *extradata++ & 0x1f; /* number of sps unit(s) */
		if (!unit_nb) {
			goto pps;
		}
		else {
			sps_seen = 1;
		}

		while (unit_nb--) {
			unit_size = extradata[0];
			unit_size = (unit_size  << 8) + extradata[1];
			total_size += unit_size + 4;
			if (total_size > INT_MAX - FF_INPUT_BUFFER_PADDING_SIZE ||
				extradata + 2 + unit_size > extradata + extradata_size) {
				return AVERROR(EINVAL);
			}
			unit_type = *(extradata + 2) & 0x1f;
			if (unit_type == 7)
			{
				//SPS
				if (sps_data_ == NULL)
				{
					sps_data_ = (uint8_t*)av_malloc(unit_size + 4);
					sps_data_[0] = 0x00;
					sps_data_[1] = 0x00;
					sps_data_[2] = 0x00;
					sps_data_[3] = 0x01;
					memcpy(sps_data_ + 4, extradata + 2, unit_size);
					sps_size_ = unit_size + 4;
				}
				
			}
			else if (unit_type == 8)
			{
				//PPS
				if (pps_data_ == NULL)
				{
					pps_data_ = (uint8_t*)av_malloc(unit_size + 4);
					pps_data_[0] = 0x00;
					pps_data_[1] = 0x00;
					pps_data_[2] = 0x00;
					pps_data_[3] = 0x01;
					memcpy(pps_data_ + 4, extradata + 2, unit_size);
					pps_size_ = unit_size + 4;
				}
			}

			extradata += 2 + unit_size;
		pps:
			if (!unit_nb && !sps_done++) {
				unit_nb = *extradata++; /* number of pps unit(s) */
				if (unit_nb)
					pps_seen = 1;
			}
		}
	}
	return 0;
}

AVCodec *EvoMediaSource::GetBestVideoDecoder(AVCodecID id) {
	if (VideoDecoderName.length() == 0) return NULL;
	
	AVCodec *c_temp = av_codec_next(NULL);
	while (c_temp != NULL) {
		if (c_temp->id == id && c_temp->type == AVMEDIA_TYPE_VIDEO 
			&& c_temp->decode != NULL)
		{
			printf("Video H264 decode:%s\n",c_temp->name);
		}
		c_temp = c_temp->next;
	}

	std::string name = VideoDecoderName;
	std::string decoder;
	while (true) {
		size_t pos = name.find(" ");
		if (pos != -1) {
			decoder = name.substr(0, pos);
			name = name.substr(pos + 1);
		}
		else {
			decoder = name;
			name = "";
		}
		if (decoder.length() > 0) {
			AVCodec * codec = avcodec_find_decoder_by_name(decoder.c_str());
			if (codec != NULL && codec->id == id) return codec;
		}
		if (name.length() == 0) break;
	}
	return NULL;
}

void EvoMediaSource::SetVideoCodecName(const char * codec)
{
	VideoDecoderName = codec;
}