#pragma once

#include <vector>
#include <string>
#include <windef.h>

#include "D3D.h"
#include "SimpleMath.h"
#include "TexCache.h"

//forward declarations
namespace DirectX { class SpriteBatch; }
namespace DirectX { class SpriteFont; }
class MouseAndKeys;
class MenuMgr;
class MenuPage;

/*
General information menu nodes might need access to, saves passing it as lots of individual parameters
*/
struct ExtraData
{
	ExtraData(float _dTime, DirectX::SpriteBatch& _spriteBatch, TexCache& _texCache, MouseAndKeys& _mouseAndKeys, MenuMgr& _menuMgr)
		: dTime(_dTime), spriteBatch(_spriteBatch), texCache(_texCache), mouseAndKeys(_mouseAndKeys), menuMgr(_menuMgr) {};
	float dTime;
	DirectX::SpriteBatch& spriteBatch;
	TexCache& texCache;
	MouseAndKeys& mouseAndKeys;
	MenuMgr& menuMgr;
};

/*
A menu node is a single element within a user interface
*/
class MenuNode
{
public:
	/*
		ROOT - it doesn't display anything, but it's children are all the parts of one screen of a user interface
		IMAGE - a non - interactive image, this could be a logo picture or a background frame for other parts of the UI
		BUTTON - mouse clickable button
		TEXT - anywhere we want text, even the text in a button is one of these text nodes
	*/
	enum class Type { NONE, ROOT, IMAGE, BUTTON, TEXT };
	enum class Event { NONE, CLICK }; //a mouse click is an event
	static const float ZDEPTH_UILAYER; //by default all user interface sprites render on this layer
	std::string mName;			//name of node, unique within one page, but might be duplicated on another page
	Type mType = Type::NONE;//what is it, a button?
	int mID = -1;			//unique ID to ensure two menu nodes can't be confused
	int mX, mY, mW, mH;		//original dimensions in screen coordinates at default resolution

	MenuNode(Type type) : mType(type) {}
	virtual ~MenuNode() {}
	/*
	Render this node and all its children
	rdata - any extra data a node might need for rendering
	offset - a change in position relative to the parent
	scale - a change in scale relative to the parent
	*/
	virtual void Render(const ExtraData& rdata, const DirectX::SimpleMath::Vector2& offset, const DirectX::SimpleMath::Vector2& scale) = 0 {
		for (auto p : mChildren)
			p->Render(rdata, offset, scale);
	}

	/*
	A non-polymorphic way to create nodes
	One works by type the other by a type-string
	*/
	static MenuNode& CreateNode(Type type);
	static MenuNode& CreateNode(const std::string& type);
	/*
	Recursive function to get a link to the root node (there should always be one)
	*/
	MenuNode *GetRoot();
	/*
	Recursive function to search for a node by name, it searches in this node and all its children
	*/
	MenuNode *FindNode(const std::string& name, MenuNode *pIgnore = nullptr);
	/*
	Add a menu node as a child of this node
	*/
	void AddChild(MenuNode& child);
	/*
	Like this node to a parent node
	*/
	void SetParent(MenuNode& parent);

	void GetImageDest(RECT& dest, const TexCache::TexData::Sprite * sprite, const DirectX::SimpleMath::Vector2& offset, 
		const DirectX::SimpleMath::Vector2& scale);
	static void GetImageDest(RECT& dest, const DirectX::SimpleMath::Vector2& nPos, const DirectX::SimpleMath::Vector2& nWH, 
		const TexCache::TexData::Sprite *pSprite, const DirectX::SimpleMath::Vector2& offset, const DirectX::SimpleMath::Vector2& scale);


private:

	MenuNode *mpParent = nullptr;	//my parent

protected:
	std::vector<MenuNode*> mChildren;	//all my children

};


class Font
{
public:
	void Release();
	bool Load();

	std::string mName;
	std::wstring mFileName;
	DirectX::SpriteFont *mpFont = nullptr;
	int mPitch=0;
};

/*
Manager for all user interface screens
*/
class MenuMgr
{
public:
	~MenuMgr() {
		Reset();
	}
	//clean out all screens and reset everything
	void Reset();
	/*
	Add a menu screen
	name - unique name like "Intro" or "GameOver"
	w,h - resolution of the menu screen
	*/
	MenuNode& AddMenu(const char* name, int w, int h);
	//activate a particular menu screen by name
	void ShowMenu(const std::string& name);
	//deactivate the currently active menu screen
	void HideMenu() {
		mpActiveMenu = nullptr;
	}
	//get hold of the current menu
	MenuPage& GetActiveMenuPage();
	//create any type of node (button, text, etc)
	MenuNode& CreateNode(MenuNode::Type type);
	/*
	Find a particular node by name
	A node should be unique within a particular manu screen
	menuName - which menu has the node in its tree
	nodeName - what is it called
	*/
	MenuNode &FindNode(const std::string& menuName, const std::string& nodeName);
	/*
	Render whichever menu screen is active
	dTime - elapsed time
	spriteBatch - for sprite rendering
	texCache - textures will be needed
	mouseAndKeys - input may be necessary
	*/
	void Render(float dTime, DirectX::SpriteBatch& spriteBatch, TexCache& texCache, MouseAndKeys& mouseAndKeys);

	//when something happens on a menu we need to be able to trigger code to run
	class Handler {
	public:
		//set a unique ID for this handler
		Handler() {
			id = sId++;
		}
		/*
		Construct a handler with a pointer to a function to handle the event
		So if there is a particlar menuNode like a button and a particular
		event like a CLICK we can write a function to respond to it
		*/
		Handler(std::function<void(MenuNode&, MenuNode::Event)> _func)
			:func(_func) {
			id = sId++;
		}
		/*
		This is a function pointer, it's using the standard template library
		which has a great feature called std::function that can handle any type of function
		even a class member!
		*/
		std::function<void(MenuNode&, MenuNode::Event)> func;
		//get the handler's ID number
		int GetID() const { return id; }
	private:
		int id; //unique ID for each user interface event we want to handle
		static int sId; //we can use this to generate unique IDs
	};

	/*
	Once we have a function that can respond to a particular UI event, we can add it to the 
	appropriate menu screen
	menuName - which screen to add the function to
	nodeName - which menu node to respond to (like a button)
	etype - what type of event does this relate to (like a CLICK)
	func - the function pointer to use when the event happens
	*/
	void AddEventHandler(const std::string& menuName, const std::string& nodeName, MenuNode::Event etype, Handler func);
	/*
	Similar to above, but to remove a function
	*/
	void RemoveEventHandler(const std::string& menuName, const std::string& nodeName, MenuNode::Event etype, Handler func);
	/*
	The menu manager can hold multiple fonts for printing
	name - friendly name for the font
	pitch - we can have fonts at different pitches (big and small)
	*/
	const DirectX::SpriteFont& GetFont(const std::string& name, int pitch) const;
	/*
	The menu manager can hold multiple fonts for any menu screen to use
	fileName - to load the font
	name - a unique friendly name the menus can use
	pitch - the pitch of the font, so you can have the same font loaded in different sizes
	*/
	bool LoadFont(const std::wstring& fileName, const std::string& name, int pitch) {
		mFontCache.push_back(Font{ name,fileName,nullptr,pitch });
		return mFontCache.back().Load();
	}

private:
	std::vector<MenuNode*> mNodes;	//menu nodes like buttons and text, could be hundreds of them
	std::vector<MenuNode*> mMenus;	//root menu nodes that represent individual pages of UI screens
	/*
	Event handling system

	*/
	struct Event
	{
		Event(MenuNode *_pNode, MenuNode::Event _etype, Handler _func)
		{
			pNode = _pNode;
			etype = _etype;
			funcs.push_back(_func);
		}
		MenuNode *pNode;
		MenuNode::Event etype;
		std::vector<Handler> funcs;
	};
	std::vector<Event> mEventHandlers;
	
	MenuPage *mpActiveMenu = nullptr;	//if the menu system is visible, there will be one active menu

	std::vector<Font> mFontCache;	//library of fonts

public:
	/*
	*/
	void TriggerEvent(MenuNode& node, MenuNode::Event type);

};

