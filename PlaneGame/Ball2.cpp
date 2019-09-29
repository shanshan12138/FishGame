#include "StdAfx.h"
#include "Ball2.h"
#include "resource.h"

CImageList CBall2::m_Images3;

CBall2::CBall2(int x,int y,int nMontion):CGameObject(x,y),m_nMotion(nMontion)
{
}

CBall2::~CBall2(void)
{
}

BOOL CBall2::Draw(CDC* pDC, BOOL bPause)
{
	if(!bPause)
	{
		m_ptPos.y = m_ptPos.y + m_nMotion * 8;
	}

	if(m_ptPos.y > GAME_HEIGHT+BALL2_HEIGHT )
		return FALSE;
	if(m_ptPos.y < -BALL2_HEIGHT)
		return FALSE;

	m_Images3.Draw(pDC,0,m_ptPos,ILD_TRANSPARENT);

	return TRUE;
	return 0;
}

BOOL CBall2::LoadImage(void)
{
	return CGameObject::LoadImage(m_Images3,IDB_BITMAP10,RGB(0,0,0),9,10,1);
	return 0;
}
