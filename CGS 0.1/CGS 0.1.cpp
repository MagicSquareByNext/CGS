// CGS 0.1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <easyx.h>
#include "CGS.h"
int main()
{
	init_game(1280, 720);
	POINT position = { 0 };//����ͼƬ���Ƶ�λ��
	float angle = 0.0f;//����ĽǶ�
	int width = 0;//ͼƬ��
	int height = 0;//ͼƬ��
	IMAGE_PNG img_man;//����ͼƬ
	IMAGE_PNG img_man_rotated;//�����ת�������ͼƬ
	loadimage(&img_man, "man.png");
	BeginBatchDraw();//������ͼ��ʼ
							  //putimage(position.x, position.y, &img_man);
	putimage(position.x, position.y, &img_man);//������ת���ͼƬ
	EndBatchDraw();//������ͼ����
	while (true)
	{

	}
    return 0;
}

