#include "StdAfx.h"
#include "BackGround.h"
#include"resource.h"
BackGround::BackGround(void)
: speed(0)
, score(0)
, level(0)
{
}

BackGround::~BackGround(void)
{
}

void BackGround::draw(CDC* pDC)
{
	if (score >= 0 && score <= 500)
	{
		speed += 1;
		if (speed > 800)
			speed = 0;
		CBitmap bmp;
		bmp.LoadBitmapW(IDB_BITMAP15);
		CDC mDC;
		mDC.CreateCompatibleDC(pDC);
		mDC.SelectObject(bmp);
		pDC->BitBlt(0, speed, 1280, 800 - speed, &mDC, 0, 0, SRCCOPY);
		pDC->BitBlt(0, 0, 1280, speed, &mDC, 0, 800 - speed, SRCCOPY);
	}
	else if (score >= 500 && score <= 1000)
	{
		speed += 1;
		if (speed > 800)
			speed = 0;
		CBitmap bmp;
		bmp.LoadBitmapW(IDB_BITMAP19);
		CDC mDC;
		mDC.CreateCompatibleDC(pDC);
		mDC.SelectObject(bmp);
		pDC->BitBlt(0, speed, 1280, 800 - speed, &mDC, 0, 0, SRCCOPY);
		pDC->BitBlt(0, 0, 1280, speed, &mDC, 0, 800 - speed, SRCCOPY);
	}



}
