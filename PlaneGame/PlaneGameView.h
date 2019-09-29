  // PlaneGameView.h : CPlaneGameView 类的接口
//


#include "MyPlane.h"
#include "Enemy.h"
#include "Bomb.h"
#include "Ball.h"
#include "Explosion.h"
#include <atlimage.h>
#include"BackGround.h"
#include"Jewel.h"
#include"Boss.h"
#include"Ball2.h"

class CMyPlane;
class CPlaneGameView : public CView
{

	enum ObjType{enBall2,enEnemy,enBomb,enBall,enExplosion,enJewel,enBoss};

protected: // 仅从序列化创建
	CPlaneGameView();
	DECLARE_DYNCREATE(CPlaneGameView) 


// 属性
public:
	CPlaneGameDoc* GetDocument() const;
	CBrush m_brush;
// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CPlaneGameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//内存DC
	CDC*        m_pMemDC;
	//设备DC
	CClientDC*  m_pDC;
	//内存位图
	CBitmap*    m_pMemBitmap;


protected:
	//初始化游戏
	BOOL InitGame();
	//结束游戏
	void StopGame();

	//刷新游戏的帧画面
	void UpdateFrame(CDC* pMemDC);


	void AI();

	//获得键的状态1->down
	int GetKey(int nVirtKey)
	{
		return (GetKeyState(nVirtKey) & 0x8000)? 1 : 0;
	}
	
	CMyPlane*  m_pMe;

	CObList    m_ObjList[10];



// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	CMyPlane plane;
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	BackGround Bg;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point );


	int level;
	int NN;
	void OnMouseMove();
	int myplane;
};

#ifndef _DEBUG  // PlaneGameView.cpp 中的调试版本
inline CPlaneGameDoc* CPlaneGameView::GetDocument() const
   { return reinterpret_cast<CPlaneGameDoc*>(m_pDocument); }
#endif

