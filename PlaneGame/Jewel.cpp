#include "StdAfx.h"
#include "Jewel.h"
#include "resource.h"

CImageList CJewel::m_Images1;


CJewel::CJewel(void)
{
	m_ptPos.x = rand()%(GAME_WIDTH-JEWEL_HEIGHT)+3;

	m_nImgIndex = rand()%3;

	m_nMotion=1;
	m_ptPos.y=-JEWEL_HEIGHT;
	if (m_nImgIndex%2!=0)//如果是图像索引是偶数
	{
		m_nMotion=-1;//宝石从下方进入
		m_ptPos.y = GAME_HEIGHT+JEWEL_HEIGHT;
	}

	m_V = rand()%5+8;

	m_nWait=0;
}

CJewel::~CJewel(void)
{
}

BOOL CJewel::Draw(CDC* pDC,BOOL bPause)
{
	m_nWait++;
	if(m_nWait>15)
		m_nWait=0;

	if(!bPause)
	{
		m_ptPos.y = m_ptPos.y + m_nMotion * m_V;
	}

	if(m_ptPos.y > GAME_HEIGHT+JEWEL_HEIGHT )
		return FALSE;
	if(m_ptPos.y < -JEWEL_HEIGHT)
		return FALSE;

	m_Images1.Draw(pDC,m_nImgIndex,m_ptPos,ILD_TRANSPARENT);

	return TRUE;
	return 0;
}

BOOL CJewel::LoadImage()
{
	return CGameObject::LoadImage(m_Images1,IDB_BITMAP18,RGB(255,255,255),30,30,1);
	return 0;
}
