// PlaneGameView.cpp : CPlaneGameView 类的
//
#include "stdafx.h"
#include "PlaneGame.h"
#include "PlaneGameDoc.h"
#include "PlaneGameView.h"
#pragma once
#include "background.h"
#include <MMSYSTEM.H>//windows中与多媒体有关的大多数接口
#include <iostream>
#pragma comment(lib, "WINMM.LIB")//导入winmm.lib函数库


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static int pass=1;//与关卡相同的一个变量
static int big;//大招
static int big2;
static int lifenum=0;//生命的数量
int score = 0;

// CPlaneGameView

IMPLEMENT_DYNCREATE(CPlaneGameView, CView)

BEGIN_MESSAGE_MAP(CPlaneGameView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_TIMER()

	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


//多线程背景音乐拷贝调用子函数开始

bool ExtractResource(LPCTSTR strDstFile, LPCTSTR strResType, LPCTSTR strResName) {//中间调用函数
																				  // 创建文件
	HANDLE hFile = ::CreateFile(strDstFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	// 查找资源文件中、加载资源到内存、得到资源大小
	HRSRC    hRes = ::FindResource(NULL, strResName, strResType);
	HGLOBAL    hMem = ::LoadResource(NULL, hRes);
	DWORD    dwSize = ::SizeofResource(NULL, hRes);

	// 写入文件
	DWORD dwWrite = 0;      // 返回写入字节
	::WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
	::CloseHandle(hFile);

	return true;

}
//多线程背景音乐拷贝调用子函数结束

volatile bool g_bEndMusicThread;

unsigned __stdcall ThreadPlayMusic(LPVOID lpParameter)//LPVOID,是一个没有类型的指针
                                                       //lpParameter:向线程函数传递的参数, 是一个指向结构的指针, 不需传递参数时, 为NULL
{
	// 原理：调用已经封装进资源的MP3，复制到内存成为临时变量文件，调用此文件，播放声音。
	// 产生临时文件的文件名
	TCHAR tmpmp3[_MAX_PATH];
	::GetTempPath(_MAX_PATH, tmpmp3);
	_tcscat_s(tmpmp3, _T("testapp_background.mp3"));
	// 将wav 资源提取为临时文件
	// 注：如果wave资源的 ID 为宏 IDR_BGM，这样做：
	ExtractResource(tmpmp3, _T("MP3"), MAKEINTRESOURCE(IDR_MP33));
	TCHAR mcicmd[300];
	_stprintf_s(mcicmd, _T("open \"%s\" alias gamebg"), tmpmp3);
	//打开背景音乐
	mciSendString(mcicmd, NULL, 0, NULL);
	//播放背景音乐(重复播放)
	mciSendString(_T("Play gamebg repeat"), NULL, 0, NULL);
	DeleteFile(tmpmp3);
	return 0;
}
//背景音乐多线程结束


/*************************************************************************/

// CPlaneGameView 构造/析构
CPlaneGameView::CPlaneGameView():m_pMe(NULL)
, level(0)
, NN(0)
, myplane(0)
{
	// TODO: 在此处添加构造代码
	CBitmap bmp;
        bmp.LoadBitmap(IDB_BITMAP15); ///加载位图
        m_brush.CreatePatternBrush(&bmp); ///创建位图画刷
}

CPlaneGameView::~CPlaneGameView()
{
}

BOOL CPlaneGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CPlaneGameView 绘制

void CPlaneGameView::OnDraw(CDC* /*pDC*/)
{
	CPlaneGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;


	// TODO: 在此处为本机数据添加绘制代码
}


// CPlaneGameView 打印

BOOL CPlaneGameView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPlaneGameView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CPlaneGameView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CPlaneGameView 诊断

#ifdef _DEBUG
void CPlaneGameView::AssertValid() const
{
	CView::AssertValid();
}

void CPlaneGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPlaneGameDoc* CPlaneGameView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlaneGameDoc)));
	return (CPlaneGameDoc*)m_pDocument;
}
#endif //_DEBUG


/*****************************************************************/


// CPlaneGameView 消息处理程序
void CPlaneGameView::OnInitialUpdate()//框架在第一次调用OnDraw前会调用OnInitialUpdate，因此OnInitialUpdate是设置滚动视图的逻辑尺寸和映射模式的最合适的地方
{
	CView::OnInitialUpdate();
	// TODO: 在此添加专用代码和/或调用基类
	//初始化游戏
	InitGame();
}
void CPlaneGameView::StopGame()
{
	delete m_pMe;
	delete m_pMemDC;
	delete m_pDC;
	delete m_pMemBitmap;
}

CImageList startIMG;
BOOL CPlaneGameView::InitGame()
{
	CRect rc;
	GetClientRect(rc);

	//产生随机数种子
	srand( (unsigned)time( NULL ) );

	//建立设备DC
	m_pDC = new CClientDC(this);

	//建立内存DC
	m_pMemDC = new CDC;
	m_pMemDC->CreateCompatibleDC(m_pDC);

	//建立内存位图
	m_pMemBitmap = new CBitmap;
	m_pMemBitmap->CreateCompatibleBitmap(m_pDC,GAME_WIDTH,GAME_HEIGHT);

	//将位图选入内存DC
	m_pMemDC->SelectObject(m_pMemBitmap);

	CMyPlane::LoadImage();//加载战机

	CEnemy::LoadImage();//加载敌机

	CBomb::LoadImage();//加载导弹

	CBall::LoadImage();//加载子弹

	CExplosion::LoadImage();//加载爆炸效果

	CJewel::LoadImage();//加载宝石

	CBoss::LoadImage();//加载BOSS

	CBall2::LoadImage();//加载BOSS的子弹
						
						//播放背景音乐
	g_bEndMusicThread = false;	//初始化子线程标志位
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadPlayMusic, NULL, 0, NULL);	//创建线程



	//产生主角(战机)
	m_pMe = new CMyPlane;

	//开始界面
	CBitmap startbmp;
	startbmp.LoadBitmapW(IDB_BITMAP22);
	startIMG.Create(900, 600, ILC_COLOR24 | ILC_MASK, 1, 0);
	startIMG.Add(&startbmp, RGB(0, 0, 0));

	//启动游戏  
	SetTimer(1,30,NULL);

	return TRUE;
}


/********************************/

int HP = 0;
void CPlaneGameView::UpdateFrame(CDC* pMemDC)
{
	//绘制天空
	
	CRect Rect;
	GetClientRect(&Rect);

		NN++;
		CRect rect;
		GetClientRect(&rect);
		CDC memdc;
		memdc.CreateCompatibleDC(pMemDC);
		CBitmap bitmap;
		BITMAP bitmap1;
		//if(m_pMe!=NULL)
		if (score >= 0 && score <600)
		bitmap.LoadBitmapW(IDB_BITMAP21);
		else if (score >= 600 && score <1200)
			bitmap.LoadBitmapW(IDB_BITMAP19);
		else if (score >= 1200 && score <1800)
			bitmap.LoadBitmapW(IDB_back3);
		else if((score >= 1800))
			bitmap.LoadBitmapW(IDB_back4);
		bitmap.GetBitmap(&bitmap1);
		memdc.SelectObject(&bitmap);
		pMemDC->StretchBlt(0, 0 + NN, GAME_WIDTH, GAME_HEIGHT, &memdc, 0, 0, bitmap1.bmWidth, bitmap1.bmHeight, SRCCOPY);
		pMemDC->StretchBlt(0, -GAME_HEIGHT + NN, GAME_WIDTH, GAME_HEIGHT, &memdc, 0, 0, bitmap1.bmWidth, bitmap1.bmHeight, SRCCOPY);
		if (NN >= GAME_HEIGHT)
			NN = 0;
		/*if (m_pMe != NULL)
		{
			CPen p1(PS_SOLID, 2, RGB(255, 0, 0));
			pMemDC->SelectObject(&p1);
			CBrush b1(RGB(0, 255, 0));
			pMemDC->SelectObject(&b1);
			pMemDC->Rectangle(60, 25, 50 + HP, 35);
		}*/
	//}
	/*else if (score >=500)
	{
		NN++;
		CRect rect;
		GetClientRect(&rect);
		CDC memdc;
		memdc.CreateCompatibleDC(pMemDC);
		CBitmap bitmap;
		BITMAP bitmap1;
		bitmap.LoadBitmapW(IDB_BITMAP19);
		bitmap.GetBitmap(&bitmap1);
		memdc.SelectObject(&bitmap);
		pMemDC->StretchBlt(0, 0 + NN, GAME_WIDTH, GAME_HEIGHT, &memdc, 0, 0, bitmap1.bmWidth, bitmap1.bmHeight, SRCCOPY);
		pMemDC->StretchBlt(0, -GAME_HEIGHT + NN, GAME_WIDTH, GAME_HEIGHT, &memdc, 0, 0, bitmap1.bmWidth, bitmap1.bmHeight, SRCCOPY);
		if (NN >= GAME_HEIGHT)
			NN = 0;
		level = 2;
	}*/
	//Bg.draw(pMemDC);//屏幕滚动

	wchar_t lifeleft[20];//扩展的字符存储方式
	wchar_t Score[20];
	wchar_t Level[20];
	wchar_t dazhao[20];
	wchar_t qingping[20];
	wchar_t fuhuo[20];
	wchar_t stop[20];
	wchar_t wudi[20];
	wchar_t over[20];
	wchar_t tishi[40];
	if(m_pMe!=NULL)
	{
		pMemDC->SetBkMode(TRANSPARENT);//填充背景
		wsprintf(lifeleft, L"生命值:%d   剩余命数:%d",m_pMe->HP,m_pMe->life );
		pMemDC->SetTextColor( RGB(255, 255, 255) );
		pMemDC->TextOutW(10,10, lifeleft, _tcslen(lifeleft));

		wsprintf(Score, L"得分:%d", score);
		pMemDC->SetTextColor( RGB(255, 192, 203) );
		pMemDC->TextOutW(10,35, Score, _tcslen(Score));

		wsprintf(Level, L"关卡: %d",level );
		pMemDC->SetTextColor( RGB(255, 255, 0) );
		pMemDC->TextOutW(430,10, Level, _tcslen(Level));	

		wsprintf(dazhao, L"shift：放大招！");
		pMemDC->SetTextColor( RGB(255, 227, 132) );
		pMemDC->TextOutW(10,400, dazhao, _tcslen(dazhao));

		wsprintf(qingping, L"Z：秒杀全屏普通小鱼！");
		pMemDC->SetTextColor( RGB(255, 128, 128) );
		pMemDC->TextOutW(10,425, qingping , _tcslen(qingping));

		wsprintf(fuhuo, L"Q：起死回生！");
		pMemDC->SetTextColor( RGB(255, 128, 128) );
		pMemDC->TextOutW(10,450, fuhuo , _tcslen(fuhuo));

		wsprintf(stop, L"P：游戏暂停！");
		pMemDC->SetTextColor( RGB(255, 128, 128) );
		pMemDC->TextOutW(10,475, stop , _tcslen(stop));

		wsprintf(wudi, L"F：开启无敌模式！");
		pMemDC->SetTextColor( RGB(255, 128, 128) );
		pMemDC->TextOutW(10,500, wudi , _tcslen(wudi));

		wsprintf(wudi, L"G：关闭无敌模式！");
		pMemDC->SetTextColor( RGB(255, 128, 128) );
		pMemDC->TextOutW(10,525, wudi , _tcslen(wudi));

		wsprintf(tishi, L"提示：多吃红心才能玩儿的更久噢 ^_^！");
		pMemDC->SetTextColor(RGB(255, 0, 255));
		pMemDC->TextOutW(10, 550, tishi,_tcslen(tishi));

	//	 if(lifenum==1)
	//	{
			wsprintf(wudi, L"无敌模式");
			pMemDC->SetTextColor( RGB(255, 255, 255) );
			pMemDC->TextOutW(350,200, wudi , _tcslen(wudi));
	//	}
	
	}
	

	//绘制我方战机

	if(m_pMe!=NULL)
	{
	   m_pMe->Draw(m_pMemDC,FALSE);
	}
	else
	{ 
		//Game Over
		

		wsprintf(over, L"G A M E  O V E R !!!");
		pMemDC->SetTextColor(RGB(255, 0, 0));
		pMemDC->TextOutW(300, (GAME_HEIGHT / 2)-50, over, _tcslen(over));
		//PlaySound(TEXT("res\\music\\blood.wav"), NULL, SND_FILENAME | SND_ASYNC);
		

		wsprintf(fuhuo, L"失败了， 按 Q 再来一次吧 ^_^");
		pMemDC->SetTextColor( RGB(255, 20, 147) );
		pMemDC->TextOutW(300,GAME_HEIGHT/2, fuhuo , _tcslen(fuhuo));
		
	}

/*********************************************/	
	
	//绘制 导弹、爆炸、敌机、子弹(七个类，七个链表)

	for(int i=0;i<7;i++)
	{
		POSITION pos1,pos2;//指针 
		for( pos1 = m_ObjList[i].GetHeadPosition(); ( pos2 = pos1 ) != NULL; )//pos1 和pos2指向同一个链表的内容
		{
			CGameObject* pObj = (CGameObject*)m_ObjList[i].GetNext( pos1 );//获得pos1的地址
			if(!pObj->Draw(pMemDC,FALSE))//内存设备描述表（或内存设备内容或显示装置内容）
				                         //如果pObj为0 
			{                            //删除pObj指针
				m_ObjList[i].RemoveAt(pos2);
			//	if(pObj！= NULL)
				delete pObj;
			}
		}
	}

	//复制内存DC到设备DC
	//m_pDC->BitBlt(0,0,GAME_WIDTH,GAME_HEIGHT,m_pMemDC,0,0,SRCCOPY);
	m_pDC->SetStretchBltMode(HALFTONE);
	m_pDC->StretchBlt(0, 0, Rect.Width(), Rect.Height(), pMemDC, 0, 0, GAME_WIDTH, GAME_HEIGHT, SRCCOPY);
	

}  
/****************************************************************/

void CPlaneGameView::AI()
{
	static int nCreator = rand() %2+10;

	//随机产生敌机
	if(nCreator<=0)
	{
		nCreator = rand()%2+10-pass;//随着关数增加，敌机数也增加         rand()%2=产生的数字在0~4之间

		m_ObjList[enEnemy].AddTail(new CEnemy);
	}
	nCreator--;
		

	static int mCreator = rand()%2+10;

	//随机产生血包
	if(mCreator<=0)
	{
		mCreator = rand()%5+20;
		m_ObjList[enJewel].AddTail(new CJewel);
	}
	mCreator--;


	static int tCreator = rand()%2+10;
	
	//随机产生Boss
	if(tCreator<=0)
	{
		tCreator = rand()%5+100;
		m_ObjList[enBoss].AddTail(new CBoss);
	}
	tCreator--;



	if(m_pMe==NULL)
		return;

	//检测四个方向键，移动战机
	for(int i=0;i<4;i++)//2表示战机速度
	{
		int nMeMotion=0;
		m_pMe->SetVerMotion(0);
		m_pMe->SetHorMotion(0);

		nMeMotion = GetKey(VK_UP);
		if(nMeMotion==1)
			m_pMe->SetVerMotion(2);//垂直
		    
		nMeMotion = GetKey(VK_DOWN);
		if(nMeMotion==1)
			m_pMe->SetVerMotion(-2);

		nMeMotion = GetKey(VK_RIGHT);
		if(nMeMotion==1)
			m_pMe->SetHorMotion(2);//水平

		nMeMotion = GetKey(VK_LEFT);
		if(nMeMotion==1)
			m_pMe->SetHorMotion(-2);//水平
	}

	//游戏的暂停、继续功能

	if(GetKey('P'))
	{
		KillTimer(1);
		if (AfxMessageBox(_T("继续游戏"), MB_OK) == 1)
		{
			SetTimer(1, 30, NULL);
		}
		else(1);
	}
	
	//产生战机导弹

	if(GetKey(VK_SPACE)==1)//按下了空格键发射子弹
	{
		if(m_pMe!=NULL && m_pMe->Fired())
		{
			CPoint pt = m_pMe->GetPoint();
			//PlaySound(TEXT("res\\music\\attack.wav"),NULL, SND_FILENAME | SND_ASYNC);    
			PlaySound((LPCTSTR)IDR_WAVE2, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
			m_ObjList[enBomb].AddTail(new CBomb(pt.x+7,pt.y+10));
			m_ObjList[enBomb].AddTail(new CBomb(pt.x+17,pt.y+10));
			m_ObjList[enBomb].AddTail(new CBomb(pt.x+27,pt.y+10));
			
		}
	}
	if(GetKey(VK_SHIFT)==1)//按下了shift键放大招1
	{
		if(m_pMe!=NULL && m_pMe->Fired())
		{
			CPoint pt = m_pMe->GetPoint();
			for (big = -GAME_WIDTH; big <= GAME_WIDTH; big +=20)
				m_ObjList[enBomb].AddTail(new CBomb(pt.x + big, GAME_HEIGHT));
		}
	}/****************************************************************************/
	/*if (GetKey('X'))//按下了X键放大招2
	{
		if (m_pMe != NULL && m_pMe->Fired())
		{
			pos.y -= speed;
			pos.x = (int)950 * sin(pos.y*0.01);
			if (pos.x < -20)
			{
				pos.x = -pos.x;
			}

		}
		*/

	//敌机发射子弹

	CPoint PlanePt = m_pMe->GetPoint();//获取战机位置
	for(POSITION ePos=m_ObjList[enEnemy].GetHeadPosition();ePos!=NULL;)
		             //消除敌机
	{
          CEnemy* pEnemy = (CEnemy*)m_ObjList[enEnemy].GetNext(ePos);
		  if(!pEnemy->Fired())
			  continue;
		  CPoint  ePt = pEnemy->GetPoint();

		  BOOL by=FALSE;

		  ///敌机在战机前面
		  if(pEnemy->GetMontion()==1 && ePt.y<PlanePt.y)
		         by=  TRUE;
		  //敌机在战机后面
		  //if(pEnemy->GetMontion()==-1 && ePt.y>PlanePt.y)
			//	 by=  TRUE;	

		  if(by && ePt.x +100 >= PlanePt.x && ePt.x<PlanePt.x+CMyPlane::PLANE_WIDTH+100)
		  {
              m_ObjList[enBall].AddTail(new CBall(ePt.x+12,ePt.y+25,pEnemy->GetMontion()));
		  }
	}

	//Boss发射子弹

	CPoint PlanePt1 = m_pMe->GetPoint();
	for(POSITION ePos=m_ObjList[enBoss].GetHeadPosition();ePos!=NULL;)
	{
          CBoss* pBoss = (CBoss*)m_ObjList[enBoss].GetNext(ePos);
		  if(!pBoss->Fired())
			  continue;
		  CPoint  ePt1 = pBoss->GetPoint();
		  BOOL by=FALSE;
		  //Boss在战机前面
		  if(pBoss->GetMontion()==1 && ePt1.y<PlanePt1.y)
		         by=  TRUE;
		  //Boss在战机后面
		  if(pBoss->GetMontion()==-1 && ePt1.y>PlanePt1.y)
				 by=  TRUE;	

		  if(by && ePt1.x +200 >= PlanePt1.x && ePt1.x<PlanePt1.x+CMyPlane::PLANE_WIDTH+100)
		  {
              m_ObjList[enBall2].AddTail(new CBall2(ePt1.x+28,ePt1.y+15,pBoss->GetMontion()));
			  //如果发生碰撞，在爆炸效果完成之后从CObList中删除该子弹
		  }
	}


	//Boss子弹炸掉战机

	POSITION kkPos1=NULL,kkPos2=NULL;
	CRect mRect = m_pMe->GetRect();
	for(kkPos1=m_ObjList[enBall2].GetHeadPosition();( kkPos2 = kkPos1 ) != NULL;)
	{
		CBall2* pBall2 = (CBall2*)m_ObjList[enBall2].GetNext(kkPos1);
		CRect bkRect = pBall2->GetRect();
		CRect tmpRect;
		if(tmpRect.IntersectRect(&bkRect,mRect))//IntersectRect返回值类型是bool类型，用于碰撞检测
		{
			//添加爆炸效果
			m_ObjList[enExplosion].AddTail(new CExplosion(mRect.left,mRect.top));

			//删除子弹
			m_ObjList[enBall2].RemoveAt(kkPos2);
			if(pBall2!=NULL)
			delete pBall2;
			if(lifenum ==0)
				m_pMe->HP -= 60;//被子弹打中一次HP-60

			if(lifenum ==0&&m_pMe->HP<=0&&m_pMe->life>=1)
			{
				--m_pMe->life;//减少生命数
				m_pMe->HP=100;
			}
			
			//删除战机
			else if((lifenum ==0&&m_pMe->life<0)||(lifenum ==0&&(m_pMe->HP<=0&&m_pMe->life==0)))
				   //当生命值和生命数均为0时结束游戏,
			{
				if(m_pMe!=NULL)
				delete m_pMe;
				m_pMe=NULL;
				break;
			}
		}
	}

	//敌机子弹炸掉战机

	POSITION  bPos1=NULL,bPos2=NULL;
	for(bPos1=m_ObjList[enBall].GetHeadPosition();( bPos2 = bPos1 ) != NULL;)
	{
		CBall* pBall = (CBall*)m_ObjList[enBall].GetNext(bPos1);
		CRect bRect = pBall->GetRect();
		CRect tmpRect;
		if(tmpRect.IntersectRect(&bRect,mRect))
		{
			//添加爆炸效果
			m_ObjList[enExplosion].AddTail(
				new CExplosion(mRect.left,mRect.top)
				);

			//删除子弹
			m_ObjList[enBall].RemoveAt(bPos2);
			if(pBall!=NULL)
			delete pBall;
			if(lifenum ==0)
				if(m_pMe!=NULL)
				m_pMe->HP-=20;//被子弹打中一次HP-20

			if(lifenum ==0&&m_pMe->HP<=0&&m_pMe->life>=1)
			{
				--m_pMe->life;//减少生命数
				m_pMe->HP=100;
			}
			
			//删除战机
			else if((lifenum ==0&&m_pMe->life<0)||(lifenum ==0&&(m_pMe->HP<=0&&m_pMe->life==0)))
				//当生命值和生命数均为0时结束游戏
			{
				if (m_pMe!= NULL)
				delete m_pMe;
				m_pMe=NULL;
				break;
			}
		}
	}

	//战机导弹炸掉敌机

	POSITION mPos1=NULL,mPos2=NULL;
	for(mPos1=m_ObjList[enBomb].GetHeadPosition();(mPos2=mPos1)!=NULL;)
	{
		CBomb* pBomb = (CBomb*)m_ObjList[enBomb].GetNext(mPos1);
		CRect bRect = pBomb->GetRect();

		POSITION ePos1=NULL,ePos2=NULL;
		for(ePos1=m_ObjList[enEnemy].GetHeadPosition();(ePos2=ePos1)!=NULL;)
		{
			CEnemy* pEnemy = (CEnemy*)m_ObjList[enEnemy].GetNext(ePos1);
			CRect mRect = pEnemy->GetRect();
			CRect tmpRect;
			if(tmpRect.IntersectRect(&bRect,mRect))
			{
				//添加爆炸效果
				m_ObjList[enExplosion].AddTail(
					new CExplosion(mRect.left,mRect.top)
					);
				//删除导弹
				m_ObjList[enBomb].RemoveAt(mPos2);
				if(pBomb!=NULL)
				delete pBomb;
				if(m_pMe!=NULL)
				score +=20;//消灭一架敌机分数+20
				if(score   >  level*500)
				{
					++ level;
					++pass;
				}

				//删除敌机
				m_ObjList[enEnemy].RemoveAt(ePos2);
				if (pEnemy != NULL)
				delete pEnemy;
				break;
			}
		}
	}

	//战机导弹炸掉Boss

	POSITION hhPos1=NULL,hhPos2=NULL;
	for(hhPos1=m_ObjList[enBomb].GetHeadPosition();(hhPos2=hhPos1)!=NULL;)
	{
		CBomb* pBomb = (CBomb*)m_ObjList[enBomb].GetNext(hhPos1);
		CRect bhRect = pBomb->GetRect();

		POSITION htPos1=NULL,htPos2=NULL;
		for(htPos1=m_ObjList[enBoss].GetHeadPosition();(htPos2=htPos1)!=NULL;)
		{
			CBoss* pBoss = (CBoss*)m_ObjList[enBoss].GetNext(htPos1);
			CRect mRect = pBoss->GetRect();
			CRect tmpRect;
			if(tmpRect.IntersectRect(&bhRect,mRect))
			{   
				pBoss->life--;
				//添加爆炸效果
				m_ObjList[enExplosion].AddTail(new CExplosion(mRect.left,mRect.top));
				//删除导弹
				if(pBomb!=NULL)
				{
					m_ObjList[enBomb].RemoveAt(hhPos2);
				}

				//删除boss
				if(pBoss->life<=0)
				{
					m_ObjList[enBoss].RemoveAt(htPos2);
					if (pBoss != NULL)
					delete pBoss;
					score += 100;//消灭一个Boss，score+100
					break;
				}
			}
		}
	}

	//战机Boss相撞

	POSITION rrPos1=NULL,rrPos2=NULL;
	for(rrPos1=m_ObjList[enBoss].GetHeadPosition();( rrPos2 = rrPos1 ) != NULL;)
	{
		CBoss* pBoss = (CBoss*)m_ObjList[enBoss].GetNext(rrPos1);
		CRect brRect = pBoss->GetRect();
		CRect tmpRect;
		if(tmpRect.IntersectRect(&brRect,mRect))
		{
			//添加爆炸效果
			m_ObjList[enExplosion].AddTail(new CExplosion(mRect.left,mRect.top));

			//删除敌机
			m_ObjList[enBoss].RemoveAt(rrPos2);
			delete pBoss;
			if (m_pMe!=NULL)
			{if (lifenum == 0)
				m_pMe->life -= 3;//被Boss撞一次战机life-3

			if (lifenum == 0 && m_pMe->HP <= 0 && m_pMe->life >= 1)
			{
				--m_pMe->life;//减少生命数
				m_pMe->HP = 100;
			}

			//删除战机
			else if ((lifenum == 0 && m_pMe->life < 0) || (lifenum == 0 && (m_pMe->HP <= 0 && m_pMe->life == 0)))//当生命值和生命数均为0时结束游戏
			{
				if (m_pMe!= NULL)
				delete m_pMe;
				m_pMe = NULL;
				break;
			}}
		}
	}

    //战机敌机相撞

	POSITION mmPos1=NULL,mmPos2=NULL;
	for(mmPos1=m_ObjList[enEnemy].GetHeadPosition();( mmPos2 = mmPos1 ) != NULL;)
	{
		CEnemy* pEnemy = (CEnemy*)m_ObjList[enEnemy].GetNext(mmPos1);
		CRect bmRect = pEnemy->GetRect();
		CRect tmpRect;
		if(tmpRect.IntersectRect(&bmRect,mRect))
		{
			//添加爆炸效果
			m_ObjList[enExplosion].AddTail(new CExplosion(mRect.left,mRect.top));

			//删除敌机
			m_ObjList[enEnemy].RemoveAt(mmPos2);
			if (pEnemy != NULL)
			delete pEnemy;
			if (m_pMe != NULL) {
				if (lifenum == 0)
					m_pMe->life -= 1;//被敌机撞一次战机life-1

				if (lifenum == 0 && m_pMe->HP <= 0 && m_pMe->life >= 1)
				{
					--m_pMe->life;//减少生命数
					m_pMe->HP = 100;
				}

				//删除战机
				else if (m_pMe->life < 0 || (m_pMe->HP <= 0 && m_pMe->life == 0))//当生命值和生命数均为0时结束游戏
				{
					if (m_pMe!= NULL)
					delete m_pMe;
					m_pMe = NULL;
					break;
				}
			}
		}

		//清除屏幕中所有普通敌机

		if(GetKey('Z')==1)
		{
			CRect mRect = pEnemy->GetRect();
			m_ObjList[enExplosion].AddTail(new CExplosion(mRect.left,mRect.top));
			m_ObjList[enEnemy].RemoveAt(mmPos2);
			if (pEnemy != NULL)
			delete pEnemy;
			score += 10;//清屏时消除每架飞机score+10
			if(score   >level*600)
			{
				++ level;
				++pass;
			}
		}
	}

	//随机产生宝石

	POSITION nnPos1=NULL,nnPos2=NULL;
	for(nnPos1=m_ObjList[enJewel].GetHeadPosition();( nnPos2 = nnPos1 ) != NULL;)
	{
		CJewel* pJewel = (CJewel*)m_ObjList[enJewel].GetNext(nnPos1);
		CRect bnRect = pJewel->GetRect();
		CRect tmpRect;
		if (tmpRect.IntersectRect(&bnRect, mRect))
		{
			m_pMe->life++;//接到桃心战机life+1
			//PlaySound(TEXT("res\\music\\update.wav"), NULL, SND_FILENAME | SND_ASYNC);
			PlaySound((LPCTSTR)IDR_WAVE1, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);

			//删除红心
			m_ObjList[enJewel].RemoveAt(nnPos2);
			if (pJewel != NULL)
			delete pJewel;
		}
	}
}
int flag2 = 0;
int pause = -1;
BOOL isStarted = FALSE;
void CPlaneGameView::OnTimer(UINT_PTR nIDEvent)////进行定时器的设定 在此设定器中进行各种函数的实现和调用 进行相应时间的刷新
{
	//刷新游戏帧画面: 在内存DC上绘图
	//UpdateFrame(m_pMemDC);
	//AI();
	
	
	CDC *pDC = GetDC();//通过指针名pDC访问CDC对象的成员函数

	CRect rect;
	/*RECT clientrect;
	m_pDC->GetClipBox(&clientrect);
	CMyPlane::GAME_WIDTH = clientrect.right - clientrect.left;//获取窗口坐标
	CMyPlane::GAME_HEIGHT = clientrect.bottom - clientrect.top;*/


	//欢迎界面
	if (isStarted == FALSE)
	{
		//获得客户区矩形区域
		GetClientRect(&rect);
		//内存缓冲CDC
		CDC cdc;
		//内存中承载临时图像的缓冲位图
		CBitmap* cacheBitmap = new CBitmap;

		//用当前设备CDC初始化缓冲CDC
		cdc.CreateCompatibleDC(pDC);
		//绑定pDC和缓冲位图的关系，cdc先输出到缓冲位图中，输出完毕之后再一次性将缓冲位图输出到屏幕
		cacheBitmap->CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		//替换cdc原本的缓冲区为缓冲位图，这样cdc输出的内容就写到了缓冲位图中

		CBitmap *pOldBit = cdc.SelectObject(cacheBitmap);//cache高速缓冲存储器

		startIMG.Draw(&cdc, 0, CPoint(0, 0), ILD_TRANSPARENT);

		static HFONT textFont = CreateFont(50, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 10, 0);//字体
		cdc.SelectObject(textFont);
		cdc.SetTextColor(RGB(255, 255, 0));
		cdc.TextOutW(rect.right / 2 - 200, 200, _T("小鲤鱼 历险记 O(∩_∩)O "));


		static HFONT startFont = CreateFont(12, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 10, 0);//字体
		cdc.SelectObject(textFont);
		cdc.SetTextColor(RGB(0, 0, 0));
		//设置透明背景
		cdc.SetBkMode(TRANSPARENT);
		static HFONT tipFont = CreateFont(24, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 10, 0);
		cdc.SelectObject(tipFont);
		cdc.SetTextColor(RGB(255, 250, 205));// 下面一行字用的颜色
		cdc.TextOutW(rect.right / 2 - 290, 410, _T("海底世界并不安全，按下 空格键 开始海底大冒险吧   (๑^ں^๑)"));
		//将二级缓冲cdc中的数据推送到一级级缓冲pDC中，即输出到屏幕中
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &cdc, 0, 0, SRCCOPY);
		pOldBit->DeleteObject();
		//释放二级cdc
		cdc.DeleteDC();
		//释放缓冲位图
		cacheBitmap->DeleteObject();
		delete cacheBitmap;
		//释放一级pDC
	}


	if (isStarted == FALSE && (GetKeyState(VK_SPACE) < 0)) {
		isStarted = TRUE;                
	}
	if (GetKey(VK_SPACE) == 1)//按下了空格键
		isStarted = TRUE;



	if (isStarted && pause == -1)
	{
		UpdateFrame(m_pMemDC);
		AI();
	}
	ReleaseDC(pDC);


	CView::OnTimer(nIDEvent);
}




void CPlaneGameView::OnDestroy()
{
	CView::OnDestroy();
	this->StopGame();
	// TODO: 在此处添加消息处理程序代码
}

BOOL CPlaneGameView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CView::OnEraseBkgnd(pDC);
}

void CPlaneGameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CPlaneGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//战机复活设置

	switch(nChar)
	{
	case 'Q':
		PlaySound((LPCTSTR)IDR_WAVE3, AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
		m_pMe = new CMyPlane;
		m_pMe->level = 1;
		//score = 0;
		m_pMe->HP = 100;
		m_pMe->life = 3;
		pass=1;
		break;
	case'F':
		lifenum =1;
		break;
	case'G':
		lifenum =0;
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPlaneGameView::OnMouseMove()
{
}

/*
void CPlaneGameView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect Rect;
	this->GetClientRect(&Rect);
	if (point.y >= Rect.top&&point.y <= Rect.bottom - 60 && point.x >= Rect.left&&point.x <= Rect.right - 50)

		plane.setPosition(point.x, point.y);
	CView::OnMouseMove(nFlags, point);
}
*/
