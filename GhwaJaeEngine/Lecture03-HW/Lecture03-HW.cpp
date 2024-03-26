#include <iostream>
#pragma region 수상한 코드
#pragma warning(disable:4146)
#include <Windows.h>
#define cimg_use_jpeg
#include "CImg.h"
using namespace cimg_library;
using namespace std;
#pragma endregion
enum ForeColor {
    DEFAULT = 0,
    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37,
};
#pragma region 수상한 코드
// .,'`^:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$
const char* ContrastMap = " .,'`^:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

typedef struct {
    enum ForeColor colorTag;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} DefineColor;

DefineColor ConsoleColors[] = {
    {ForeColor::BLACK, 0, 0, 0},
    {ForeColor::BLUE, 0, 0, 255},
    {ForeColor::GREEN, 0, 255, 0},
    {ForeColor::CYAN, 0, 255, 255},
    {ForeColor::RED, 255, 0, 0},
    {ForeColor::MAGENTA, 255, 0, 255},
    {ForeColor::YELLOW, 255, 255, 0},
    {ForeColor::WHITE, 255, 255, 255}
};

void PrintImageFast(CImg<unsigned char> origin);
void PrintImage(CImg<unsigned char> origin);

DefineColor FindColorByColorCode(ForeColor colorCode) 
{
    for (int i = 0; i < sizeof(ConsoleColors) / sizeof(ConsoleColors[0]); i++)
    {
        if (ConsoleColors[i].colorTag == colorCode)
            return ConsoleColors[i];
    }
    return ConsoleColors[0];
}

CImg<unsigned char> GetGrayScaleImage(CImg<unsigned char> origin)
{
    CImg<unsigned char> grayWeight(origin.width(), origin.height(), 1, 1, 0);

    cimg_forXY(origin, x, y) {
        int R = static_cast<int>(origin(x, y, 0, 0));
        int G = static_cast<int>(origin(x, y, 0, 1));
        int B = static_cast<int>(origin(x, y, 0, 2));

        // Luma coding을 이용한 grayscale 변환법
        // 밝기 정보(Luma)만을 추출하여 grayscale 이미지를 생성한다.
        // 인간의 시각이 가장 민감한 녹색을 크게 반영하여 계산한다.
        int grayValueWeight = (int)(0.299 * R + 0.587 * G + 0.114 * B);

        grayWeight(x, y, 0, 0) = grayValueWeight;
    }
    
    return grayWeight;
}

CImg<unsigned char> InitializeImage(const char* address, int targetWidth) {
    CImg<unsigned char> origin(address);

    float ratio = (float)origin.height() / (float)origin.width();
    int resizedHeight = static_cast<int>(targetWidth * ratio);
    int resizedWidth = targetWidth * 2;

    origin.resize(resizedWidth, resizedHeight);

    return origin;
}

CImg<unsigned char> GetSourceImage(const char* address, int targetWidth) {
    CImg<unsigned char> origin(address);

    float ratio = (float)origin.height() / (float)origin.width();
    int resizedHeight = static_cast<int>(targetWidth * ratio);
    int resizedWidth = targetWidth * 2;

    origin.resize(resizedWidth, resizedHeight);

    PrintImageFast(origin);

    return origin;
}

void PrintImageWithColor(CImg<unsigned char> origin, ForeColor colorCode) {
    CImg<unsigned char> grayWeight = GetGrayScaleImage(origin);

    CImg<unsigned char> colored(grayWeight.width(), grayWeight.height(), 1, 3, 0);

    cimg_forXY(grayWeight, x, y) {
        int GRAY = static_cast<int>(grayWeight(x, y, 0, 0));

        DefineColor defineColor = FindColorByColorCode(colorCode);

        colored(x, y, 0, 0) = GRAY * (defineColor.r) / 256.0f * 256.0f;
        colored(x, y, 0, 1) = GRAY * (defineColor.g) / 256.0f * 256.0f;
        colored(x, y, 0, 2) = GRAY * (defineColor.b) / 256.0f * 256.0f;
    }
    //7 10 11 14 15 18 19
    // 색상 확장 시퀀스 문자열 길이: 19
    PrintImage(colored);
}

void PrintImageFast(CImg<unsigned char> origin) 
{
    CImg<unsigned char> grayWeight = GetGrayScaleImage(origin);

    //7 10 11 14 15 18 19
    // 색상 확장 시퀀스 문자열 길이: 19
    const int PIXEL_SPACE = 20;

    const int BUFFER_WITDH_SIZE = origin.width() * PIXEL_SPACE + 1;

    char* buffer = static_cast<char*>(malloc(sizeof(char) * BUFFER_WITDH_SIZE * origin.height()));

    for (int y = 0; y < origin.height(); y++)
    {
        for (int x = 0; x < origin.width(); x++)
        {
            int pixelSizedX = x * PIXEL_SPACE;

            unsigned char R = origin(x, y, 0, 0);
            unsigned char G = origin(x, y, 0, 1);
            unsigned char B = origin(x, y, 0, 2);

            int GRAY = static_cast<int>(grayWeight(x, y, 0, 0));

            int index = static_cast<int>(static_cast<int>(GRAY) / 256.0f * strlen(ContrastMap));

            int count = 0;
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '\x1b';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '[';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '3';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '8';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '2';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';

            char colorBuffer[4];

            sprintf_s(colorBuffer, "%3d", static_cast<int>(R));
            for (int i = 0; i < 3; i++)
            {
                if (colorBuffer[i] == ' ')
                    buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '0';
                else
                    buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = colorBuffer[i];
            }
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';

            sprintf_s(colorBuffer, "%3d", static_cast<int>(G));
            for (int i = 0; i < 3; i++)
            {
                if (colorBuffer[i] == ' ')
                    buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '0';
                else
                    buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = colorBuffer[i];
            }
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ';';

            sprintf_s(colorBuffer, "%3d", static_cast<int>(B));
            for (int i = 0; i < 3; i++)
            {
                if (colorBuffer[i] == ' ')
                    buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = '0';
                else
                    buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = colorBuffer[i];
            }

            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = 'm';
            buffer[y * (BUFFER_WITDH_SIZE)+(pixelSizedX + count++)] = ContrastMap[index];
        }
        buffer[y * BUFFER_WITDH_SIZE + BUFFER_WITDH_SIZE - 1] = '\n';
    }
    buffer[origin.height() * BUFFER_WITDH_SIZE - 1] = '\0';
    cout << buffer << endl;
}

void PrintImage(CImg<unsigned char> origin) 
{
    CImg<unsigned char> grayWeight = GetGrayScaleImage(origin);

    for (int y = 0; y < origin.height(); y++)
    {
        for (int x = 0; x < origin.width(); x++)
        {
            unsigned char R = origin(x, y, 0, 0);
            unsigned char G = origin(x, y, 0, 1);
            unsigned char B = origin(x, y, 0, 2);

            int GRAY = static_cast<int>(grayWeight(x, y, 0, 0));

            int index = static_cast<int>(static_cast<int>(GRAY) / 256.0f * strlen(ContrastMap));

            // cout << "\x1b[38;2;" << static_cast<int>(R) << ";" << static_cast<int>(G) << ";" << static_cast<int>(B) << "m" << ContrastMap[index];
            cout << "\x1b[38;2;" << static_cast<int>(R) << ";" << static_cast<int>(G) << ";" << static_cast<int>(B) << "m" << ContrastMap[index];
        }
        cout << "\n";
    }
}

#pragma endregion

int main()
{
#pragma region 수상한 코드
    const int TARGET_WIDTH = 50;
    CImg<unsigned char> source = InitializeImage("marioblock.jpg", TARGET_WIDTH);
#pragma endregion
    cout << "화면에 그림을 그리는 프로그램입니다." << endl;
    cout << "학번 : 202327005" << endl;
    cout << "이름 : 김동현" << endl;
    while (true)
    {
        cout << "\x1b[0m" << "화면에 그릴 물체코드를 입력하세요 : ";
        int input;
        cin >> input;

        if (input > 64 && input != 100 && input != 101) {
            cout << "올바른 코드를 입력하십시오." << endl;
            continue;
        }
#pragma region 수상한 코드
        if (input == 100) {
            source = GetSourceImage("professor_es.jpg", TARGET_WIDTH);
            continue;
        }
        if (input == 101) {
            source = GetSourceImage("marioblock.jpg", TARGET_WIDTH);
            continue;
        }
#pragma endregion
        if (input & 0b1) {
            PrintImageWithColor(source, ForeColor::WHITE);
        }
        if (input & 0b10) {
            PrintImageWithColor(source, ForeColor::RED);
        }
        if (input & 0b100) {
            PrintImageWithColor(source, ForeColor::GREEN);
        }
        if (input & 0b1000) {
            PrintImageWithColor(source, ForeColor::YELLOW);
        }
        if (input & 0b10000) {
            PrintImageWithColor(source, ForeColor::CYAN);
        }
        if (input & 0b100000) {
            PrintImageWithColor(source, ForeColor::MAGENTA);
        }
        if (input == 64)
            break;
    }
}