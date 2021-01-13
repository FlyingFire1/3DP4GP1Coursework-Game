#pragma once

#include "Sprite.h"

/*
GameObjects are anything with a sprite
*/
class GameObj
{
public:
	GameObj(MyD3D& d3d) : mSpr(d3d) {}
	virtual ~GameObj() {}
	//must be implemented by any children
	virtual void Update(float dTime) = 0;
	virtual void Render(float dTime, DirectX::SpriteBatch& batch) {
		if(mActive)
			mSpr.Draw(batch);
	}

	Sprite mSpr;			//main sprite
	bool mActive = false;	//by default its asleep and should not render or update
};

