#include <iostream>
#include <Windows.h>

// CImg
#pragma warning(disable:4146)
#define cimg_use_jpeg
#include "CImg.h"
using namespace cimg_library;
using namespace std;

// FFmpeg
extern "C"{
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libpostproc/postprocess.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "postproc.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil-58.dll")

#include <conio.h>

const char* ContrastMap = " .,'`^:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

int main(void) {
    int ret;
    AVFormatContext* fmtCtx = NULL;
    int vidx = -1, aidx = -1;
    AVStream* vStream, * aStream;
    AVCodecParameters* vPara, * aPara;
    const AVCodec* vCodec, * aCodec;
    AVCodecContext* vCtx, * aCtx;

    uint8_t* rgbbuf = NULL;
    SwsContext* swsCtx = NULL;

    ret = avformat_open_input(&fmtCtx, "Videos\\Zion.T-Unkown.mp4", NULL, NULL);
    if (ret != 0) { return -1; }

    avformat_find_stream_info(fmtCtx, NULL);

    vidx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    aidx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, vidx, NULL, 0);

    vStream = fmtCtx->streams[vidx];
    vPara = vStream->codecpar;
    vCodec = avcodec_find_decoder(vPara->codec_id);
    vCtx = avcodec_alloc_context3(vCodec);
    avcodec_parameters_to_context(vCtx, vPara);
    avcodec_open2(vCtx, vCodec, NULL);

    aStream = fmtCtx->streams[aidx];
    aPara = aStream->codecpar;
    aCodec = avcodec_find_decoder(aPara->codec_id);
    aCtx = avcodec_alloc_context3(aCodec);
    avcodec_parameters_to_context(aCtx, aPara);
    avcodec_open2(aCtx, aCodec, NULL);


    // 루프를 돌며 패킷을 모두 읽는다.

    int vcount = 0, acount = 0;
    AVPacket packet = { 0, };
    AVFrame RGBFrame = { 0, };
    AVFrame vFrame = { 0, }, aFrame = { 0, };
    char* buffer = NULL;

    bool allocFlag = true;

    while (av_read_frame(fmtCtx, &packet) == 0) {
        if (packet.stream_index == vidx) {
            ret = avcodec_send_packet(vCtx, &packet);
            if (ret != 0) { continue; }
            for (;;) {
                ret = avcodec_receive_frame(vCtx, &vFrame);
                if (ret == AVERROR(EAGAIN)) break;

                // 스케일 컨텍스트 생성
                int targetWidth = 100;
                float ratio = (float)vFrame.height / (float)vFrame.width;
                int resizedHeight = static_cast<int>(targetWidth * ratio);
                int resizedWidth = targetWidth * 2;
                RGBFrame.format = AV_PIX_FMT_RGB24;
                RGBFrame.width = resizedWidth;
                RGBFrame.height = resizedHeight;

                if (swsCtx == NULL)
                {
                    swsCtx = sws_getCachedContext(
                        NULL, vFrame.width, vFrame.height, AVPixelFormat(vFrame.format),
                        RGBFrame.width, RGBFrame.height, AVPixelFormat(RGBFrame.format),
                        SWS_BICUBIC, NULL, NULL, NULL);

                    // 변환 결과를 저장할 프레임 버퍼 할당
                    int rasterbufsize = av_image_get_buffer_size(AV_PIX_FMT_RGB24,
                        RGBFrame.width, RGBFrame.height, 1);
                    rgbbuf = (uint8_t*)av_malloc(rasterbufsize);
                    av_image_fill_arrays(RGBFrame.data, RGBFrame.linesize, rgbbuf,
                        AV_PIX_FMT_RGB24, RGBFrame.width, RGBFrame.height, 1);
                }

                sws_scale_frame(
                    swsCtx, 
                    &RGBFrame,
                    &vFrame);

                const int PIXEL_SPACE = 20;
                const int BUFFER_WITDH_SIZE = RGBFrame.width * PIXEL_SPACE + 1;
                
                if (allocFlag) {
                    buffer = static_cast<char*>(malloc(sizeof(char) * BUFFER_WITDH_SIZE * RGBFrame.height));
                    allocFlag = false;
                }
                // 압축 해제한 이미지 출력
                for (int y = 0; y < RGBFrame.height; y++) {
                    for (int x = 0; x < RGBFrame.width; x++) {
                        int R, G, B;
                        int offset = (y * RGBFrame.width + x) * 3;
                        R = (int)RGBFrame.data[0][offset + 0];
                        G = (int)RGBFrame.data[0][offset + 1];
                        B = (int)RGBFrame.data[0][offset + 2];

                        R = max(0, min(255, R));
                        G = max(0, min(255, G));
                        B = max(0, min(255, B));

                        int GRAY = (int)(0.299 * R + 0.587 * G + 0.114 * B);

                        int index = static_cast<int>(static_cast<int>(GRAY) / 256.0f * strlen(ContrastMap));

                        int pixelSizedX = x * PIXEL_SPACE;
                        int count = 0;
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '\x1b';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '[';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '3';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '8';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '2';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';

                        char colorBuffer[4];

                        sprintf_s(colorBuffer, "%3d", R);
                        for (int i = 0; i < 3; i++)
                        {
                            if (colorBuffer[i] == ' ')
                                buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '0';
                            else
                                buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = colorBuffer[i];
                        }
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';

                        sprintf_s(colorBuffer, "%3d", G);
                        for (int i = 0; i < 3; i++)
                        {
                            if (colorBuffer[i] == ' ')
                                buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '0';
                            else
                                buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = colorBuffer[i];
                        }
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';

                        sprintf_s(colorBuffer, "%3d", B);
                        for (int i = 0; i < 3; i++)
                        {
                            if (colorBuffer[i] == ' ')
                                buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '0';
                            else
                                buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = colorBuffer[i];
                        }

                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = 'm';
                        buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '@';
                    }
                    buffer[y * BUFFER_WITDH_SIZE + BUFFER_WITDH_SIZE - 1] = '\n';
                    buffer[RGBFrame.height * BUFFER_WITDH_SIZE - 1] = '\0';

                }
                printf("\x1b[0;0H");
                printf(buffer);
                av_packet_unref(&packet);
                Sleep(10);
            }
        }

    }
    printf("\x1b[0m");
    // 메모리 해제

    free(buffer);
    if (rgbbuf) { av_free(rgbbuf); rgbbuf = NULL; }
    if (swsCtx) { sws_freeContext(swsCtx); swsCtx = NULL; }

    av_frame_unref(&vFrame);
    av_frame_unref(&aFrame);
    avcodec_free_context(&vCtx);
    avcodec_free_context(&aCtx);
    avformat_close_input(&fmtCtx);
}