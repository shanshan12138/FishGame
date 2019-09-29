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
	
	//���ȷ��Xλ��
	m_ptPos.x = rand()%(GAME_WIDTH-BOSS_HEIGHT)+1;

	//���ȷ��ͼ������
	m_nImagIndex = rand()%2;

	//����ͼ������ȷ������
	m_nMotion=1;
	m_ptPos.y=-BOSS_HEIGHT;
	if (m_nImagIndex%2!=0)//�����ͼ��������ż��
	{
		//m_nMotion=-1;�л����·�����
		m_ptPos.y = GAME_HEIGHT+BOSS_HEIGHT;
	}
	//���ȷ���ٶ�
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
