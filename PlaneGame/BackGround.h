#pragma once

class BackGround
{
public:
	BackGround(void);
	~BackGround(void);
	void draw(CDC* pDC);
	int speed;
	int score;
	int level;
};
