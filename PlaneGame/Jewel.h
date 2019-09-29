#pragma once
#include "gameobject.h"

class CJewel :
	public CGameObject
{
public:
	CJewel(void);
	~CJewel(void);
	BOOL Draw(CDC* pDC, BOOL bPause);

	static BOOL LoadImage();
	CRect GetRect()
	{
		return CRect(m_ptPos,CPoint(m_ptPos.x+JEWEL_HEIGHT,m_ptPos.y+JEWEL_HEIGHT));
	}


	int GetMontion() const
	{
		return m_nMotion;//方向，1->向下 0->停止 -1->向上
	}

private:
	static const int JEWEL_HEIGHT = 18;
	static CImageList m_Images1;
	int m_nImgIndex;
	int m_V;
	int m_nMotion;
	int m_nWait;
};
