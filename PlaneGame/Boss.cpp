#include "StdAfx.h"
#include "Boss.h"
#include"resource.h"

CImageList CBoss::m_Images2;
CBoss::CBoss(void)
: m_nMotion(0)
, m_V(0)
, m_nWait(10)
, m_nImagIndex(0)
, life(10)
{
	
	//随机确定X位置
	m_ptPos.x = rand()%(GAME_WIDTH-BOSS_HEIGHT)+1;

	//随机确定图像索引
	m_nImagIndex = rand()%2;

	//根据图像索引确定方向
	m_nMotion=1;
	m_ptPos.y=-BOSS_HEIGHT;
	if (m_nImagIndex%2!=0)//如果是图像索引是偶数
	{
		//m_nMotion=-1;敌机从下方进入
		m_ptPos.y = GAME_HEIGHT+BOSS_HEIGHT;
	}
	//随机确定速度
	m_V = rand()%6+2;

	m_nWait=10;
}

CBoss::~CBoss(void)
{
}

BOOL CBoss::Draw(CDC* pDC, BOOL bPause)
{
	m_nWait++;
	if(m_nWait>20)
		m_nWait=10;

	if(!bPause)
		m_ptPos.y = m_ptPos.y + m_nMotion * m_V;

	if(m_ptPos.y > GAME_HEIGHT+BOSS_HEIGHT )
		return FALSE;
	if(m_ptPos.y < -BOSS_HEIGHT)
		return FALSE;

	m_Images2.Draw(pDC,m_nImagIndex,m_ptPos,ILD_TRANSPARENT);

	return TRUE;
	return 0;
}

BOOL CBoss::LoadImage()
{
	return CGameObject::LoadImage(m_Images2,IDB_BITMAP4,RGB(255,255,255),75,86,2);
	return 0;
}

BOOL CBoss::Fired(void)
{
	if(m_nWait==10)
		return TRUE;
	else
		return FALSE;
	return 0;
}
