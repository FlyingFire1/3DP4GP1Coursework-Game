#include <algorithm>
#include <locale>
#include <codecvt>

#include "SpriteBatch.h"
#include "SpriteFont.h"


#include "MenuMgr.h"
#include "TexCache.h"
#include "D3DUtil.h"
#include "WindowUtils.h"
#include "Input.h"
#include "MenuNodes.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

const float MenuNode::ZDEPTH_UILAYER = 0.01f;


int MenuMgr::Handler::sId = 0;

MenuNode *MenuNode::GetRoot() 
{
	if (mType == Type::ROOT)
		return this;
	assert(mpParent);
	return mpParent->GetRoot();
}

MenuNode *MenuNode::FindNode(const string& name, MenuNode *pIgnore ) 
{
	if (this != pIgnore && mName == name)
		return this;
	for (MenuNode* n : mChildren)
	{
		MenuNode *p = n->FindNode(name, pIgnore);
		if (p)
			return p;
	}
	return nullptr;
}

void MenuNode::AddChild(MenuNode& child) 
{
	if (GetRoot()->FindNode(child.mName))
		assert(false);
	mChildren.push_back(&child);
}

void MenuNode::SetParent(MenuNode& parent) 
{
	mpParent = &parent;
	mpParent->mChildren.push_back(this);
}

MenuNode& MenuNode::CreateNode(const string& type)
{
	MenuNode *p = nullptr;
	if (type == "Image")
		return CreateNode(Type::IMAGE);
	if (type == "Button")
		return CreateNode(Type::BUTTON);
	if (type == "Text")
		return CreateNode(Type::TEXT);

	if (type != "Menu")
	{
		DBOUT("couldn't make menu node " << type);
		assert(false);
	}
	return CreateNode(Type::ROOT);
}

MenuNode& MenuNode::CreateNode(MenuNode::Type type)
{
	MenuNode *p = nullptr;
	switch (type)
	{
	case Type::IMAGE:
		p = new MenuImage();
		break;
	case Type::BUTTON:
		p = new MenuButton();
		break;
	case Type::ROOT:
		p = new MenuPage();
		break;
	case Type::TEXT:
		p = new MenuText();
		break;
	default:
		assert(false);
	}
	assert(p);
	return *p;
}


void MenuNode::GetImageDest(RECT& dest, const Vector2& nPos, const Vector2& nWH, const TexCache::TexData::Sprite *pSprite, const Vector2& offset, const Vector2& scale)
{
	Vector2 wh{ nWH };
	if (pSprite)
	{
		//set destination size to match sprite
		Vector2 sdim = Vector2{ (float)(pSprite->dim.right - pSprite->dim.left), (float)(pSprite->dim.bottom - pSprite->dim.top) };
		if (nWH.x == -1 && nWH.y == -1)
			wh = sdim;
		//do we want to maintain aspect ratio
		else if (nWH.x == -1 && nWH.y != -1)
			wh.x = nWH.y * (sdim.x / sdim.y);
		else if (nWH.x != -1 && nWH.y == -1)
			wh.y = nWH.x * (sdim.y / sdim.x);
	}
	else
		assert(nWH.x >= 0 && nWH.y >= 0);

	dest.left = (LONG)round(offset.x + nPos.x * scale.x);
	dest.top = (LONG)round(offset.y + nPos.y * scale.y);
	dest.right = (LONG)round(dest.left + wh.x * scale.x);
	dest.bottom = (LONG)round(dest.top + wh.y * scale.y);
}

void MenuNode::GetImageDest(RECT& dest, const TexCache::TexData::Sprite* pSprite, const Vector2& offset, const Vector2& scale)
{
	//if no sprite then the destination size is predetermined
	Vector2 wh((float)mW, (float)mH);
	if (pSprite)
	{
		//set destination size to match sprite
		Vector2 sdim = Vector2{ (float)(pSprite->dim.right - pSprite->dim.left), (float)(pSprite->dim.bottom - pSprite->dim.top) };
		if (mW == -1 && mH == -1)
			wh = sdim;
		//do we want to maintain aspect ratio
		else if (mW == -1 && mH != -1)
			wh.x = mH * (sdim.x / sdim.y);
		else if (mW != -1 && mH == -1)
			wh.y = mW * (sdim.y / sdim.x);
	}
	else
		assert(mW >= 0 && mH >= 0);

	dest.left = (LONG)round(offset.x + mX * scale.x);
	dest.top = (LONG)round(offset.y + mY * scale.y);
	dest.right = (LONG)round(dest.left + wh.x * scale.x);
	dest.bottom = (LONG)round(dest.top + wh.y * scale.y);
}



void MenuMgr::Reset()
{
	for (auto* p : mNodes)
		delete p;
	mNodes.clear();
	mMenus.clear();
	mpActiveMenu = nullptr;
	mEventHandlers.clear();
	for (auto& f : mFontCache)
		f.Release();
	mFontCache.clear();
}

MenuNode& MenuMgr::AddMenu(const char* name, int w, int h)
{
	MenuNode& n = CreateNode(MenuNode::Type::ROOT);
	n.mName = name;
	n.mX = n.mY = 0;
	n.mW = w;
	n.mH = h;
	mMenus.push_back(&n);
	return n;
}

void MenuMgr::ShowMenu(const string& name) {
	vector<MenuNode*>::iterator it = find_if(mMenus.begin(), mMenus.end(), [&name](MenuNode*p) { return p->mName == name; });
	assert(it != mMenus.end() && (*it)->mType == MenuNode::Type::ROOT);
	mpActiveMenu = dynamic_cast<MenuPage*>(*it);
	assert(mpActiveMenu);
}

MenuPage& MenuMgr::GetActiveMenuPage() {
	assert(mpActiveMenu);
	return *mpActiveMenu;
}

MenuNode& MenuMgr::CreateNode(MenuNode::Type type)
{
	MenuNode& n = MenuNode::CreateNode(type);
	n.mID = mNodes.size();
	mNodes.push_back(&n);
	return n;
}


MenuNode &MenuMgr::FindNode(const string& menuName, const string& nodeName) 
{
	//find the node by name
	vector<MenuNode*>::iterator it = find_if(mNodes.begin(), mNodes.end(),
		[&menuName](MenuNode* p) { return p->mType == MenuNode::Type::ROOT && p->mName == menuName; });
	assert(it != mNodes.end());
	MenuNode *pN = (*it)->FindNode(nodeName);
	assert(pN);
	return *pN;
}

void MenuMgr::Render(float dTime, SpriteBatch& spriteBatch, TexCache& texCache, MouseAndKeys& mouseAndKeys) {
	if (!mpActiveMenu)
		return;
	Vector2 offset{ 0,0 };
	int sw, sh;
	WinUtil::Get().GetClientExtents(sw, sh);
	Vector2 scale{ (float)sw / mpActiveMenu->mW, (float)sh / mpActiveMenu->mH };
	ExtraData rd(dTime, spriteBatch, texCache, mouseAndKeys, *this);
	mpActiveMenu->Render(rd, offset, scale);
}


void MenuMgr::AddEventHandler(const string& menuName, const string& nodeName, MenuNode::Event etype, Handler func)
{
	MenuNode *pN = &FindNode(menuName, nodeName);

	//is there already an event handler?
	vector<Event>::iterator eit = find_if(mEventHandlers.begin(), mEventHandlers.end(),
		[etype, pN](Event& e) { return e.etype == etype && e.pNode == pN; });
	if (eit != mEventHandlers.end())
		(*eit).funcs.push_back(func);
	else
		mEventHandlers.push_back(Event(pN, etype, func));
}
void MenuMgr::RemoveEventHandler(const string& menuName, const string& nodeName, MenuNode::Event etype, Handler func)
{
	MenuNode *pN = &FindNode(menuName, nodeName);
	vector<Event>::iterator eit = find_if(mEventHandlers.begin(), mEventHandlers.end(),
		[etype, pN](Event& e) { return e.etype == etype && e.pNode == pN; });
	size_t n = (*eit).funcs.size();
	(*eit).funcs.erase(find_if((*eit).funcs.begin(), (*eit).funcs.end(),
		[&func](Handler& f) { return f.GetID() == func.GetID(); }));
	assert(n > (*eit).funcs.size());
}


void MenuMgr::TriggerEvent(MenuNode& node, MenuNode::Event type)
{
	std::vector<Event>::iterator it = find_if(mEventHandlers.begin(), mEventHandlers.end(), [&node,&type](Event& event) {
		return event.etype == type && event.pNode == &node;
	});
	if (it == mEventHandlers.end())
		return;
	for (auto& h : (*it).funcs)
		h.func(node, type);
}


void Font::Release() 
{
	delete mpFont;
	mpFont = nullptr;
}

bool Font::Load()
{
	assert(!mFileName.empty() && !mName.empty() && mPitch>0);
	delete mpFont;
	mpFont = new SpriteFont(&WinUtil::Get().GetD3D().GetDevice(), mFileName.c_str());
	assert(mpFont);
	return mpFont != nullptr;
}


const SpriteFont& MenuMgr::GetFont(const std::string& name, int pitch) const 
{
	std::vector<Font>::const_iterator it = find_if(mFontCache.begin(), mFontCache.end(), [&name, pitch](const Font& f) {
		return f.mName == name && pitch == f.mPitch;
	});
	assert(it != mFontCache.end());
	return *((*it).mpFont);
}


