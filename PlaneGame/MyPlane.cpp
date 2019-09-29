#include "StdAfx.h"
#include "MyPlane.h"
#include "resource.h"
#include "stdafx.h"
#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")
#include <MMSYSTEM.H>
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣



CImageList CMyPlane::m_Images;

CMyPlane::~CMyPlane(void)
{
}
CMyPlane::CMyPlane(void):m_nHorMotion(0),m_nVerMotion(0)
, HP(100)
, life(5)
, score(0)
, level(1)//���ó�ʼֵ
{
	m_nWait = 0;
	m_ptPos.x = 400;
	m_ptPos.y = 500;
}

BOOL CMyPlane::Fired()
{
	if(m_nWait==0)
		return TRUE;
	else
		return FALSE;

}

BOOL CMyPlane::LoadImage()
{     
	return CGameObject::LoadImage(m_Images, IDB_BITMAP2, RGB(255, 255, 255), 50, 57, 1);
}
BOOL CMyPlane::Draw(CDC* pDC,BOOL bPause)
{
	m_nWait++;
	if(m_nWait>3)
		m_nWait=0;


	if(!bPause)
	{
		m_ptPos.x = m_ptPos.x + m_nHorMotion*6;
		m_ptPos.y = m_ptPos.y - m_nVerMotion*6;
	}

	if(m_ptPos.x>=GAME_WIDTH-PLANE_WIDTH)
	m_ptPos.x =GAME_WIDTH-PLANE_WIDTH;
	if(m_ptPos.x<=0)
		m_ptPos.x=0;

	if(m_ptPos.y<=0)
		m_ptPos.y =0;
	if(m_ptPos.y>=GAME_HEIGHT-PLANE_HEIGHT-20)
		m_ptPos.y=GAME_HEIGHT-PLANE_HEIGHT-20;


	//m_Images.Draw(pDC,0,m_ptPos,ILD_TRANSPARENT);

	if (HP >= 50)
		m_Images.Draw(pDC, 0, m_ptPos, ILD_TRANSPARENT);
	else if(HP>0 && HP<50)
	{
		
		CDC memDC;//����һ������DC�Ķ���
		memDC.CreateCompatibleDC(pDC);//��memDC��pDC����
		CBitmap bmpDraw;//����һ��λͼ����
		bmpDraw.LoadBitmap(IDB_BITMAP16);//װ��DDB
		CBitmap* pbmpOld = memDC.SelectObject(&bmpDraw);//����ԭ�е�DDB����ѡ����DDB��DC
		//pDC->BitBlt(x, y, 50, 58, &memDC, 0, 0, SRCCOPY);//��ԴDC�и��Ƶ�Ŀ��DC
		pDC->TransparentBlt(m_ptPos.x, m_ptPos.y, 71, 69, &memDC, 0, 0, 71, 69, RGB(255, 255, 255));
		memDC.SelectObject(pbmpOld);//ѡ��ԭDDB
		//PlaySound(TEXT("res\\music\\update.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	
	
	return TRUE;
}