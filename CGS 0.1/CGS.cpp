//////////////////////////////////////////////////////////////////////////
//��ÿһ��ס�ڴ�ɽ��С��鶼��һ��
//������Ϸ����
//CreateGameStudio
//2015��11��4��10:20:15-QuarterColor&MagicSquare
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
//����PNGͼƬ
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
	//ȡ��ԴͼƬ��Ⱥ͸߶� 
	int srcWidth = bmpSrc->GetWidth();
	int srcHeight = bmpSrc->GetHeight();
	//����������������ű��� 
	float scaleH = (float)destWidth / srcWidth;
	float scaleV = (float)destHeight / srcHeight;
	//�����Ҫ���ֳ���ȣ�����������ͳһ���ý�С�����ű��� 
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
	//����Ŀ���� 
	destWidth = (int)(srcWidth * scaleH);
	destHeight = (int)(srcHeight * scaleV);
	//����Ŀ��Bitmap 
	Gdiplus::Bitmap * bmpDest = new Gdiplus::Bitmap(destWidth, destHeight, PixelFormat32bppARGB);
	Gdiplus::Graphics graphic(bmpDest);
	//���ò�ֵ�㷨 
	graphic.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	//��Դͼ����Ƶ�Ŀ��ͼ���� 
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

	//���ԭʼͼƬ�Ĵ�С
	int srcWidth = srcimg->getWidth();
	int srcHeight = srcimg->getHeight();

	//�����ԭʼͼƬ�Խ��ߵľ��룬��ΪĿ��ͼƬ�ĳ��Ϳ�
	int dstWidth = sqrt(srcWidth*srcWidth + srcHeight*srcHeight);
	int dstHeight = dstWidth;

	if (dstimg->pngImage != NULL)
		delete dstimg->pngImage;

	//����ԭʼͼƬ�Խ��߳��ȣ�����Ŀ��ͼƬ���Ϳ��������Ƴ�����ת��ͼƬ�����ᱻ��������������
	dstimg->pngImage = new Gdiplus::Bitmap(dstWidth, dstHeight, PixelFormat32bppARGB);

	Gdiplus::Graphics graphic(dstimg->pngImage);

	Pen * pen = new Pen(Color::Yellow);//�����ߣ�������
									   //graphic.DrawRectangle(pen, 0, 0, dstWidth - 1, dstHeight - 1);//�����ߣ������á����Ƴ���ת��ͼƬ�ı߽�

	Matrix m;//����任�õľ������
	PointF cp(REAL(srcWidth / 2.0f), REAL(srcHeight / 2.0f));//center point����ת������
	PointF mp(REAL((dstWidth - srcWidth) / 2.0f), (dstHeight - srcHeight) / 2.0f);

	//ע�⣬����任��˳���Ƿ��ģ�Ҳ����˵����������Ч���൱������ת�����ƶ���
	m.Translate(mp.X, mp.Y);//Ч��Ϊ���Ƚ�ԭʼͼƬ���ĵ��ƶ���Ŀ��ͼƬ�����ġ�
	m.RotateAt(angle, cp);//Ч��Ϊ������Ŀ��ͼƬ�����ĵ������ת��
	graphic.SetTransform(&m);//���þ���任�õľ��������

							 //��ͼ��ͬʱ���о���任
	graphic.DrawImage(srcimg->pngImage, Gdiplus::Rect(0, 0, srcWidth, srcHeight),
		0, 0, srcWidth, srcHeight, Gdiplus::UnitPixel);
	//graphic.DrawRectangle(pen, 0, 0, srcWidth - 1, srcHeight - 1);//�����ߣ������á����Ƴ�ԭʼͼƬ��Ŀ��ͼ���е�λ�á�

	delete pen;
}
//////////////////////////////////////////////////////////////////////////
//��ʼ��������
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
//��ɫ��
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
//������
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
