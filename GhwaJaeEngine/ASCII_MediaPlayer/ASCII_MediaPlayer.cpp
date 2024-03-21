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

AVFormatContext* fmtCtx;

int main(void) {

    av_log(NULL, AV_LOG_INFO, "AVFromat major version = %d\n", LIBAVFORMAT_VERSION_MAJOR);

    return 0;

}