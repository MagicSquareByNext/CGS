#pragma once

#include <easyx.h>
#include <graphics.h>
#include "GDIPlus/Includes/Gdiplus.h"


//��ʼ����Ϸ����x,y
void init_game(const int&, const int&);

//PNG������
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