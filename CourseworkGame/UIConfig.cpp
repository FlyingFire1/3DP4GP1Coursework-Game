#include <d3d11.h>

#include "UIConfig.h"
#include "D3D.h"
#include "MenuNodes.h" 
#include "Game.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


//**************************************************************************

void GenericButtonSetup(MenuButton* pB3, MenuImage *pIm, const string& b1Name, int x, int y, int w, int h)
{
	pB3->buttons[MenuButton::NORMAL].spriteID = 1;
	pB3->buttons[MenuButton::NORMAL].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::NORMAL].colour = Vector4(1, 1, 1, 1);
	pB3->buttons[MenuButton::HOVER].spriteID = 1;
	pB3->buttons[MenuButton::HOVER].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::HOVER].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB3->buttons[MenuButton::PRESSED].spriteID = 2;
	pB3->buttons[MenuButton::PRESSED].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::PRESSED].colour = Vector4(0.95f, 0.95f, 0.95f, 1);

	pB3->mName = b1Name;
	pB3->mX = x;
	pB3->mY = y;
	pB3->mW = w;
	pB3->mH = h;
	pB3->SetParent(*pIm);
}

//**************************************************************************
void BuildTwoButtonUI(MenuMgr& mgr, MenuNode& root, float x, float y, const string& b1, const string& b1Name, const string& b2, const string& b2Name)
{
	//add a background node
	MenuImage *pIm = dynamic_cast<MenuImage*>(&mgr.CreateNode(MenuNode::Type::IMAGE));
	assert(pIm);
	pIm->mName = "Background";
	pIm->mX = x;
	pIm->mY = y;
	pIm->mW = 120;
	pIm->mH = 125;
	pIm->SetParent(root);
	pIm->mTextureName = "data/blueSheet.dds";
	//pIm->mSpriteName = "blue_panel";
	pIm->mSpriteID = 0;

	MenuButton*pB = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
	assert(pB);
	pB->buttons[MenuButton::NORMAL].spriteID = 1;
	pB->buttons[MenuButton::NORMAL].textureName = "data/blueSheet.dds";
	pB->buttons[MenuButton::NORMAL].colour = Vector4(1, 1, 1, 1);
	pB->buttons[MenuButton::HOVER].spriteID = 1;
	pB->buttons[MenuButton::HOVER].textureName = "data/blueSheet.dds";
	pB->buttons[MenuButton::HOVER].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB->buttons[MenuButton::PRESSED].spriteID = 2;
	pB->buttons[MenuButton::PRESSED].textureName = "data/blueSheet.dds";
	pB->buttons[MenuButton::PRESSED].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB->mName = b1Name;
	pB->mX = 20;
	pB->mY = 20;
	pB->mW = 75;
	pB->mH = 40;
	pB->SetParent(*pIm);


	MenuText*pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);
	pT->mName = "title";
	pT->mX = 5;
	pT->mY = 5;
	pT->mW = 60;
	pT->mH = 30;
	pT->mText = b1;
	pT->mFont = "comicsans";
	pT->mPitch = 12;
	pT->SetParent(*pB);


	MenuButton* pB3 = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
	assert(pB3);
	pB3->buttons[MenuButton::NORMAL].spriteID = 1;
	pB3->buttons[MenuButton::NORMAL].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::NORMAL].colour = Vector4(1, 1, 1, 1);
	pB3->buttons[MenuButton::HOVER].spriteID = 1;
	pB3->buttons[MenuButton::HOVER].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::HOVER].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB3->buttons[MenuButton::PRESSED].spriteID = 2;
	pB3->buttons[MenuButton::PRESSED].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::PRESSED].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB3->mName = b2Name;
	pB3->mX = 20;
	pB3->mY = 70;
	pB3->mW = 75;
	pB3->mH = 40;
	pB3->SetParent(*pIm);

	pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);
	pT->mName = "title";
	pT->mX = 5;
	pT->mY = 5;
	pT->mW = 60;
	pT->mH = 30;
	pT->mText = b2;
	pT->mFont = "comicsans";
	pT->mPitch = 12;
	pT->SetParent(*pB3);
}
//**************************************************************************
void BuildThreeButtonUI(MenuMgr& mgr, MenuNode& root, float x, float y, const string& b1, const string& b1Name, const string& b2, const string& b2Name, const string& b3, const string& b3Name)
{
	//add a background node
	MenuImage *pIm = dynamic_cast<MenuImage*>(&mgr.CreateNode(MenuNode::Type::IMAGE));
	assert(pIm);
	pIm->mName = "Background";
	pIm->mX = x;
	pIm->mY = y;
	pIm->mW = 120;
	pIm->mH = 175;
	pIm->SetParent(root);
	pIm->mTextureName = "data/blueSheet.dds";
	pIm->mSpriteID = 0;

	MenuButton*pB = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
	assert(pB);
	pB->buttons[MenuButton::NORMAL].spriteID = 1;
	pB->buttons[MenuButton::NORMAL].textureName = "data/blueSheet.dds";
	pB->buttons[MenuButton::NORMAL].colour = Vector4(1, 1, 1, 1);
	pB->buttons[MenuButton::HOVER].spriteID = 1;
	pB->buttons[MenuButton::HOVER].textureName = "data/blueSheet.dds";
	pB->buttons[MenuButton::HOVER].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB->buttons[MenuButton::PRESSED].spriteID = 2;
	pB->buttons[MenuButton::PRESSED].textureName = "data/blueSheet.dds";
	pB->buttons[MenuButton::PRESSED].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB->mName = b1Name;
	pB->mX = 20;
	pB->mY = 20;
	pB->mW = 75;
	pB->mH = 40;
	pB->SetParent(*pIm);


	MenuText*pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);
	pT->mName = "title";
	pT->mX = 5;
	pT->mY = 5;
	pT->mW = 60;
	pT->mH = 30;
	pT->mText = b1;
	pT->mFont = "comicsans";
	pT->mPitch = 12;
	pT->SetParent(*pB);


	MenuButton* pB3 = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
	assert(pB3);
	pB3->buttons[MenuButton::NORMAL].spriteID = 1;
	pB3->buttons[MenuButton::NORMAL].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::NORMAL].colour = Vector4(1, 1, 1, 1);
	pB3->buttons[MenuButton::HOVER].spriteID = 1;
	pB3->buttons[MenuButton::HOVER].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::HOVER].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB3->buttons[MenuButton::PRESSED].spriteID = 2;
	pB3->buttons[MenuButton::PRESSED].textureName = "data/blueSheet.dds";
	pB3->buttons[MenuButton::PRESSED].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB3->mName = b2Name;
	pB3->mX = 20;
	pB3->mY = 70;
	pB3->mW = 75;
	pB3->mH = 40;
	pB3->SetParent(*pIm);

	pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);
	pT->mName = "title";
	pT->mX = 5;
	pT->mY = 5;
	pT->mW = 60;
	pT->mH = 30;
	pT->mText = b2;
	pT->mFont = "comicsans";
	pT->mPitch = 12;
	pT->SetParent(*pB3);


	MenuButton* pB4 = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
	assert(pB4);
	pB4->buttons[MenuButton::NORMAL].spriteID = 1;
	pB4->buttons[MenuButton::NORMAL].textureName = "data/blueSheet.dds";
	pB4->buttons[MenuButton::NORMAL].colour = Vector4(1, 1, 1, 1);
	pB4->buttons[MenuButton::HOVER].spriteID = 1;
	pB4->buttons[MenuButton::HOVER].textureName = "data/blueSheet.dds";
	pB4->buttons[MenuButton::HOVER].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB4->buttons[MenuButton::PRESSED].spriteID = 2;
	pB4->buttons[MenuButton::PRESSED].textureName = "data/blueSheet.dds";
	pB4->buttons[MenuButton::PRESSED].colour = Vector4(0.95f, 0.95f, 0.95f, 1);
	pB4->mName = b3Name;
	pB4->mX = 20;
	pB4->mY = 120;
	pB4->mW = 75;
	pB4->mH = 40;
	pB4->SetParent(*pIm);

	pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);
	pT->mName = "title";
	pT->mX = 5;
	pT->mY = 5;
	pT->mW = 60;
	pT->mH = 30;
	pT->mText = b3;
	pT->mFont = "comicsans";
	pT->mPitch = 12;
	pT->SetParent(*pB4);
}

void BuildLeaderBoard(MenuMgr& mgr, MenuNode& root, float x, float y, const std::string& b1, const std::string& b1Name)
{	//add a background node
	MenuImage* pIm = dynamic_cast<MenuImage*>(&mgr.CreateNode(MenuNode::Type::IMAGE));
	assert(pIm);
	pIm->mName = "Background";
	pIm->mX = x;
	pIm->mY = y;
	pIm->mW = 100;
	pIm->mH = 150;
	pIm->SetParent(root);
	pIm->mTextureName = "data/blueSheet.dds";
	pIm->mSpriteID = 0;


	MenuText* pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);


	vector<int> scores;
	Game::Get().GetScores(scores);
	int n = 0;
	for (int& s : scores)
	{
		MenuButton* pB4 = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
		assert(pB4);
		char i[20];
		
		sprintf(i, "%d", s);
		string l = i;
		DBOUT(l);
		GenericButtonSetup(pB4, pIm, "n1", 20, (10 + (12 * n)), 60, 10);
		int test = (10 + (25 * n));
		DBOUT(test);
		pB4->SetParent(*pIm);

		sprintf(i, "n%d text", n);
		string k = i;

		pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
		assert(pT);
		pT->mName = k;
		pT->mX = 27;
		pT->mY = 0;
		pT->mW = 20;
		pT->mH = 7;
		pT->mText = l;
		pT->mFont = "comicsans";
		pT->mPitch = 12;
		pT->SetParent(*pB4);
		n++;
	}

	MenuButton* pB5 = dynamic_cast<MenuButton*>(&mgr.CreateNode(MenuNode::Type::BUTTON));
	assert(pB5);

	GenericButtonSetup(pB5, pIm, b1Name, 20, 130, 60, 10);
	pB5->SetParent(*pIm);

	pT = dynamic_cast<MenuText*>(&mgr.CreateNode(MenuNode::Type::TEXT));
	assert(pT);
	pT->mName = "title";
	pT->mX = 20;
	pT->mY = 0;
	pT->mW = 20;
	pT->mH = 7;
	pT->mText = b1;
	pT->mFont = "comicsans";
	pT->mPitch = 12;
	pT->SetParent(*pB5);

}

