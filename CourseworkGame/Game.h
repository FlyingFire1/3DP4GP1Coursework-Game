#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

#include "Input.h"
#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Singleton.h"
#include "ModeMgr.h"
#include "MenuMgr.h"

namespace GC {
	const int ESC = 27;
	const int SPACE = 32;
	const float SPEED = 250.f;
	const float MOUSE_SPEED = 5000.f;
	const float PAD_SPEED = 500.f;
	const float FIRE_DELAY = 0.25f;
	const float POWERUP_TIME = 5.f;
	const float MISSILE_SPEED = 300.f;
	const float SCROLL_SPEED = 5.f;
	const int BGND_LAYERS = 3;
};

//start of the game
class IntroMode : public AMode
{
public:
	static const std::string MODE_NAME;
	IntroMode();
	void Update(float dTime) override;
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	std::string GetMName() const override {
		return "INTRO";
	}
	void ProcessKey(char key) override;
	bool Exit() override;
	void Enter() override;
private:
	Sprite mSpr;
	float dpBlock = GetClock();
	float dpBlockTime = 0.2f;
	void HandleUIEvent(MenuNode& node, MenuNode::Event etype);
};

//start of the game
class InfoMode : public AMode
{
public:
	static const std::string MODE_NAME;
	InfoMode();
	void Update(float dTime) override;
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	std::string GetMName() const override {
		return "INFO";
	}
	void ProcessKey(char key) override;
	bool Exit() override;
	void Enter() override;
private:
	Sprite mSpr;
	float dpBlock = GetClock();
	float dpBlockTime = 0.2f;
	void HandleUIEvent(MenuNode& node, MenuNode::Event etype);
};


//end of the game
class GameOverMode : public AMode
{
public:
	static const std::string MODE_NAME;
	GameOverMode();
	void Update(float dTime) override;
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	std::string GetMName() const override {
		return "GAMEOVER";
	}
	void ProcessKey(char key) override;
	bool Exit() override;
	void Enter() override;
private:
	Sprite mSpr;
	float dpBlock = GetClock();
	float dpBlockTime = 0.2f;
	void HandleUIEvent(MenuNode& node, MenuNode::Event etype);
};

/*
Basic wrapper for a game
*/
class Game : public Singleton<Game> 
{
public:
	//input
	MouseAndKeys mMKIn;
	Gamepads mGamepads;

	Game(MyD3D& d3d);
	~Game() {
		Release();
	}
	//shut it all down
	void Release();
	//usual update+render+windiwsKeys process
	void Update(float dTime);
	void Render(float dTime);
	void ProcessKey(char key) {
		mModeMgr.ProcessKey(key);
	}
	void SaveScores(int score);
	void GetScores(std::vector<int>& scores);
	void CreateScoreFile(std::vector<int>& scores);
	//getters
	MyD3D& GetD3D() { return mD3D; }
	ModeMgr& GetModeMgr() { return mModeMgr; }
	MenuMgr& GetMenuMgr() { return mMenuMgr; }
	
private:
	MyD3D& mD3D;	//main access point to D3D
	DirectX::SpriteBatch *mpSB = nullptr;	//for sprite rendering
	ModeMgr mModeMgr;	//different bits of the game: intro, game, gameOver
	MenuMgr mMenuMgr;	//different menu screens

	//create the different menuscreens and setup each tree of nodes
	void ConfigureUI();
};


