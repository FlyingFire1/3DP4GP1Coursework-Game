#include <iostream>
#include <fstream>

#include "Game.h"
#include "WindowUtils.h"
#include "CommonStates.h"
#include "PlayMode.h"
#include "UIConfig.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#include "MenuMgr.h"
#include "MenuNodes.h"


Game::Game(MyD3D& d3d)
	: mD3D(d3d), mpSB(nullptr)
{
	mMKIn.Initialise(WinUtil::Get().GetMainWnd(), true, false);
	mGamepads.Initialise();
	mpSB = new SpriteBatch(&mD3D.GetDeviceCtx());
	ConfigureUI();
	audio.Initialise();
	audio.GetSongMgr()->Load("music");
	audio.GetSfxMgr()->Load("sfx");
	mModeMgr.AddMode(new PlayMode());
	mModeMgr.AddMode(new IntroMode());
	mModeMgr.AddMode(new InfoMode());
	mModeMgr.AddMode(new LeaderboardMode());
	mModeMgr.AddMode(new GameOverMode());
	mModeMgr.SwitchMode(IntroMode::MODE_NAME);
}



//any memory or resources we made need releasing at the end
void Game::Release()
{
	delete mpSB;
	mpSB = nullptr;
	mModeMgr.Release();
}

//called over and over, use it to update game logic
void Game::Update(float dTime)
{
	mGamepads.Update();
	mModeMgr.Update(dTime);
}

//called over and over, use it to render things
void Game::Render(float dTime)
{
	mD3D.BeginRender(Colours::Black);


	CommonStates dxstate(&mD3D.GetDevice());
	mpSB->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(), &mD3D.GetWrapSampler());
	mModeMgr.Render(dTime, *mpSB);
	mMenuMgr.Render(dTime, *mpSB, mD3D.GetCache(), mMKIn, mGamepads);
	mpSB->End();


	mD3D.EndRender();
	mMKIn.PostProcess();
}

void Game::SaveScores(int score)
{
	
	vector<int> save;
	GetScores(save);

	//popping lowest score, adding new score
	for (auto& m : save) {
		if (score > m) {
			save.pop_back();
			save.push_back(score);
			break;
		}
	}

	//lambda sort(high-low)
	std::sort(save.begin(), save.end(), [](int a, int b) {
		return a > b;
		});

	std::ofstream fileWrite("Scores.dat", std::ios::binary | std::ios::trunc);
	fileWrite.write((char*)save.data(), save.size() * sizeof(int)); //writing a binary the size of the content vector points too
	fileWrite.close();
}

void Game::GetScores(vector<int>& scores)
{
	std::ifstream fileRead("Scores.dat", std::ios::binary);
	std::vector<int> load;
	int temp;

	//read file while there are ints left
	while (fileRead.read((char*)&temp, sizeof(int)))
		load.push_back(temp);
	fileRead.close();

	//if there isn't 10 scores in file, make a file with 10 scores
	if (load.size() != 10)
		CreateScoreFile(scores);
	else
		scores = load;

}

void Game::CreateScoreFile(vector<int>& scores)
{
	std::vector<int> save;
	scores = save;
	if (save.size() < 10)
	{
		std::vector<int> save2;
		save2.insert(save2.begin(), 10, 0);
		save = save2;
	}
	while (save.size() != 10)
	{
		save.push_back(0);
	}
	ofstream file("Scores.dat", std::ios::binary | std::ios::trunc);
	file.write((char*)save.data(), save.size() * sizeof(int)); //writing a binary the size of the content vector points too
	file.close();
	scores = save;
}


void Game::ConfigureUI()
{
	mMenuMgr.Reset();
	vector<TexCache::TexData::Sprite> frames{
		{{10,10},true,{192,94,192 + 99,94 + 100}},		//blue_panel
		{{0,0},false,{0,94,0 + 190,94 + 49}},	//blue_button00
		{{0,0},false,{190,49,190 + 190,49 + 45}},	//blue_button01
		{{0,0},false,{190,0,190 + 190,0 + 49}},	//blue_button02
	};
	mD3D.GetCache().LoadTexture(&mD3D.GetDevice(), "data/blueSheet.dds", "data/blueSheet.dds", false, &frames);
	mMenuMgr.LoadFont(L"data/fonts/comicSansMS.spritefont", "comicsans", 12);
	//create an empty menu page for the intro UI
	MenuNode& root = mMenuMgr.AddMenu("Intro", 512, 256);
	BuildThreeButtonUI(mMenuMgr, root, 370, 75, "Start", "start button", "Quit", "quit button", "Info", "info button");
	MenuNode& root2 = mMenuMgr.AddMenu("GameOver", 512, 256);
	BuildThreeButtonUI(mMenuMgr, root2, 370, 75, "Restart", "restart button", "Quit", "quit button", "Leaderboards", "leaderboard button");
	MenuNode& root3 = mMenuMgr.AddMenu("Info", 512, 256);
	BuildTwoButtonUI(mMenuMgr, root3, 370, 120, "Back", "restart button", "Quit", "back button");
	MenuNode& root4 = mMenuMgr.AddMenu("Leaderboard", 512, 256);
	BuildLeaderBoard(mMenuMgr, root4, 200, 50, "Back", "back button");
}




//********************************************************************
const std::string IntroMode::MODE_NAME = "INTRO";

IntroMode::IntroMode()
	: mSpr(Game::Get().GetD3D())
{
	mSpr.SetTex(*Game::Get().GetD3D().GetCache().LoadTexture(&Game::Get().GetD3D().GetDevice(), "start1.dds", "start1"));
	mSpr.SetScale(Vector2(WinUtil::Get().GetClientWidth() / mSpr.GetTexData().dim.x, WinUtil::Get().GetClientHeight() / mSpr.GetTexData().dim.y));

	MenuMgr::Handler h1{ [this](MenuNode& node, MenuNode::Event etype) {HandleUIEvent(node, etype); } };
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	mgr.AddEventHandler("Intro", "quit button", MenuNode::Event::CLICK, h1);
	mgr.AddEventHandler("Intro", "start button", MenuNode::Event::CLICK, h1);
	mgr.AddEventHandler("Intro", "info button", MenuNode::Event::CLICK, h1);

}
void IntroMode::Update(float dTime)
{
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	Game::Get().audio.Update();

	if (!Game::Get().audio.GetSongMgr()->IsPlaying(musicHndl))
		Game::Get().audio.GetSongMgr()->Play("credits", true, false, &musicHndl, 0.2f);

	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_DOWN) && dpBlock < GetClock())
	{
		//Get Menu Button
		MenuButton* quit = dynamic_cast<MenuButton*>(&mgr.FindNode("Intro", "quit button"));
		MenuButton* start = dynamic_cast<MenuButton*>(&mgr.FindNode("Intro", "start button"));
		MenuButton* info = dynamic_cast<MenuButton*>(&mgr.FindNode("Intro", "info button"));

		//DPAD Logic
		if (start->gOver)
		{
			start->gOver = false;
			quit->gOver = true;
		}
		else if (quit->gOver)
		{
			info->gOver = true;
			quit->gOver = false;
		}
		else if (info->gOver)
		{
			info->gOver = false;
			start->gOver = true;
		}
		else
		{
			start->gOver = true;
		}
		dpBlock = GetClock() + dpBlockTime;
	}
	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_UP) && dpBlock < GetClock())
	{		
		//Get Menu Button
		MenuButton* quit = dynamic_cast<MenuButton*>(&mgr.FindNode("Intro", "quit button"));
		MenuButton* start = dynamic_cast<MenuButton*>(&mgr.FindNode("Intro", "start button"));
		MenuButton* info = dynamic_cast<MenuButton*>(&mgr.FindNode("Intro", "info button"));

		//DPAD Logic
		if (start->gOver)
		{
			start->gOver = false;
			info->gOver = true;
		}
		else if (quit->gOver)
		{
			start->gOver = true;
			quit->gOver = false;
		}
		else if (info->gOver)
		{
			info->gOver = false;
			quit->gOver = true;
		}
		else
			start->gOver = true;
		dpBlock = GetClock() + dpBlockTime;
	}
}

void IntroMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	mSpr.Draw(batch);
}

void IntroMode::ProcessKey(char key)
{
	switch (key)
	{
	case GC::SPACE:
		Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME);
		break;
	case GC::ESC:
		PostQuitMessage(0);
		break;
	}
}

void IntroMode::HandleUIEvent(MenuNode& node, MenuNode::Event etype)
{
	if (node.mName == "quit button" && etype == MenuNode::Event::CLICK)
	{
		PostQuitMessage(0);
	}
	else if (node.mName == "info button" && etype == MenuNode::Event::CLICK)
	{
		Game::Get().GetModeMgr().SwitchMode(InfoMode::MODE_NAME);
	}
	else if (node.mName == "start button" && etype == MenuNode::Event::CLICK)
	{
		Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME);
	}
}

bool IntroMode::Exit()
{
	Game::Get().GetMenuMgr().HideMenu();
	Game::Get().audio.GetSongMgr()->Stop(musicHndl);
	return true;
}

void IntroMode::Enter()
{
	Game::Get().GetMenuMgr().ShowMenu("Intro");
}

//********************************************************************
const std::string InfoMode::MODE_NAME = "INFO";

InfoMode::InfoMode()
	: mSpr(Game::Get().GetD3D())
{
	mSpr.SetTex(*Game::Get().GetD3D().GetCache().LoadTexture(&Game::Get().GetD3D().GetDevice(), "info.dds", "info"));
	mSpr.SetScale(Vector2(WinUtil::Get().GetClientWidth() / mSpr.GetTexData().dim.x, WinUtil::Get().GetClientHeight() / mSpr.GetTexData().dim.y));

	MenuMgr::Handler h1{ [this](MenuNode& node, MenuNode::Event etype) {HandleUIEvent(node, etype); } };
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	mgr.AddEventHandler("Info", "back button", MenuNode::Event::CLICK, h1);
	mgr.AddEventHandler("Info", "restart button", MenuNode::Event::CLICK, h1);

}
void InfoMode::Update(float dTime)
{
	MenuMgr& mgr = Game::Get().GetMenuMgr();

	Game::Get().audio.Update();

	if (!Game::Get().audio.GetSongMgr()->IsPlaying(musicHndl))
		Game::Get().audio.GetSongMgr()->Play("credits", true, false, &musicHndl, 0.2f);

	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_DOWN) && dpBlock < GetClock())
	{
		//Get Menu Button
		MenuButton* back = dynamic_cast<MenuButton*>(&mgr.FindNode("Info", "back button"));
		MenuButton* restart = dynamic_cast<MenuButton*>(&mgr.FindNode("Info", "restart button"));
		//DPAD Logic
		if (restart->gOver)
		{
			restart->gOver = false;
			back->gOver = true;
		}
		else
		{
			restart->gOver = true;
			back->gOver = false;
		}
		dpBlock = GetClock() + dpBlockTime;
	}
	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_UP) && dpBlock < GetClock())
	{		
		//Get Menu Button
		MenuButton* quit = dynamic_cast<MenuButton*>(&mgr.FindNode("Info", "back button"));
		MenuButton* restart = dynamic_cast<MenuButton*>(&mgr.FindNode("Info", "restart button"));
		//DPAD Logic
		if (restart->gOver)
		{
			restart->gOver = false;
			quit->gOver = true;
		}
		else
		{
			restart->gOver = true;
			quit->gOver = false;
		}
		dpBlock = GetClock() + dpBlockTime;
	}
}

void InfoMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	mSpr.Draw(batch);
}

void InfoMode::ProcessKey(char key)
{
	switch (key)
	{
	case GC::SPACE:
		Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME);
		break;
	case GC::ESC:
		PostQuitMessage(0);
		break;
	}
}

void InfoMode::HandleUIEvent(MenuNode& node, MenuNode::Event etype)
{
	if (node.mName == "back button" && etype == MenuNode::Event::CLICK)
	{
		PostQuitMessage(0);
	}
	else if (node.mName == "restart button" && etype == MenuNode::Event::CLICK)
	{
		Game::Get().GetModeMgr().SwitchMode(IntroMode::MODE_NAME);
	}
}

bool InfoMode::Exit()
{
	Game::Get().GetMenuMgr().HideMenu();
	Game::Get().audio.GetSongMgr()->Stop(musicHndl);
	return true;
}

void InfoMode::Enter()
{
	Game::Get().GetMenuMgr().ShowMenu("Info");
}

//********************************************************************
const std::string LeaderboardMode::MODE_NAME = "LEADERBOARD";

LeaderboardMode::LeaderboardMode()
	: mSpr(Game::Get().GetD3D())
{
	mSpr.SetTex(*Game::Get().GetD3D().GetCache().LoadTexture(&Game::Get().GetD3D().GetDevice(), "start1.dds", "start"));
	mSpr.SetScale(Vector2(WinUtil::Get().GetClientWidth() / mSpr.GetTexData().dim.x, WinUtil::Get().GetClientHeight() / mSpr.GetTexData().dim.y));

	MenuMgr::Handler h1{ [this](MenuNode& node, MenuNode::Event etype) {HandleUIEvent(node, etype); } };
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	mgr.AddEventHandler("Leaderboard", "back button", MenuNode::Event::CLICK, h1);
}
void LeaderboardMode::Update(float dTime)
{
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	Game::Get().audio.Update();

	if (!Game::Get().audio.GetSongMgr()->IsPlaying(musicHndl))
		Game::Get().audio.GetSongMgr()->Play("credits", true, false, &musicHndl, 0.2f);
	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_DOWN) && dpBlock < GetClock())
	{
		//Get Menu Button
		MenuButton* back = dynamic_cast<MenuButton*>(&mgr.FindNode("Leaderboard", "back button"));
		//DPAD Logic
		back->gOver = true;

		dpBlock = GetClock() + dpBlockTime;
	}
	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_UP) && dpBlock < GetClock())
	{
		//Get Menu Button
		MenuButton* back = dynamic_cast<MenuButton*>(&mgr.FindNode("Leaderboard", "back button"));
		//DPAD Logic
		back->gOver = true;

		dpBlock = GetClock() + dpBlockTime;
	}
}

void LeaderboardMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	mSpr.Draw(batch);
}

void LeaderboardMode::ProcessKey(char key)
{
	switch (key)
	{
	case GC::SPACE:
		Game::Get().GetModeMgr().SwitchMode(PlayMode::MODE_NAME);
		break;
	case GC::ESC:
		PostQuitMessage(0);
		break;
	}
}

void LeaderboardMode::HandleUIEvent(MenuNode& node, MenuNode::Event etype)
{
	if (node.mName == "back button" && etype == MenuNode::Event::CLICK)
	{
		Game::Get().GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
	}

}

bool LeaderboardMode::Exit()
{
	Game::Get().GetMenuMgr().HideMenu();
	Game::Get().audio.GetSongMgr()->Stop(musicHndl);
	return true;
}

void LeaderboardMode::Enter()
{
	Game::Get().GetMenuMgr().ShowMenu("Leaderboard");
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	vector<int> scores;
	Game::Get().GetScores(scores);
	int n = 0;
	for (int& s : scores)
	{
		char i[20];
		sprintf(i, "n%d text", n);
		string l = i;
		sprintf(i, "%d", s);
		string k = i;
		MenuText* text = dynamic_cast<MenuText*>(&mgr.FindNode("Leaderboard", l));
		text->mText = k;
		n++;
	}
}


//********************************************************************
const std::string GameOverMode::MODE_NAME = "GAMEOVER";

GameOverMode::GameOverMode()
	: mSpr(Game::Get().GetD3D())
{
	mSpr.SetTex(*Game::Get().GetD3D().GetCache().LoadTexture(&Game::Get().GetD3D().GetDevice(), "game_over.dds", "gameover"));
	float xscale = (WinUtil::Get().GetClientWidth() * 0.8f) / mSpr.GetTexData().dim.x;
	mSpr.SetScale(Vector2( xscale, xscale));

	MenuMgr::Handler h1{ [this](MenuNode& node, MenuNode::Event etype) {HandleUIEvent(node, etype); } };
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	mgr.AddEventHandler("GameOver", "quit button", MenuNode::Event::CLICK, h1);
	mgr.AddEventHandler("GameOver", "restart button", MenuNode::Event::CLICK, h1);
	mgr.AddEventHandler("GameOver", "leaderboard button", MenuNode::Event::CLICK, h1);
}
void GameOverMode::Update(float dTime)
{
	MenuMgr& mgr = Game::Get().GetMenuMgr();
	Game::Get().audio.Update();

	if (!Game::Get().audio.GetSongMgr()->IsPlaying(musicHndl))
		Game::Get().audio.GetSongMgr()->Play("silence", true, false, &musicHndl, 0.2f);

	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_DOWN) && dpBlock < GetClock())
	{
		//Get Menu Button
		MenuButton* quit = dynamic_cast<MenuButton*>(&mgr.FindNode("GameOver", "quit button"));
		MenuButton* start = dynamic_cast<MenuButton*>(&mgr.FindNode("GameOver", "restart button"));
		MenuButton* leaderboard = dynamic_cast<MenuButton*>(&mgr.FindNode("GameOver", "leaderboard button"));
		//DPAD Logic
		if (start->gOver)
		{
			start->gOver = false;
			quit->gOver = true;
		}
		else if (quit->gOver)
		{
			leaderboard->gOver = true;
			quit->gOver = false;
		}
		else if (leaderboard->gOver)
		{
			start->gOver = true;
			leaderboard->gOver = false;
		}
		else
		{
			start->gOver = true;
		}
		dpBlock = GetClock() + dpBlockTime;
	}
	if (Game::Get().mGamepads.IsPressed(0, XINPUT_GAMEPAD_DPAD_UP) && dpBlock < GetClock())
	{
		//Get Menu Button
		MenuButton* quit = dynamic_cast<MenuButton*>(&mgr.FindNode("GameOver", "quit button"));
		MenuButton* start = dynamic_cast<MenuButton*>(&mgr.FindNode("GameOver", "restart button"));
		MenuButton* leaderboard = dynamic_cast<MenuButton*>(&mgr.FindNode("GameOver", "leaderboard button"));
		//DPAD Logic
		if (start->gOver)
		{
			start->gOver = false;
			leaderboard->gOver = true;
		}
		else if(quit->gOver)
		{
			start->gOver = true;
			quit->gOver = false;
		}
		else if (leaderboard->gOver)
		{
			quit->gOver = true;
			leaderboard->gOver = false;
		}
		else
		{
			start->gOver = true;
		}
		dpBlock = GetClock() + dpBlockTime;
	}
}

void GameOverMode::Render(float dTime, DirectX::SpriteBatch& batch)
{
	mSpr.Draw(batch);
}

void GameOverMode::ProcessKey(char key)
{
	switch (key)
	{
	case GC::SPACE:
		Game::Get().GetModeMgr().SwitchMode(IntroMode::MODE_NAME);
		break;
	case GC::ESC:
		PostQuitMessage(0);
		break;
	}
}

void GameOverMode::HandleUIEvent(MenuNode& node, MenuNode::Event etype)
{
	if (node.mName == "quit button" && etype == MenuNode::Event::CLICK)
	{
		PostQuitMessage(0);
	}
	else if (node.mName == "restart button" && etype == MenuNode::Event::CLICK)
	{
		Game::Get().GetModeMgr().SwitchMode(IntroMode::MODE_NAME);
	}	
	else if (node.mName == "leaderboard button" && etype == MenuNode::Event::CLICK)
	{
		Game::Get().GetModeMgr().SwitchMode(LeaderboardMode::MODE_NAME);
	}
}

bool GameOverMode::Exit()
{
	Game::Get().GetMenuMgr().HideMenu();
	Game::Get().audio.GetSongMgr()->Stop(musicHndl);
	return true;
}

void GameOverMode::Enter()
{
	Game::Get().GetMenuMgr().ShowMenu("GameOver");
}



