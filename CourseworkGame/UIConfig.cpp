#include <d3d11.h>

#include "UIConfig.h"
#include "D3D.h"
#include "MenuNodes.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//**************************************************************************
void BuildTwoButtonUI(MenuMgr& mgr, MenuNode& root, float x, float y, const string& b1, const string& b1Name, const string& b2, const string& b2Name)
{
	//add a background node
	MenuImage *pIm = dynamic_cast<MenuImage*>(&mgr.CreateNode(MenuNode::Type::IMAGE));
	assert(pIm);
	pIm->mName = "Background";
	pIm->mX = 370;
	pIm->mY = 120;
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
