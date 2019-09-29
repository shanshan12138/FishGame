#pragma once
#include "gameobject.h"

class CBall2 :
	public CGameObject
{
public:
	CBall2(int x,int y,int nMontion);
	~CBall2(void);
	BOOL Draw(CDC* pDC, BOOL bPause);
	static BOOL LoadImage(void);

	CRect GetRect()
	{
		return CRect(m_ptPos,CPoint(m_ptPos.x+9,m_ptPos.y+BALL2_HEIGHT));
	}

private:
	static const int BALL2_HEIGHT = 10;
	static CImageList m_Images3;
	int    m_nMotion;
};
