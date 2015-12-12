//////////////////////////////////////////////////////////////////////////
//让每一个住在大山的小伙伴都有一个
//创造游戏的梦
//CreateGameStudio
//2015年11月4日10:20:15-QuarterColor&MagicSquare
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <easyx.h>
#include <graphics.h>
#include <Vfw.h>
#include <conio.h>
#include <math.h>
#include <string>
#include <WinUser.h>
#include <sstream>
#include <iostream>

#include "GDIPlus/Includes/Gdiplus.h"

#pragma comment(lib, "./GDIPlus/GdiPlus.lib")
#pragma comment (lib, "Vfw32.lib")
#pragma comment( lib, "MSIMG32.LIB")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"User32.lib")

#define MAX_IMAGE_LIST 100

using namespace std;
using namespace Gdiplus;


//////////////////////////////////////////////////////////////////////////
//时间类
//时间类，开始的时间节点为创建类的时间
//时间误差为55毫秒
//一般不需要自行创建，当游戏开始运行时便已经新建了一个时间类，变量名为：Gtime
//////////////////////////////////////////////////////////////////////////

class GameTime
{
public:
	GameTime();
	~GameTime();
	//设置开始时间，即从此刻开始重新计时
	void SetStartTime();
	//刷新时间
	void GetNowTime();
	//返回开始时间
	const LONGLONG& ReturnStartTime();
	//返回相对时间
	const LONGLONG& ReturnNowTime();
private:
	LONGLONG GameRunTime, GamePlayTime;
	LONGLONG GameNowTime;
};



//////////////////////////////////////////////////////////////////////////
//PNG类
//by strack 完成时间：2015年09月12日
//////////////////////////////////////////////////////////////////////////

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
void loadimage(IMAGE_PNG * image, const char* pImgFile, int nWidth = 0, int nHeight = 0);
void putimage(IMAGE_PNG *image,IMAGE *img,int dstX, int dstY);
//////////////////////////////////////////////////////////////////////////
//场景类
//by 黄日晟 完成时间：2015年11月10日
//////////////////////////////////////////////////////////////////////////

//场景类
class scene
{
public:
	//存入单张图片地址
	scene(string);
	//存入多张图片地址（图片文件夹，数量，文件类型）注意为开始为0
	scene(string, int, string);
	~scene();
	//加载场景内所有图片
	void Prepare();
	//播放场景（接受单个无参数函数）
	void Play(void(*fun)());
	//播放场景（接受单个含参数函数，传输数值为当前场景数）
	void Play(void(*fun)(const int&));
	//显示场景（场景数）
	void PlayScene(int);
	void PlayScene(const int& i, const int & x, const int& y);
	//按任意键继续
	static void Pause();
private:
	string inline getstring(const int n);

	IMAGE img[100];
	string m_str[100];
	int count = 0;
};



//////////////////////////////////////////////////////////////////////////
//元素类
//by 黄日晟 完成时间：2015年11月13日
//////////////////////////////////////////////////////////////////////////

//创建元素
class element
{
public:

	element();
	//创建新元素（包含初始化位置，以及元素大小）
	element(const int& x, const int& y, const int& Lenth, const int& Width);

	//	character(const character& charer, int x, int y);
	~element();

	//添加单张元素贴图（支持格式PNG）
	void SetElementIMG(const string& url);
	//添加多张元素贴图（支持格式PNG，注意文件初始标号为0）
	void SetElementIMG(const string& url, const int& amount);
	//设置元素位置
	void SetElementPosition(const int& x, const int& y);

	//加载贴图
	void Prepare();
	//打印指定贴图
	void Putimage(const int& orderNumber, const int& x = -1, const int& y = -1);

	//设置动画序列（在元素贴图中设置动画序列）
	void SetElementAnimation(const int& orderNumber, const int& start, const int& end);
	//播放动画序列（在元素贴图中设置动画序列）
	void PlayElementAnimation(const int& orderNumber, int& cald, const int& type = 0, const int& time = -1);

	//无用函数
	void SetHit(const bool&);
	//无用函数
	void SetMouseHit(const bool&);
	//无用函数
	void SetKeyboardHit(const bool&);
	//无用函数
	void SetMouseHover(const bool&);

	const bool& IsHit();
	//返回鼠标是否点击元素（bool）
	const bool& IsMouseHit();
	//返回鼠标是否在元素上（bool）
	const bool& IsMouseHover();
	//返回是否被绑定的按键击中（bool）
	const bool& IsKeyboardHit();

	//返回元素长
	const int& ReturnSizeLenth();
	//返回元素宽
	const int& ReturnSizeWidth();
	//返回元素坐标X
	const int& ReturnPositionX();
	//返回元素坐标Y
	const int& ReturnPositionY();

	//判断元素是否在屏幕以外
	const bool IsOutscreen();


private:
	string inline getstring(const int&);
	LONGLONG AnimateStartTime;
	bool mHit;
	bool mMouseHit;
	bool mKeyboardHit;
	bool mMouseHover;
	int mSize[2];
	int mXY[2];
	IMAGE_PNG ElementImg[MAX_IMAGE_LIST];
	string ElementImgUrl[MAX_IMAGE_LIST];
	int ElementImgCount = 0;
	int ElementAnimation[100][2];
	int cal;
	LONGLONG TimeTemp;
};

//////////////////////////////////////////////////////////////////////////
//音乐类
//用于播放音乐音效
//每一次定义，限一个音效/音乐
//在使用前请先使用Prepare函数或ListPrepare函数初始化
//结束请使用Close函数或CloseAll函数结束
//by 黄日晟 完成时间：2015年11月09日
//////////////////////////////////////////////////////////////////////////

class music
{
public:
	music(const string& url, const string& alias);
	~music();
	//加载
	void Prepare();
	//播放
	void Play();
	//暂停
	void Pause();
	//停止
	void Stop();
	//关闭
	void Close();
	//关闭全部
	static void CloseAll();
	template<int N>
	//加载一个数组的音乐
	static void ListPrepare(music* (&mu_list)[N]);
private:
	string address;
	string mPlay;
	string mPause;
	string mStop;
	string mClose;
};

//////////////////////////////////////////////////////////////////////////
//视频类
//by 黄日晟 完成时间：2015年11月14日
//////////////////////////////////////////////////////////////////////////

class video
{
public:
	video(string, int);
	~video();
	void Play();

private:
	int i;
	string s;
	HWND hwnd = MCIWndCreate(GetHWnd(), NULL, WS_CHILD | WS_VISIBLE | MCIWNDF_NOMENU | MCIWNDF_NOPLAYBAR, NULL);
};



//////////////////////////////////////////////////////////////////////////
//基础类
//基础类，主要控制游戏初始化结束以及控制游戏过程
//（如判断键盘输入、鼠标输入、是否碰撞等）
//by 黄日晟 完成时间：2015年11月12日
//////////////////////////////////////////////////////////////////////////

struct TrackElement
{
	TrackElement(int id, TrackElement * before, element * thisElement);
	TrackElement(int id, TrackElement*, element*,int);
	TrackElement(int id, TrackElement*, element*, element*, bool);
	~TrackElement();

	int id;
	element *Element[2];
	int ElementTrackStyle;
	bool* ElementBool;
	TrackElement* nBefore;
	TrackElement* nAfter;
	int ElementPlace;
};


//基础类
class base
{
public:
	base();
	~base();
	//初始化游戏（分辨率x，y）
	static void InitGame(const int& x, const int& y);
	//结束游戏过程
	static void CloseGame();
	//运行时计算函数
	void Baserun(const int& time);
	//返回键盘输入的字符（char型）
	const char& KeyBoardHit() const;
	//返回键盘输入的字符（MOUSEMSG型）
	const MOUSEMSG& Mouse() const;
	//添加追踪,判断元素是否被指定按键击中
	const int AddElementTrack(int id, element* Element, int VK);
	//添加追踪，判断元素是否被鼠标击中或覆盖
	const int AddElementTrack(int id, element* Element);
	//添加追踪，判断是否碰撞
	const int AddElementTrack(int id, element*, element*, bool&);
	
	//删除元素追踪
	void DelElementTrack(const int&);

private:
	void CleanKeyboardHit();
	void GetKeyboardHit();
	void track();
	const int SetVK(int VK);
	char key = NULL;
	TrackElement *ElementList=NULL;
	int ElementListCount = 0;
	MOUSEMSG m;
	int VK_CODE[2][100];
	int VK_CODE_Count;
};

struct ImageList
{
	IMAGE* img;
	ImageList* nBefore;
	ImageList* nAfter;
	int n;
};


//动画序列类(预渲染)
class AnimationList
{
public:
	AnimationList() :count(0),n(0),nowRender(0),preCount(0),STATE_CA(0){}
	~AnimationList();
	void Play();
	ImageList& CreateIL();
	void Prepare();
	void EndPrepare();
	const int& ReturnNowRender();
	void Goto(int i);
	IMAGE& NowIMAGE();
	void ControlAnimation(int n);
private:
	ImageList* ImageLoadList;
	ImageList* now;
	int count;
	int preCount;
	int n;
	int nowRender;
	int STATE_CA;
	long long START_Time;
	long long NOW_Time_A;
	long long NOW_Time_B;
};


