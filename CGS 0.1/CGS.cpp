//////////////////////////////////////////////////////////////////////////
//让每一个住在大山的小伙伴都有一个
//创造游戏的梦
//CreateGameStudio
//2015年11月4日10:20:15-QuarterColor&MagicSquare
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CGS.h"

int CGSScreenXY[2];

//////////////////////////////////////////////////////////////////////////
//时间
//////////////////////////////////////////////////////////////////////////

GameTime::GameTime()
{
	GameRunTime = GetTickCount();
}

GameTime::~GameTime()
{
}

inline void GameTime::SetStartTime()
{
	GameRunTime = GetTickCount();
}

inline void GameTime::GetNowTime()
{
	GamePlayTime = GetTickCount();
	GameNowTime = GamePlayTime - GameRunTime;
}

inline const LONGLONG & GameTime::ReturnStartTime()
{
	return GameRunTime;
}

inline const LONGLONG & GameTime::ReturnNowTime()
{
	return GameNowTime;
}

GameTime Gtime;
//////////////////////////////////////////////////////////////////////////
//Gdi+
//////////////////////////////////////////////////////////////////////////

static ULONG_PTR gdiplusToken;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
#define WCHAR_BUFFER_MAX 1024
static wchar_t wcharBuffer[WCHAR_BUFFER_MAX];

//////////////////////////////////////////////////////////////////////////
//处理PNG图片
//////////////////////////////////////////////////////////////////////////

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

void loadimage(IMAGE_PNG * image, const char* pImgFile, int nWidth, int nHeight)
{
	if (image == NULL || image->pngImage != NULL)
		return;

	wchar_t * wFileName = charToWChar((char*)pImgFile);

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

void putimage(IMAGE_PNG *image ,IMAGE *img,int dstX, int dstY)
{
	Gdiplus::Graphics * graphic = Gdiplus::Graphics::FromHDC(GetImageHDC(img));

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
	int dstWidth = (int)sqrt(srcWidth*srcWidth + srcHeight*srcHeight);
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
//元素类
//////////////////////////////////////////////////////////////////////////


element::element() :mHit(false), mKeyboardHit(false), mMouseHit(false), mMouseHover(false) {}


element::element(const int& x, const int& y, const int& Lenth, const int& Width) : mHit(false), mKeyboardHit(false), mMouseHit(false), mMouseHover(false)
{
	mXY[0] = x;
	mXY[1] = y;
	mSize[0] = Lenth;
	mSize[1] = Width;
}

void element::Putimage(const int& i, const int& x, const int& y)
{
	if (x == -1 && y != -1)
		putimage(mXY[0], y, &ElementImg[i]);
	else if (y == -1 && x != -1)
		putimage(x, mXY[1], &ElementImg[i]);
	else if (x == -1 && y == -1)
		putimage(mXY[0], mXY[1], &ElementImg[i]);
	else
		putimage(x, y, &ElementImg[i]);
}

void element::SetElementAnimation(const int & orderNumber, const int & start, const int & end)
{
	ElementAnimation[orderNumber][0] = start;
	ElementAnimation[orderNumber][1] = end;
}

void element::PlayElementAnimation(const int& orderNumber, int& cald, const int& type, const int& time)
{
	if (cald==0)
	{
		cal = ElementAnimation[orderNumber][0];
		TimeTemp = (LONGLONG)time / (ElementAnimation[orderNumber][1] - ElementAnimation[orderNumber][0]);
		cald = 1;
	}
	
	if (type == 0)
	{
		
		if (cal >= ElementAnimation[orderNumber][1] + 1) return;
		if (Gtime.ReturnNowTime() - AnimateStartTime >= TimeTemp*(cal-ElementAnimation[orderNumber][0]))
		{
			Putimage(mXY[0], mXY[1], cal);
			if (ElementAnimation[orderNumber][1] <= cal) cal++;
		}
		else Putimage(mXY[0], mXY[1], cal);
	}
	else if (type == 1)
	{
		if (cal >= ElementAnimation[orderNumber][1] + 1) cal = ElementAnimation[orderNumber][0];
		if (Gtime.ReturnNowTime() - AnimateStartTime >= TimeTemp*(cal - ElementAnimation[orderNumber][0]))
		{
			Putimage(mXY[0], mXY[1], cal);
			if (ElementAnimation[orderNumber][1] < cal) cal++;
		}
		else Putimage(mXY[0], mXY[1], cal);
	}
	else if (type == 2)
	{
		int count = (ElementAnimation[orderNumber][1] - ElementAnimation[orderNumber][0]);
		int temp = count - abs(count - cal);
		if (temp >= ElementAnimation[orderNumber][1] + 1) temp = ElementAnimation[orderNumber][0];
		if (Gtime.ReturnNowTime() - AnimateStartTime >= TimeTemp*(cal - ElementAnimation[orderNumber][0]))
		{
			Putimage(mXY[0], mXY[1], temp);
			if (ElementAnimation[orderNumber][1] < temp) cal++;
			if(temp>=count*2) cal = ElementAnimation[orderNumber][0];
		}
		else Putimage(mXY[0], mXY[1], cal);
	}
}

void element::Prepare()
{
	for (int j = 0; j < ElementImgCount; j++)
	{
		loadimage(&ElementImg[j], ElementImgUrl[j].c_str());
	}
}

string inline element::getstring(const int& n)
{
	std::stringstream newstr;
	newstr << n;
	return newstr.str();
}

void element::SetElementIMG(const string& url)
{
	ElementImgUrl[ElementImgCount] = url;
	ElementImgCount++;
}

void element::SetElementIMG(const string& url, const int& i)
{
	for (int j = 0; j < i; j++)
	{
		ElementImgUrl[ElementImgCount] = url + getstring(j) + ".png";
	}
	ElementImgCount = i;
}

void element::SetElementPosition(const int & x, const int & y)
{
	if (x > -1)
		mXY[0] = x;
	if (y > -1)
		mXY[1] = y;
}

void element::SetHit(const bool& hit)
{
	mHit = hit;
}

void element::SetMouseHit(const bool& hit)
{
	mMouseHit = hit;
}

void element::SetMouseHover(const bool& hit)
{
	mMouseHover = hit;
}

void element::SetKeyboardHit(const bool& hit)
{
	mKeyboardHit = hit;
}

const bool& element::IsHit()
{
	return mHit;
}

const bool& element::IsMouseHit()
{
	return mMouseHit;
}

const bool& element::IsMouseHover()
{
	return mMouseHover;
}

const bool& element::IsKeyboardHit()
{
	return mKeyboardHit;
}

const int& element::ReturnPositionX()
{
	return mXY[0];
}

const int& element::ReturnPositionY()
{
	return mXY[1];
}

const bool  element::IsOutscreen()
{
	if (mXY[0] > CGSScreenXY[0] || mXY[1] > CGSScreenXY[1] || mXY[0] < 0 || mXY[1] < 0)return true;
	return false;
}

const int& element::ReturnSizeLenth()
{
	return mSize[0];
}

const int& element::ReturnSizeWidth()
{
	return mSize[1];
}

element::~element()
{

}

//////////////////////////////////////////////////////////////////////////
//场景类
//////////////////////////////////////////////////////////////////////////

string inline scene::getstring(const int n)
{
	std::stringstream newstr;
	newstr << n;
	return newstr.str();
}

scene::scene(string str)
{
	m_str[count] = str;
	count++;
}

void scene::Prepare()
{
	for (int i = 0; i < count; i++)
	{
		loadimage(&img[i], m_str[i].c_str(), img[i].getwidth(), img[i].getheight());
	}
}

scene::scene(string str, int i, string file)
{
	for (int j = 0; j < i; j++)
	{
		m_str[j] = str + getstring(j) + '.' + file;
	}
	count = i;
}

void scene::PlayScene(int i)
{
	if (i >= count) return;
	putimage(0, 0, &img[i]);
}

void scene::PlayScene(const int& i, const int & x, const int & y)
{
	if (i >= count) return;
	putimage(x, y, &img[i]);
}

void scene::Play(void(*fun)())
{
	for (int j = 0; j < count; j++)
	{
		putimage(0, 0, &img[j]);
		fun();
	}
}

void scene::Play(void(*fun)(const int&))
{
	for (int j = 0; j < count; j++)
	{
		putimage(0, 0, &img[j]);
		fun(j);
	}
}

scene::~scene()
{
}

void scene::Pause()
{
	_getch();
}


//////////////////////////////////////////////////////////////////////////
//DEBUG
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//视频
//////////////////////////////////////////////////////////////////////////


video::video(string address, int time) :s(address), i(time) {}

void video::Play()
{
	SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_SHOWWINDOW);
	MCIWndOpen(hwnd, s.c_str(), NULL);
	MCIWndPlay(hwnd);
	Sleep(i);
	MCIWndClose(hwnd);
	MCIWndDestroy(hwnd);
	hwnd = NULL;
	cleardevice();
}

video::~video()
{
}

//////////////////////////////////////////////////////////////////////////
//音乐
//////////////////////////////////////////////////////////////////////////

music::music(const string& address, const string& alias)
{
	this->address = "open " + address + " type MPEGVideo alias " + alias;
	this->mPlay = "play " + alias;
	this->mPause = "pause " + alias;
	this->mStop = "stop " + alias;
	this->mClose = "close " + alias;
}

void music::Prepare()
{
	mciSendString(address.c_str(), NULL, 0, NULL);
}

void music::Play()
{
	mciSendString(mPlay.c_str(), NULL, 0, NULL);
}

void music::Pause()
{
	mciSendString(mPause.c_str(), NULL, 0, NULL);
}

void music::Stop()
{
	mciSendString(mStop.c_str(), NULL, 0, NULL);
}

void music::Close()
{
	mciSendString(mClose.c_str(), NULL, 0, NULL);
}

music::~music()
{
}

template<int N>
void music::ListPrepare(music* (&mu_list)[N])
{
	for (int i = 0; i < N; i++)
	{
		if (mu_list[i])
		{
			mu_list[i]->Prepare();
		}
	}
}

void music::CloseAll()
{
	mciSendString("close all", NULL, 0, NULL);
}

//////////////////////////////////////////////////////////////////////////
//基础类
//by 黄日晟 完成时间：2015年11月12日
//////////////////////////////////////////////////////////////////////////

base::base()
{
}

const int base::AddElementTrack(int id, element* ElementA, int VK)
{
	if (ElementList==NULL)
	{
		ElementList = new TrackElement(id, NULL, ElementA, SetVK(VK));
		return id;
	}

	TrackElement* P = ElementList;
	while (P->nAfter)
	{
		P = P->nAfter;
	}
	ElementList = new TrackElement(id, NULL, ElementA, SetVK(VK));
	ElementListCount++;
	return id;
}

const int base::AddElementTrack(int id, element* ElementA)
{
	if (ElementList == NULL)
	{
		ElementList = new TrackElement(id, NULL, ElementA);
		return id;
	}

	TrackElement* P = ElementList;
	while (P->nAfter)
	{
		P = P->nAfter;
	}
	P->nAfter = new TrackElement(id, P, ElementA);
	ElementListCount++;
	return id;
}

const int base::AddElementTrack(int id, element *ElementA, element *ElementB, bool & b)
{
	if (ElementList == NULL)
	{
		ElementList = new TrackElement(id, NULL, ElementA, ElementB, b);
		return id;
	}
	TrackElement* P = ElementList;
	while (P->nAfter)
	{
		P = P->nAfter;
	}
	P->nAfter = new TrackElement(id, P, ElementA, ElementB, b);
	ElementListCount++;
	return id;
}

void base::Baserun(const int& time)
{
	CleanKeyboardHit();
	FlushBatchDraw();
	Sleep(time);
	if(MouseHit())
		m = GetMouseMsg();
	GetKeyboardHit();
	Gtime.GetNowTime();
	track();
	cleardevice();
	BeginBatchDraw();
}
const char& base::KeyBoardHit() const
{
	return key;
}
const MOUSEMSG& base::Mouse() const
{
	return m;
}

void base::track()
{
	TrackElement *p=ElementList;
	while (p)
	{
		if (p->ElementTrackStyle == 1)
		{
			if (p->Element[0]->ReturnPositionX() >= p->Element[1]->ReturnPositionX() && p->Element[0]->ReturnPositionX() >= (p->Element[1]->ReturnPositionX() + p->Element[1]->ReturnSizeLenth())) *p->ElementBool = false;
			else if (p->Element[0]->ReturnPositionX() <= p->Element[1]->ReturnPositionX() && p->Element[1]->ReturnPositionX() >= (p->Element[0]->ReturnPositionX() + p->Element[0]->ReturnSizeLenth())) *p->ElementBool = false;
			else if (p->Element[0]->ReturnPositionY() >= p->Element[1]->ReturnPositionY() && p->Element[0]->ReturnPositionY() >= (p->Element[1]->ReturnPositionY() + p->Element[1]->ReturnSizeWidth())) *p->ElementBool = false;
			else if (p->Element[0]->ReturnPositionY() <= p->Element[1]->ReturnPositionY() && p->Element[1]->ReturnPositionY() >= (p->Element[0]->ReturnPositionY() + p->Element[0]->ReturnSizeWidth())) *p->ElementBool = false;
			*p->ElementBool = true;
		}
		else if (p->ElementTrackStyle == 2)
		{
			if (m.x >= p->Element[0]->ReturnPositionX() &&
				m.x <= p->Element[0]->ReturnPositionX() + p->Element[0]->ReturnSizeLenth() &&
				m.y >= p->Element[0]->ReturnPositionY() &&
				m.y <= p->Element[0]->ReturnPositionY() + p->Element[0]->ReturnSizeWidth())
			{
				p->Element[0]->SetMouseHover(true);
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					p->Element[0]->SetMouseHit(true);
				}
			}
			else
			{
				p->Element[0]->SetMouseHover(false);
				p->Element[0]->SetMouseHit(false);
			}
		}
		else if (p->ElementTrackStyle == 3)
		{
			if (VK_CODE[1][p->ElementPlace] == 1)
			{
				p->Element[0]->SetKeyboardHit(true);
			}
			else
			{
				p->Element[0]->SetKeyboardHit(false);
			}
		}
		p = p->nAfter;
	}
}

const int base::SetVK(int VK)
{
	for (int i=0;i < VK_CODE_Count;i++)
	{
		if (VK_CODE[0][i] == VK) return i;
	}
	VK_CODE[0][VK_CODE_Count] = VK;
	VK_CODE[1][VK_CODE_Count] = 0;
	VK_CODE_Count++;
	return VK_CODE_Count - 1;
}

void base::DelElementTrack(const int & i)
{
	TrackElement* p = ElementList;
	while (p)
	{
		if (p->id == i)
		{
			delete p;
		}
	}
	ElementListCount--;
}

void base::CleanKeyboardHit()
{
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	key = NULL;
}

void base::GetKeyboardHit()
{
	for (int i = 0;i < VK_CODE_Count;i++)
	{
		if (VK_CODE[0][i]!=0)
		{
			VK_CODE[1][i]=((GetAsyncKeyState(VK_CODE[0][i]) & 0x8000) ? 1 : 0);
		}
	}
	//if (_kbhit()) key = _getch();
}

void base::InitGame(const int& x, const int& y)
{
	initgraph(x, y);
	CGSScreenXY[0] = x;
	CGSScreenXY[1] = y;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void base::CloseGame()
{
	if (gdiplusToken)
		Gdiplus::GdiplusShutdown(gdiplusToken);
	closegraph();
}

base::~base()
{
	TrackElement *p = ElementList;
	if(p)
		while (p->nAfter)
		{
			p = p->nAfter;
		}
	else
	{
		return;
	}
	while (p != ElementList)
	{
		p = p->nBefore;
		delete p->nAfter;
	}
	delete ElementList;
	return;
}

TrackElement::TrackElement(int id, TrackElement * before, element * thisElement)
{
	this->id = id;
	Element[0] = thisElement;
	Element[1] = NULL;
	ElementTrackStyle = 2;
	ElementBool = NULL;
	ElementPlace = NULL;
	nBefore = before;
	before->nAfter = this;
	nAfter = NULL;
}

TrackElement::TrackElement(int id, TrackElement * before, element *thisElement,int place)
{
	this->id = id;
	Element[0] = thisElement;
	Element[1] = NULL;
	ElementTrackStyle = 3;
	ElementBool = NULL;
	ElementPlace = place;
	nBefore = before;
	before->nAfter = this;
	nAfter = NULL;
}

TrackElement::TrackElement(int id, TrackElement *before, element *thisElementA, element *thisElementB, bool b)
{
	this->id = id;
	Element[0] = thisElementA;
	Element[1] = thisElementB;
	ElementTrackStyle = 1;
	ElementPlace = NULL;
	ElementBool = &b;
	nBefore = before;
	before->nAfter = this;
	nAfter = NULL;
}

TrackElement::~TrackElement()
{
	id = NULL;
	Element[0] = NULL;
	Element[1] = NULL;
	ElementPlace = NULL;
	ElementTrackStyle = NULL;
	ElementBool = NULL;
	if (nBefore != NULL) 
		nBefore->nAfter = nAfter;
	if (nAfter != NULL)
		nAfter->nBefore = nBefore;
	nBefore = NULL;
	nAfter = NULL;
}


AnimationList::~AnimationList()
{
}

void AnimationList::Play()
{
	if (!now)
		return;
	putimage(0, 0, now->img);
	now=now->nAfter;
}

ImageList & AnimationList::CreateIL()
{
	if (!ImageLoadList)
	{
		ImageLoadList = new ImageList;
		ImageLoadList->nAfter = NULL;
		ImageLoadList->nBefore = NULL;
		ImageLoadList->img = new IMAGE(1280, 720);
		now = ImageLoadList;
		count++;
		return *ImageLoadList;
	}
	ImageList *p;
	p = ImageLoadList;
	while (p)
	{
		p = p->nAfter;
	}
	p->nAfter = new ImageList;
	p->nAfter->nBefore = p;
	p->nAfter->nAfter = NULL;
	p->nAfter->img = new IMAGE(1280, 720);
	count++;
	p->nAfter->n = count;
	return *p;
}

void AnimationList::Prepare()
{
	SetWorkingImage(CreateIL().img);
	nowRender++;
}

const int & AnimationList::ReturnNowRender()
{
	return nowRender;
	// TODO: 在此处插入 return 语句
}

void AnimationList::Goto(int i)
{
	ImageList *p = now;
	if (p->n==i)
	{
		return;
	}
	else if (p->n<i)
	{
		while (p)
		{
			if (p->n == i)
			{
				now = p;
				return;
			}
			p = p->nBefore;
		}
	}
	else
	{
		while (p)
		{
			if (p->n == i)
			{
				now = p;
				return;
			}
			p = p->nAfter;
		}
	}
	return;
}

IMAGE & AnimationList::NowIMAGE()
{
	return *now->img;
	// TODO: 在此处插入 return 语句
}

void AnimationList::ControlAnimation(int n)
{
	if (STATE_CA == 0)
	{
		START_Time = Gtime.ReturnNowTime();
		STATE_CA = 1;
	}
	if (STATE_CA == 0)
	{
		NOW_Time_A = Gtime.ReturnNowTime();
		STATE_CA = 2;
	}
	NOW_Time_B = Gtime.ReturnNowTime();
	preCount++;
	if (NOW_Time_B-NOW_Time_A>=1000)
	{
		STATE_CA = 1;
		if (preCount<n)
		{
			Goto(nowRender + n - preCount);
			nowRender += n - preCount;
		}
		preCount = 0;
	}
}

void AnimationList::EndPrepare()
{
	SetWorkingImage();
}

