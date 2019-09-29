#include "StdAfx.h"
#include "Explosion.h"
#include "resource.h"

CImageList CExplosion::m_Images;//Images�Ǿ�̬���� ��Ҫ�������ʼ��

CExplosion::CExplosion(int x,int y):CGameObject(x,y),m_nProcess(0)//����ͱ����е����Գ�ʼ��
{
}

CExplosion::~CExplosion(void)
{
}
BOOL CExplosion::Draw(CDC* pDC,BOOL bPause)//����ͼƬ ����������bool���� Ŀ����Ϊ�˽����Ѻ�
{
	
	if(m_nProcess==15)
	{
		m_nProcess=0;
		return FALSE;
	}

	//����λ�û���ͼ��
	m_Images.Draw(pDC,m_nProcess,m_ptPos,ILD_TRANSPARENT);
	m_nProcess++;
	return TRUE;
}

BOOL CExplosion::LoadImage()
{
	return CGameObject::LoadImage(m_Images,IDB_EXPLOSION,RGB(0,0,0),66,66,8);//ͼƬ��Сλ66 �а˸�ͼƬ���
}