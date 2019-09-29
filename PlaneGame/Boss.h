#pragma once
#include "gameobject.h"

class CBoss :
	public CGameObject
{
public:
	CBoss(void);
	~CBoss(void);

	BOOL Draw(CDC* pDC, BOOL bPause);

	static BOOL LoadImage();

	CRect GetRect()
	{
		return CRect(m_ptPos,CPoint(m_ptPos.x+64,m_ptPos.y+BOSS_HEIGHT));
	}

	int GetMontion() const
	{
		return m_nMotion;//方向，1->向下 0->停止 -1->向上
	}


	BOOL Fired(void);
	int life;
private:
	static const int BOSS_HEIGHT = 45;
	static CImageList m_Images2;
	int m_nMotion;
	int m_V;
	int m_nWait;
	int m_nImagIndex;

};
