#include "StdAfx.h"
#include "Explosion.h"
#include "resource.h"

CImageList CExplosion::m_Images;//Images是静态函数 需要在类外初始化

CExplosion::CExplosion(int x,int y):CGameObject(x,y),m_nProcess(0)//父类和本类中的属性初始化
{
}

CExplosion::~CExplosion(void)
{
}
BOOL CExplosion::Draw(CDC* pDC,BOOL bPause)//绘制图片 返回类型是bool类型 目的是为了界面友好
{
	
	if(m_nProcess==15)
	{
		m_nProcess=0;
		return FALSE;
	}

	//用新位置绘制图像
	m_Images.Draw(pDC,m_nProcess,m_ptPos,ILD_TRANSPARENT);
	m_nProcess++;
	return TRUE;
}

BOOL CExplosion::LoadImage()
{
	return CGameObject::LoadImage(m_Images,IDB_EXPLOSION,RGB(0,0,0),66,66,8);//图片大小位66 有八个图片组成
}