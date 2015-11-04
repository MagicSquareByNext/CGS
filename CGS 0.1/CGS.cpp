//////////////////////////////////////////////////////////////////////////
//让每一个住在大山的小伙伴都有一个
//创造游戏的梦
//CreateGameStudio
//2015年11月4日10:20:15-QuarterColor&MagicSquare
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CGS.h"
#include <string>
using namespace std;
#define MAX_IMAGE_LIST 100
#include <iostream>
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//Gdi+
//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "./GDIPlus/Gdiplus.lib")
#include "./GDIPlus/Includes/Gdiplus.h"

using namespace Gdiplus;

static ULONG_PTR gdiplusToken;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
#define WCHAR_BUFFER_MAX 1024
static wchar_t wcharBuffer[WCHAR_BUFFER_MAX];

//////////////////////////////////////////////////////////////////////////
//处理PNG图片
//////////////////////////////////////////////////////////////////////////

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

IMAGE_PNG::IMAGE_PNG()
{
	pngImage = NULL;
	angle = 0.0f;
}

IMAGE_PNG::~IMAGE_PNG()
{
	if (pngImage != NULL)
		delete pngImage;
}

int IMAGE_PNG::getWidth()
{
	return pngImage->GetWidth();
}

int IMAGE_PNG::getHeight()
{
	return pngImage->GetHeight();
}

wchar_t * charToWChar(const char * ss)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ss, -1, NULL, 0);
	if (nLen == 0 || (nLen * sizeof(wchar_t)) >= WCHAR_BUFFER_MAX)
	{
		return NULL;
	}

	int ret = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ss, -1, wcharBuffer, nLen);

	return wcharBuffer;
}

Gdiplus::Bitmap * ResizeBitmap(Gdiplus::Bitmap * bmpSrc, int destWidth, int destHeight, bool keepAspect = false)
{
	//取得源图片宽度和高度 
	int srcWidth = bmpSrc->GetWidth();
	int srcHeight = bmpSrc->GetHeight();
	//计算横向和纵向的缩放比率 
	float scaleH = (float)destWidth / srcWidth;
	float scaleV = (float)destHeight / srcHeight;
	//如果需要保持长宽比，则横向和纵向统一采用较小的缩放比率 
	if (keepAspect)
	{
		if (scaleH > scaleV)
		{
			scaleH = scaleV;
		}
		else
		{
			scaleV = scaleH;
		}
	}
	//计算目标宽高 
	destWidth = (int)(srcWidth * scaleH);
	destHeight = (int)(srcHeight * scaleV);
	//创建目标Bitmap 
	Gdiplus::Bitmap * bmpDest = new Gdiplus::Bitmap(destWidth, destHeight, PixelFormat32bppARGB);
	Gdiplus::Graphics graphic(bmpDest);
	//设置插值算法 
	graphic.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	//将源图像绘制到目标图像上 
	graphic.DrawImage(bmpSrc, Gdiplus::Rect(0, 0, destWidth, destHeight),
		0, 0, srcWidth, srcHeight, Gdiplus::UnitPixel);
	graphic.Flush();
	return bmpDest;
}

void loadimage(IMAGE_PNG * image, LPCTSTR pImgFile, int nWidth, int nHeight)
{
	if (image == NULL || image->pngImage != NULL)
		return;

	wchar_t * wFileName = charToWChar(pImgFile);

	std::wcout << wFileName << endl;

	image->pngImage = Gdiplus::Bitmap::FromFile(wFileName);

	if (nWidth != 0 && nHeight != 0)
	{
		Bitmap * backup = image->pngImage;

		image->pngImage = ResizeBitmap(image->pngImage, nWidth, nHeight);

		delete backup;
	}
}

void putimage(int dstX, int dstY, IMAGE_PNG *image)
{
	Gdiplus::Graphics * graphic = Gdiplus::Graphics::FromHDC(GetImageHDC());

	graphic->DrawImage(image->pngImage, dstX, dstY, image->getWidth(), image->getHeight());

	delete graphic;
}

void rotateimage(IMAGE_PNG *dstimg, IMAGE_PNG *srcimg, float angle)
{
	if (dstimg == NULL || srcimg == NULL)
		return;

	//获得原始图片的大小
	int srcWidth = srcimg->getWidth();
	int srcHeight = srcimg->getHeight();

	//计算出原始图片对角线的距离，作为目标图片的长和宽！
	int dstWidth = sqrt(srcWidth*srcWidth + srcHeight*srcHeight);
	int dstHeight = dstWidth;

	if (dstimg->pngImage != NULL)
		delete dstimg->pngImage;

	//按照原始图片对角线长度，设置目标图片长和宽（这样绘制出的旋转后图片，不会被“剪辑”掉！）
	dstimg->pngImage = new Gdiplus::Bitmap(dstWidth, dstHeight, PixelFormat32bppARGB);

	Gdiplus::Graphics graphic(dstimg->pngImage);

	Pen * pen = new Pen(Color::Yellow);//辅助线，测试用
									   //graphic.DrawRectangle(pen, 0, 0, dstWidth - 1, dstHeight - 1);//辅助线，测试用。绘制出旋转后图片的边界

	Matrix m;//矩阵变换用的矩阵变量
	PointF cp(REAL(srcWidth / 2.0f), REAL(srcHeight / 2.0f));//center point，旋转的中心
	PointF mp(REAL((dstWidth - srcWidth) / 2.0f), (dstHeight - srcHeight) / 2.0f);

	//注意，矩阵变换的顺序是反的！也就是说，下面代码的效果相当于先旋转，后移动！
	m.Translate(mp.X, mp.Y);//效果为：先将原始图片中心点移动到目标图片的中心。
	m.RotateAt(angle, cp);//效果为：再以目标图片的中心点进行旋转。
	graphic.SetTransform(&m);//设置矩阵变换用的矩阵变量。

							 //绘图，同时进行矩阵变换
	graphic.DrawImage(srcimg->pngImage, Gdiplus::Rect(0, 0, srcWidth, srcHeight),
		0, 0, srcWidth, srcHeight, Gdiplus::UnitPixel);
	//graphic.DrawRectangle(pen, 0, 0, srcWidth - 1, srcHeight - 1);//辅助线，测试用。绘制出原始图片在目标图框中的位置。

	delete pen;
}
//////////////////////////////////////////////////////////////////////////
//初始化、结束
//////////////////////////////////////////////////////////////////////////

void init_game(const int& x, const int& y)
{
	initgraph(x, y);
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void close_game()
{
	if (gdiplusToken)
		Gdiplus::GdiplusShutdown(gdiplusToken);
	closegraph();
}

//////////////////////////////////////////////////////////////////////////
//角色类
//////////////////////////////////////////////////////////////////////////

class Adt
{
public:
	Adt()
	{
		add_attribute = '0';
		m_value = 0;
	}
	Adt(string name,int value):add_attribute(name),m_value(value)
	{
		add_attribute = '0';
		m_value = 0;
	}
	~Adt()
	{

	}
	const string name()
	{
		return add_attribute;
	}
	void setValue(int value)
	{
		m_value = value;
	}
	void value(int& va)
	{
		va = m_value;
	}
private:
	string add_attribute;
	int m_value;
};

class character
{
public:
	character();
	character(int, string);
	~character();
	void AddAttribute(string, int);
	void changeAddAttribute(string, int);
	void SetCharacterIMG()
	{

	}
private:
	int searchAddAttribute(string);
	int m_id;
	string m_name;
	IMAGE character_img[MAX_IMAGE_LIST];
	int character_img_count;
	Adt* add_attribute[100];
	int add_att_count;
};

character::character() :m_id(0), m_name(0), add_att_count(0), character_img_count(0) {}

character::character(int id, string name) : m_id(id), m_name(name), add_att_count(0), character_img_count(0) {}

void character::AddAttribute(string name, int value)
{
	add_attribute[add_att_count] = new Adt(name,value);
	add_att_count++;
}

int character::searchAddAttribute(string name)
{
	for (int i = 0;i < add_att_count;i++)
	{
		if (add_attribute[i]->name() == name) return i;
	}
	return -1;
}

void character::changeAddAttribute(string name, int value)
{
	if (int q = searchAddAttribute(name) != -1)
		add_attribute[q]->setValue(value);
}

character::~character()
{

}

//////////////////////////////////////////////////////////////////////////
//场景类
//////////////////////////////////////////////////////////////////////////

class scene
{
public:
	scene();
	~scene();

private:

};

scene::scene()
{
}

scene::~scene()
{
}

//////////////////////////////////////////////////////////////////////////
//DEBUG
//////////////////////////////////////////////////////////////////////////
