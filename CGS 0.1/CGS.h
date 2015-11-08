#pragma once

#include <easyx.h>
#include <graphics.h>
#include <iostream>
#include <math.h>
#include <string>
#include "GDIPlus/Includes/Gdiplus.h"
#include <string>
#include <Vfw.h>

#pragma comment(lib, "./GDIPlus/Gdiplus.lib")
#pragma comment (lib, "Vfw32.lib")
#pragma comment( lib, "MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")

using namespace std;
using namespace Gdiplus;

//初始化游戏窗口x,y
void init_game(const int&, const int&);

//PNG处理类
struct IMAGE_PNG
{
public:
	int getHeight();
	int getWidth();
	IMAGE_PNG();
	virtual ~IMAGE_PNG();
	Gdiplus::Bitmap * pngImage;
	float angle;
};

void putimage(int dstX, int dstY, IMAGE_PNG *image);
void loadimage(IMAGE_PNG * image, const char* pImgFile, int nWidth=0, int nHeight=0);

class Music
{
public:
	Music(string, string);
	~Music();
	void prepare();
	void play();
private:
	string address;
	string alias;
};

template<int N>
void music_list_prepare(Music* (&mu_list)[N])
{
	for (int i = 0;i < N;i++)
	{
		if (mu_list[i])
		{
			mu_list[i]->prepare();
		}
	}
}