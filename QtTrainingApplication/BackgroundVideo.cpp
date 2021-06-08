#include "BackgroundVideo.h"

BackgroundVideo::BackgroundVideo(QString path, QOpenGLWidget* widget)
{
	const QString audioPath = path;
    
    AVFormatContext* formatContext = avformat_alloc_context();                                    //分配fomat上下文
    if (avformat_open_input(&formatContext, audioPath.toLocal8Bit().data(), nullptr, nullptr) != 0) {  //打开输入流
        qDebug() << "can`t open the file.";
        return;
    }
    av_dump_format(formatContext, 0, audioPath.toLocal8Bit(), 0);                                  //在终端打印
    if (avformat_find_stream_info(formatContext, nullptr) != 0) {                                   //加载输入流中的信息
        qDebug() << "can`t find stream infomation";
        return;
    }
    videoStreamIndex = -1;
    for (uint i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {                //查找第一个视频流（一个视频中可能有多个流（音频+视频））
            videoStreamIndex = i;
            break;
        }
    }
    if (videoStreamIndex == -1) {
        qDebug() << "didn`t find a video stream";
        return;
    }
    AVCodecParameters* codecParam = formatContext->streams[videoStreamIndex]->codecpar;           //获取编解码器的参数集
    AVCodec* codec = avcodec_find_decoder(codecParam->codec_id);                                  //获取编解码器
    codecContext = avcodec_alloc_context3(nullptr);                               //获取编解码上下文
    avcodec_parameters_to_context(codecContext, codecParam);                                     //根据编解码器参数填充编解码上下文

    if (codec == nullptr) {
        qDebug() << "can`t find codec.";
        return;
    }
    if (avcodec_open2(codecContext, codec, nullptr) != 0) {                                          //开启编解码器
        qDebug() << "can`t open codec";
        return;
    }

    AVPacket* packet = av_packet_alloc();                                                       //分配一个数据包

    AVFrame* frame = av_frame_alloc();                                                            //分配一个视频帧

    int outputLineSize[4];                                                                         //构造AVFrame到QImage所需要的数据
    av_image_fill_linesizes(outputLineSize, AV_PIX_FMT_RGB32, codecParam->width);
    //构造一个格式转换上下文
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
            QImage output(codecParam->width, codecParam->height, QImage::Format_RGB32);                  //构造一个QImage用作输出
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
