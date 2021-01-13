#pragma once

#include <vector>

#include "D3D.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "SpriteFont.h"
#include "Singleton.h"
#include "ModeMgr.h"
#include "GameObj.h"



/*
Animated missile bullet
It goes to sleep when it leaves the screen
*/
class Bullet : public GameObj
{
public:

	Bullet(MyD3D& d3d);
	void Update(float dTime) override;
};

class PlayMode;
class Score
{
public:
	Score() : mScore(0)
	{};

	int GetScore() { return mScore; }
	int SetScore(int s) { return mScore = s; }

	int operator++(int) { return ++mScore; };	//This postfix ++ acts as a prefix version
private:										//This is done so it can be used in pointers
	int mScore;
};
/*
The player's ship, flies around the screen, it shows a thrust
anim when in motion and can periodically fire a bullet forwards
*/
class Player : public GameObj
{
public:
	Player();
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	void Update(float dTime) override;
	//the player needs a link to the mode it belongs to
	void SetMode(PlayMode& pm) {
		mpMyMode = &pm;
	}
	Score mScore;					//The player's score they get from 'roids

private:
	Sprite mThrust;					//flames out the back
	//once we start thrusting we have to keep doing it for 
	//at least a fraction of a second or it looks whack
	float mThrusting = 0;
	RECTF mPlayArea;				//where can I move?
	PlayMode *mpMyMode = nullptr;	//my mode owner
	float mFireTimer = 0;			//time limit on firing
	
	void Init();
};

//The animated roids players need to avoid or shoot
class Asteroid : public GameObj
{
public:
	//Gets D3D for GameObj
	Asteroid();
	//Initialise function
	void Init();
	//Render from GameObj
	//Move left until offscreen, then go inactive
	void Update(float dTime) override;
private:
	const float roidSpeed = 100.f;
};

class Coin : public GameObj
{
public:
	//Gets D3D and initalises the sprite
	Coin();
	//Override the default render to add the text render too
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	//Update Animation
	void Update(float dTime) override;

	void GetPlayerPointer(Player* pPlayer) { mpPlayer = pPlayer; }
private:
	DirectX::SpriteFont *mpFont = nullptr;	//Font for the text to be rendered in
	Player* mpPlayer = nullptr;				//Pointer to the player so we can get his score
};

//horizontal scrolling with player controlled ship
class PlayMode : public AMode
{
public:
	static const std::string MODE_NAME;
	PlayMode();
	~PlayMode();
	void Initalise();		//Set the game up between mode change
	void Update(float dTime) override;
	void Render(float dTime, DirectX::SpriteBatch& batch) override;
	std::string GetMName() const override { return "PLAY"; }
	void ProcessKey(char key) override;
	/*
	add a GameObj to the mObjects array, the object should be indvidually
	dynamically allocated e.g. pmode.Add(new Bullet(d3d));
	*/
	void Add(GameObj* pObj);
	/*
	Remove and delete the specified object from the mObjects array
	*/
	void Remove(GameObj*& pObj);
	/*
	Given the type of the object and whether it should be active or inactive
	find the first matching object in the mObjects array
	*/
	template<class ObjectType>
	ObjectType* FindFirst(const std::type_info& type, bool active);

private:

	std::vector<Sprite> mBgnd;		//parallax layers
	std::vector<GameObj*> mObjects;	//objects needing update/render

	float mSpawnRateSec = 1.f;	//how fast to spawn in new asteroids
	float mLastSpawn = 0;		//used in spawn timing
	const int ROID_CACHE = 32;	//The max amount of Asteroids stored in memory

	//setup once
	void InitBgnd();
	//make it scroll parallax
	void UpdateBgnd(float dTime);

	//create a cache of roids to use and re-use
	void InitRoids();
	//Search through the mObjects to find all of the specified type in existence
	template<class ObjectType>
	std::vector<ObjectType*> GetGameObjects();
	/* Try to make one just off screen to the right that definitely
	isn't touching anything else.*/
	Asteroid* SpawnRoid();
	/* Give each active roid an update and monitor the spawn delay
	creating new roids as needed */
	void UpdateRoids(float dTime);
	//me - an object to check for collision
	//them - the object to check agaisnt
	bool CheckForCollision(GameObj& me, GameObj& them);
	//The parent function that calls CheckForCollisions
	void CheckCollisions();
};
