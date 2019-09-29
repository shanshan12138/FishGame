#pragma once
#include "gameobject.h"

class CEnemy :public CGameObject
{
public:
	CEnemy(void);
	~CEnemy(void);

	BOOL Draw(CDC* pDC,BOOL bPause);

	static BOOL LoadImage();

	CRect GetRect()
	{
		return CRect(m_ptPos,CPoint(m_ptPos.x+51,m_ptPos.y+37));
	}

	int GetMontion() const
	{
		return m_nMotion;//����1->���� 0->ֹͣ -1->����
	}
	//�Ƿ���Կ������ӵ�
	BOOL Fired();
private:
	static const int ENEMY_HEIGHT = 37;
	static const int ENEMY_WIDTH = 51;
	static CImageList m_Images;
	int    m_nMotion;//���� 1->���� 0->ֹͣ -1->����
	//ͼ������
	int m_nImgIndex;
	//�ٶ�
	int m_V;
	int m_nWait;//������ʱ
};
