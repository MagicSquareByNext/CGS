// CGS 0.1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <easyx.h>
#include "CGS.h"
int main()
{
	init_game(1280, 720);
	POINT position = { 0 };//人物图片绘制的位置
	float angle = 0.0f;//人物的角度
	int width = 0;//图片宽
	int height = 0;//图片高
	IMAGE_PNG img_man;//人物图片
	IMAGE_PNG img_man_rotated;//存放旋转后的人物图片
	loadimage(&img_man, "man.png");
	BeginBatchDraw();//批量绘图开始
							  //putimage(position.x, position.y, &img_man);
	putimage(position.x, position.y, &img_man);//再贴旋转后的图片
	EndBatchDraw();//批量绘图结束
	while (true)
	{

	}
    return 0;
}

