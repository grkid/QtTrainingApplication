#include "BackgroundVideo.h"

BackgroundVideo::BackgroundVideo(QString path, QOpenGLWidget* widget)
{
	const QString audioPath = path;
    
    AVFormatContext* formatContext = avformat_alloc_context();                                    //����fomat������
    if (avformat_open_input(&formatContext, audioPath.toLocal8Bit().data(), nullptr, nullptr) != 0) {  //��������
        qDebug() << "can`t open the file.";
        return;
    }
    av_dump_format(formatContext, 0, audioPath.toLocal8Bit(), 0);                                  //���ն˴�ӡ
    if (avformat_find_stream_info(formatContext, nullptr) != 0) {                                   //�����������е���Ϣ
        qDebug() << "can`t find stream infomation";
        return;
    }
    videoStreamIndex = -1;
    for (uint i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {                //���ҵ�һ����Ƶ����һ����Ƶ�п����ж��������Ƶ+��Ƶ����
            videoStreamIndex = i;
            break;
        }
    }
    if (videoStreamIndex == -1) {
        qDebug() << "didn`t find a video stream";
        return;
    }
    AVCodecParameters* codecParam = formatContext->streams[videoStreamIndex]->codecpar;           //��ȡ��������Ĳ�����
    AVCodec* codec = avcodec_find_decoder(codecParam->codec_id);                                  //��ȡ�������
    codecContext = avcodec_alloc_context3(nullptr);                               //��ȡ�����������
    avcodec_parameters_to_context(codecContext, codecParam);                                     //���ݱ�����������������������

    if (codec == nullptr) {
        qDebug() << "can`t find codec.";
        return;
    }
    if (avcodec_open2(codecContext, codec, nullptr) != 0) {                                          //�����������
        qDebug() << "can`t open codec";
        return;
    }

    AVPacket* packet = av_packet_alloc();                                                       //����һ�����ݰ�

    AVFrame* frame = av_frame_alloc();                                                            //����һ����Ƶ֡

    int outputLineSize[4];                                                                         //����AVFrame��QImage����Ҫ������
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, codecParam->width);
    //����һ����ʽת��������
    SwsContext* imgConvertContext = sws_getContext(codecParam->width, codecParam->height, (AVPixelFormat)codecParam->format, codecParam->width, codecParam->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    int index = 0;
    int frameCountInner = 0;

    while (true) {
        if (av_read_frame(formatContext, packet) < 0)
            break;
        if (packet->stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, packet) != 0)
            {
                qDebug() << "avcodec_send_packet continue";
                continue;
            }
            if (avcodec_receive_frame(codecContext, frame) != 0)
            {
                qDebug() << "avcodec_receive_frame continue";
                continue;
            }
            QImage output(codecParam->width, codecParam->height, QImage::Format_RGB32);                  //����һ��QImage�������
            uint8_t* outputDst[] = { output.bits() };
            sws_scale(imgConvertContext, frame->data, frame->linesize, 0, codecParam->height, outputDst, outputLineSize);
            images.append(output);
            /*if (frameCountInner % frameSaveInterval == 0)
            {
                contexts.append(formatContext);
            }*/
            frameCountInner++;
        }
    }

    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);

    frameCount = frameCountInner;
}

BackgroundVideo::~BackgroundVideo()
{
}

QSize BackgroundVideo::getSize()
{
	return QSize();
}

void BackgroundVideo::__debug_out()
{
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
}
