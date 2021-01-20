#include <string>
#include <iomanip>
#include <algorithm>
#include <stdlib.h>

#include "PlayMode.h"
#include "Game.h"
#include "WindowUtils.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//Texture origin, bool isStretched value
//..and the rect on the sprite sheet we want
const TexCache::TexData::Sprite missileSpin[]{
	{{0,0}, false, { 0,  0, 53, 48} },
	{{0,0}, false, { 54, 0, 107, 48} },
	{{0,0}, false, { 108, 0, 161, 48}},
	{{0,0}, false, { 162, 0, 220, 48} },
};

const TexCache::TexData::Sprite thrustAnim[]{
	{{0,0}, false, { 0,  0, 15, 16} },
	{{0,0}, false, { 16, 0, 31, 16} },
	{{0,0}, false, { 32, 0, 47, 16} },
	{{0,0}, false, { 48, 0, 64, 16} },
};

const TexCache::TexData::Sprite coinSpin[]{
	{{0,0}, false, { 0, 0, 15, 15} },
	{{0,0}, false, { 16,0,31,15 } },
	{{0,0}, false, { 32,0,47,15 } },
	{{0,0}, false, { 48,0,63,15 } },
	{{0,0}, false, { 0,16,15,31 } },
	{{0,0}, false, { 16,16,31,31 } },
	{{0,0}, false, { 32,16,47,31 } },
	{{0,0}, false, { 48,16,63,31 } }
};

Bullet::Bullet(MyD3D & d3d)
	:GameObj(d3d)
{
	vector<TexCache::TexData::Sprite> frames2(missileSpin, missileSpin + sizeof(missileSpin) / sizeof(missileSpin[0]));
	ID3D11ShaderResourceView* p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "missile.dds", "missile", true, &frames2);

	mSpr.SetTex(*p);
	mSpr.GetAnim().Init(0, 3, 15, true);
	mSpr.GetAnim().Play(true);
	mSpr.SetScale(Vector2(0.5f, 0.5f));
	mSpr.origin = Vector2((missileSpin[0].dim.right - missileSpin[0].dim.left) / 2.f, (missileSpin[0].dim.bottom - missileSpin[0].dim.top) / 2.f);
	mActive = false;
}


void Bullet::Update(float dTime)
{
	if (mActive)
	{
		mSpr.mPos.y -= GC::MISSILE_SPEED * dTime;
		if (mSpr.mPos.y < 0)
			mActive = false;
		mSpr.GetAnim().Update(dTime);
	}
}

void Bullet::OnCollision(GameObj* collider)
{
	mActive = false;
}

//****************************************************************

void Player::Update(float dTime)
{
	if (Game::Get().mMKIn.IsPressed(VK_SPACE) && GetClock() > mFireTimer)
	{
		if (hasDoubleShot) {
			FireBullet(10.f);
			FireBullet(-10.f);
		}
		else 
		{
			FireBullet(0.f);
		}
		mFireTimer = GetClock() + GC::FIRE_DELAY;
	}

	//Reset Double Damage PowerUp
	if (GetClock() > mDDTimer)
	{
		hasDoubleDamage = false;
	}
	//Reset Double Shot PowerUp
	if (GetClock() > mDSTimer)
	{
		hasDoubleShot = false;
	}
	//Reset Double Points PowerUp
	if (GetClock() > mDPTimer)
	{
		hasDoublePoints = false;
	}

	if (mThrusting)
	{
		mThrust.mPos = mSpr.mPos;
		mThrust.mPos.x -= 12;
		mThrust.mPos.y += 24;
		mThrust.SetScale(Vector2(1.5f, 1.5f));
		mThrust.GetAnim().Update(dTime);
	}

	//PowerUp Light Sprites
	if (hasDoubleDamage)
	{
		mRedLight.mPos = mSpr.mPos;
		mRedLight.mPos.x -= 2.5;
	}
	if (hasDoublePoints)
	{
		mBlueLight.mPos = mSpr.mPos;
		mBlueLight.mPos.x -= 2.5;
		mBlueLight.mPos.y += 8;
	}
	if (hasDoubleShot)
	{
		mGreenLight.mPos = mSpr.mPos;
		mGreenLight.mPos.x -= 2.5;
		mGreenLight.mPos.y += 16;
	}


	Game& gm = Game::Get();
	Vector2 mouse{ gm.mMKIn.GetMousePos(false) };
	bool keypressed = gm.mMKIn.IsPressed(VK_UP) || gm.mMKIn.IsPressed(VK_DOWN) ||
		gm.mMKIn.IsPressed(VK_RIGHT) || gm.mMKIn.IsPressed(VK_LEFT);
	bool sticked = false;
	if (gm.mGamepads.IsConnected(0) &&
		(gm.mGamepads.GetState(0).leftStickX != 0 || gm.mGamepads.GetState(0).leftStickX != 0))
		sticked = true;

	if (keypressed || (mouse.Length() > VERY_SMALL) || sticked)
	{
		//move the ship around
		Vector2 pos(0, 0);
		if (gm.mMKIn.IsPressed(VK_UP))
			pos.y -= GC::SPEED * dTime;
		else if (gm.mMKIn.IsPressed(VK_DOWN))
			pos.y += GC::SPEED * dTime;
		if (gm.mMKIn.IsPressed(VK_RIGHT))
			pos.x += GC::SPEED * dTime;
		else if (gm.mMKIn.IsPressed(VK_LEFT))
			pos.x -= GC::SPEED * dTime;

		pos += mouse * GC::MOUSE_SPEED * dTime;

		if (sticked)
		{
			DBOUT("left stick x=" << gm.mGamepads.GetState(0).leftStickX << " y=" << gm.mGamepads.GetState(0).leftStickY);
			pos.x += gm.mGamepads.GetState(0).leftStickX * GC::PAD_SPEED * dTime;
			pos.y -= gm.mGamepads.GetState(0).leftStickY * GC::PAD_SPEED * dTime;
		}

		//keep it within the play area
		pos += mSpr.mPos;
		if (pos.x < mPlayArea.left)
			pos.x = mPlayArea.left;
		else if (pos.x > mPlayArea.right)
			pos.x = mPlayArea.right;
		if (pos.y < mPlayArea.top)
			pos.y = mPlayArea.top;
		else if (pos.y > mPlayArea.bottom)
			pos.y = mPlayArea.bottom;
		pos.y = mPlayArea.bottom;
		mSpr.mPos = pos;
		mThrusting = GetClock() + 0.2f;
	}
}

void Player::AddScore(int amt)
{
	if(hasDoublePoints)
		mScore.SetScore(mScore.GetScore() + (amt * 2));
	else
		mScore.SetScore(mScore.GetScore() + amt);
}

void Player::GivePowerUp(PowerUpType powerType)
{
	switch (powerType) {
	case(PowerUpType::DoubleDamage): hasDoubleDamage = true;
		mDDTimer = GetClock() + GC::POWERUP_TIME;
		break;
	case(PowerUpType::DoubleShot): hasDoubleShot = true;
		mDSTimer = GetClock() + GC::POWERUP_TIME;
		break;
	case(PowerUpType::DoublePoints): hasDoublePoints = true;
		mDPTimer = GetClock() + GC::POWERUP_TIME;
		break;
	}
}

void Player::OnCollision(GameObj* collider)
{
}

void Player::Reset()
{
	hasDoubleShot = false;
	hasDoubleDamage = false;
	hasDoublePoints = false;
}

//Fire bullet from player
void Player::FireBullet(float relpos)
{
	Bullet* pM = mpMyMode->FindFirst<Bullet>(typeid(Bullet), false);
	if (pM) {
		pM->mActive = true;
		pM->mSpr.rotation = PI / 2.f;
		pM->mSpr.mPos = Vector2(mSpr.mPos.x + relpos, mSpr.mPos.y - mSpr.GetScreenSize().y / 2.f);
		pM->isDoubleDamage = hasDoubleDamage;
	}
}

Player::Player()
	: GameObj(Game::Get().GetD3D()),
	mThrust(Game::Get().GetD3D()),
	mRedLight(Game::Get().GetD3D()),
	mBlueLight(Game::Get().GetD3D()),
	mGreenLight(Game::Get().GetD3D())
{
	Init();
}

void Player::Init()
{
	MyD3D& d3d = Game::Get().GetD3D();
	//load a orientate the ship
	ID3D11ShaderResourceView *p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "ship.dds");
	mSpr.SetTex(*p);
	mSpr.SetScale(Vector2(0.1f, 0.1f));
	mSpr.origin = mSpr.GetTexData().dim / 2.f;
	mSpr.rotation = 0;

	//setup the play area
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mPlayArea.left = mSpr.GetScreenSize().x*0.6f;
	mPlayArea.top = mSpr.GetScreenSize().y * 0.6f;
	mPlayArea.right = w - mPlayArea.left;
	mPlayArea.bottom = h * 0.85f;
	mSpr.mPos = Vector2(mPlayArea.left + mSpr.GetScreenSize().x / 2.f, mPlayArea.bottom);

	vector<TexCache::TexData::Sprite> frames(thrustAnim, thrustAnim + sizeof(thrustAnim) / sizeof(thrustAnim[0]));
	p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "thrust.dds", "thrust", true, &frames);
	mThrust.SetTex(*p);
	mThrust.GetAnim().Init(0, 3, 15, true);
	mThrust.GetAnim().Play(true);
	mThrust.rotation = 0;

	p = p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "redlight.dds");
	mRedLight.SetTex(*p);
	mRedLight.SetScale(Vector2(0.1, 0.1));
	p = p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "greenlight.dds", "greenlight");
	mGreenLight.SetTex(*p);
	mGreenLight.SetScale(Vector2(0.1, 0.1));
	p = p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "bluelight.dds", "bluelight");
	mBlueLight.SetTex(*p);
	mBlueLight.SetScale(Vector2(0.1, 0.1));
}

void Player::Render(float dTime, SpriteBatch& batch)
{
	if (mThrusting > GetClock())
		mThrust.Draw(batch);
	GameObj::Render(dTime, batch);
	if (hasDoubleDamage)
		mRedLight.Draw(batch);
	if (hasDoublePoints)
		mBlueLight.Draw(batch);
	if (hasDoubleShot)
		mGreenLight.Draw(batch);
}

Asteroid::Asteroid()
	: GameObj(Game::Get().GetD3D())
{}

void Asteroid::Init()
{
	MyD3D& d3d = Game::Get().GetD3D();
	vector<TexCache::TexData::Sprite> roidAnim;
	const float ASS_SZ = 102;
	int c = 0;
	for (int y = 0; y < 8; ++y)
		for (int x = 0; x < 8; ++x) {
			TexCache::TexData::Sprite roidData;
			roidData.corner = Vector2{ 0,0 };
			roidData.stretchedSides = false;
			roidData.dim = RECTF{ x * ASS_SZ,y * ASS_SZ,x * ASS_SZ + ASS_SZ,y * ASS_SZ + ASS_SZ };
			roidAnim.push_back(roidData);
		}
	ID3D11ShaderResourceView* p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "asteroid.dds", "asteroid", true, &roidAnim);

	mSpr.SetTex(*p);
	mSpr.GetAnim().Init(0, 31, 15, true);
	mSpr.GetAnim().Play(true);
	mSpr.SetScale(Vector2(0.5f, 0.5f));
	mSpr.origin = Vector2(ASS_SZ / 2.f, ASS_SZ / 2.f);
	mActive = false;
}

void PlayMode::InitRoids()
{
	vector<Asteroid*> asteroids = GetGameObjects<Asteroid>();
	assert(asteroids.empty());
	for (int i = 0; i < ROID_CACHE; ++i)
	{
		Asteroid* a = new Asteroid();
		a->Init();
		if (GetRandom(0, 1) == 0)
			a->mSpr.GetAnim().Init(0, 31, GetRandom(10.f, 20.f), true);
		else
			a->mSpr.GetAnim().Init(32, 63, GetRandom(10.f, 20.f), true);
		a->mSpr.GetAnim().SetFrame(GetRandom(a->mSpr.GetAnim().GetStart(), a->mSpr.GetAnim().GetEnd()));
		Add(a);
	}
}
void PlayMode::InitPowerUps()
{
	vector<PowerUp*> powerups = GetGameObjects<PowerUp>();
	assert(powerups.empty());
	for (int i = 0; i < POWERUP_CACHE; ++i)
	{
		PowerUp* a = new PowerUp();
		Add(a);
	}
}

void Asteroid::Update(float dTime) {
	if (mActive)
	{
		int w, h;
		WinUtil::Get().GetClientExtents(w, h);
		mSpr.mPos.y += roidSpeed * dTime;
		if (mSpr.mPos.y > h)
			mActive = false;
		mSpr.GetAnim().Update(dTime);
	}
}

void Asteroid::TakeDamage(float amount)
{
	mHealth -= amount;
	if (mHealth <= 0)
		mActive = false;
}

void Asteroid::Reset()
{
	mHealth = 100.f;
}

void Asteroid::OnCollision(GameObj* collider)
{
	if (typeid(*collider) == typeid(Bullet))
	{
		if (dynamic_cast<Bullet*>(collider)->isDoubleDamage)
			TakeDamage(100.f);
		else
			TakeDamage(50.f);
	}
}

template<class ObjectType>
vector<ObjectType*> PlayMode::GetGameObjects()
{
	vector<ObjectType*> objects;
	for (size_t i = 0; i < mObjects.size(); ++i)
		if (typeid(*mObjects[i]) == typeid(ObjectType))
			objects.push_back(dynamic_cast<ObjectType*>(mObjects[i]));
	return objects;
}

bool RangeIntersect(float min0, float max0, float min1, float max1) {
	return max(min0, max0) >= min(min1, max1) && min(min0, max0) <= max(min1, max1);
}

bool SpriteIntersect(Sprite S1, Sprite S2) {
	S1.GetAnim().SetFrame(S1.GetAnim().GetStart());
	S2.GetAnim().SetFrame(S2.GetAnim().GetStart());
	return RangeIntersect(S1.mPos.x, S1.mPos.x + ((S1.GetTexRect().right * S1.GetScale().x) - (S1.GetTexRect().left * S1.GetScale().x)),
			S2.mPos.x, S2.mPos.x + ((S2.GetTexRect().right * S2.GetScale().x) - (S2.GetTexRect().left * S2.GetScale().x))) &&
		RangeIntersect(S1.mPos.y, S1.mPos.y + ((S1.GetTexRect().top * S1.GetScale().y) - (S1.GetTexRect().bottom * S1.GetScale().y)),
			S2.mPos.y, S2.mPos.y + ((S2.GetTexRect().top * S2.GetScale().y) - (S2.GetTexRect().bottom * S2.GetScale().y)));
}

bool PlayMode::CheckForCollision(GameObj& me, GameObj& them)
{
	bool coll = false;
	
	if ((&them != &me) && SpriteIntersect(me.mSpr, them.mSpr) && them.mActive && me.mActive)
		coll = true;

	return coll;
}

void PlayMode::CheckCollisions()
{
	GameObj* p = FindFirst<Player>(typeid(Player), true);

	for (auto& b : GetGameObjects<Bullet>()) {
		for (auto& a : GetGameObjects<Asteroid>()) {
			if (CheckForCollision(*a, *b)) {
				a->OnCollision(b);
				b->OnCollision(a);
				FindFirst<Player>(typeid(Player), true)->AddScore(1);
			}
			if (CheckForCollision(*a, *p)) {
				Game::Get().GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
				Initalise();
			}
		}
	}
	for (auto& a : GetGameObjects<PowerUp>()) {
		if (CheckForCollision(*a, *p)) {
			FindFirst<Player>(typeid(Player), true)->GivePowerUp(a->GetType());
			a->mActive = false;
			
		}
	}
}

Asteroid* PlayMode::SpawnRoid()
{
	vector<Asteroid*> asteroids = GetGameObjects<Asteroid>();
	assert(!asteroids.empty());
	size_t i = 0;
	Asteroid* p = nullptr;
	while (i < asteroids.size() && !p)
	{
		if (!asteroids[i]->mActive)
			p = asteroids[i];
		++i;
	}

	if (p)
	{
		int w, h;
		WinUtil::Get().GetClientExtents(w, h);
		float radius = asteroids[0]->mSpr.GetScreenSize().Length() / 2.f;
		Vector2& pos = p->mSpr.mPos;
		pos.y = (float)0;
		pos.x = (float)GetRandom((0.f), (w - 20.f));
		p->Reset();
		p->mActive = true;
	}
	return p;
}

PowerUp* PlayMode::SpawnPowerUp()
{
	vector<PowerUp*> powerups = GetGameObjects<PowerUp>();
	assert(!powerups.empty());
	size_t i = 0;
	PowerUp* p = nullptr;
	while (i < powerups.size() && !p)
	{
		if (!powerups[i]->mActive)
			p = powerups[i];
		++i;
	}

	if (p)
	{
		int w, h;
		WinUtil::Get().GetClientExtents(w, h);
		float radius = powerups[0]->mSpr.GetScreenSize().Length() / 2.f;
		Vector2& pos = p->mSpr.mPos;
		pos.y = (float)0;
		pos.x = (float)GetRandom((0.f), (w - 20.f));
		srand(time(NULL));
		int pwrt = rand() % 3;
		switch (pwrt)
		{
		case(0): p->SetType(PowerUpType::DoubleDamage);
			break;
		case(1): p->SetType(PowerUpType::DoublePoints);
			break;
		case(2): p->SetType(PowerUpType::DoubleShot);
			break;
		}
		p->mActive = true;
	}
	return p;
}

void PlayMode::UpdateRoids(float dTime)
{
	vector<Asteroid*> asteroids = GetGameObjects<Asteroid>();
	assert(!asteroids.empty());
	for (auto& a : asteroids) {
		a->Update(dTime);
	}

	if ((GetClock() - mLastSpawnAsteroid) > mSpawnRateSecAsteroid)
	{
		if (SpawnRoid())
			mLastSpawnAsteroid = GetClock();

		//Increase Difficulty gradually by spawning more asteroids as time goes on
		if (!((mSpawnRateSecAsteroid - 0.05f) < 0.2f))
			mSpawnRateSecAsteroid -= 0.005f;
		else
			mSpawnRateSecAsteroid = 0.2f;
	}
	if ((GetClock() - mLastSpawnPowerUp) > mSpawnRateSecPowerUp)
	{
		if (SpawnPowerUp())
			mLastSpawnPowerUp = GetClock();
	}
}

Coin::Coin()
	: GameObj(Game::Get().GetD3D())
{
	MyD3D& d3d = Game::Get().GetD3D();
	vector<TexCache::TexData::Sprite> frames(coinSpin, coinSpin + sizeof(coinSpin) / sizeof(coinSpin[0]));
	ID3D11ShaderResourceView* p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "coin.dds", "coin", true, &frames);
	mSpr.SetTex(*p);
	mSpr.SetScale(Vector2(2, 2));
	mSpr.GetAnim().Init(0, 7, 15, true);
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mSpr.mPos = Vector2(w * 0.8f, h * 0.85f);
	mSpr.GetAnim().Play(true);
	mpFont = new SpriteFont(&d3d.GetDevice(), L"data/fonts/comicSansMS.spritefont");
	assert(mpFont);
}

void Coin::Render(float dTime, DirectX::SpriteBatch& batch)
{
	GameObj::Render(dTime, batch);

	stringstream ss;					
	ss << std::setfill('0') << std::setw(3) << mpPlayer->mScore.GetScore();
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mpFont->DrawString(&batch, ss.str().c_str(), Vector2(w * 0.86f, h * 0.85f), Vector4(1, 1, 1, 1));
}

void Coin::Update(float dTime)
{
	mSpr.GetAnim().Update(dTime);
}

//********************************************************************
const std::string PlayMode::MODE_NAME = "PLAY";

PlayMode::PlayMode()
{
	InitBgnd();
	mObjects.reserve(1000);

	Player *p = new Player();
	p->SetMode(*this);
	p->mActive = true;
	p->mScore.SetScore(0);
	Add(p);
	InitRoids();
	InitPowerUps();
	for (int i = 0; i < 30; ++i)
		Add(new Bullet(Game::Get().GetD3D()));
	Coin *c = new Coin();
	c->GetPlayerPointer(p);
	c->mActive = true;
	Add(c);
}

PlayMode::~PlayMode()
{
	for (size_t i = 0; i < mObjects.size(); ++i)
		Remove(mObjects[i]);
	mObjects.clear();
}

void PlayMode::Initalise()
{
	FindFirst<Player>(typeid(Player), true)->mSpr.mPos = Vector2(53.f, 98.f);
	FindFirst<Player>(typeid(Player), true)->mScore.SetScore(0);
	FindFirst<Player>(typeid(Player), true)->Reset();

	mSpawnRateSecAsteroid = 1.f;
	//Skip the first item which is the player
	for (size_t i = 1; i < mObjects.size(); ++i)
		mObjects[i]->mActive = false;
	FindFirst<Coin>(typeid(Coin), false)->mActive = true;
}

void PlayMode::UpdateBgnd(float dTime)
{
	//scroll the background layers
	int i = 0;
	for (auto& s : mBgnd)
		s.Scroll((dTime * (i) * GC::SCROLL_SPEED) / 2, dTime * (i++) * GC::SCROLL_SPEED);
}

void PlayMode::Update(float dTime)
{
	UpdateBgnd(dTime);

	CheckCollisions();

	UpdateRoids(dTime);

	for (size_t i = 0; i < mObjects.size(); ++i)
		if (mObjects[i]->mActive)
			mObjects[i]->Update(dTime);
}

void PlayMode::Render(float dTime, DirectX::SpriteBatch & batch) {
	for (auto& s : mBgnd)
		s.Draw(batch);
	for (size_t i = 0; i < mObjects.size(); ++i)
		if (mObjects[i]->mActive)
			mObjects[i]->Render(dTime, batch);
}

void PlayMode::InitBgnd()
{
	//a sprite for each layer
	assert(mBgnd.empty());
	mBgnd.insert(mBgnd.begin(), GC::BGND_LAYERS, Sprite(Game::Get().GetD3D()));

	//a neat way to package pairs of things (nicknames and filenames)
	pair<string, string> files[GC::BGND_LAYERS]{
		{ "bgnd0","backgroundlayers/nebulabackground_00.dds" },
		{ "bgnd1","backgroundlayers/nebulabackground_01.dds" },
		{ "bgnd2","backgroundlayers/nebulabackground_02.dds" }
	};
	int i = 0;
	for (auto& f : files)
	{
		//set each texture layer
		ID3D11ShaderResourceView *p = Game::Get().GetD3D().GetCache().LoadTexture(&Game::Get().GetD3D().GetDevice(), f.second, f.first);
		if (!p)
			assert(false);
		mBgnd[i].SetTex(*p);
		//Scale background to screen
		mBgnd[i].SetScale(Vector2(WinUtil::Get().GetClientWidth() / mBgnd[i].GetTexData().dim.x, WinUtil::Get().GetClientHeight() / mBgnd[i].GetTexData().dim.y));
		i++;
	}

}

void PlayMode::ProcessKey(char key)
{
	switch (key)
	{
	case GC::ESC:
		Game::Get().GetModeMgr().SwitchMode(GameOverMode::MODE_NAME);
		break;
	}
}

void PlayMode::Add(GameObj* pObj) {
	assert(pObj);
	mObjects.push_back(pObj);
}

void PlayMode::Remove(GameObj*& pObj) {
	size_t sz = mObjects.size();
	assert(sz > 0);
	mObjects.erase(std::remove(mObjects.begin(), mObjects.end(), pObj));
	assert(sz != mObjects.size());
	delete pObj;
	pObj = nullptr;
}

template<class ObjectType>
ObjectType* PlayMode::FindFirst(const std::type_info& type, bool active) {
	size_t i = 0;
	while (i < mObjects.size() && (typeid(*mObjects[i]) != type || mObjects[i]->mActive != active))
		++i;
	if (i >= mObjects.size())
		return nullptr;
	return dynamic_cast<ObjectType*>(mObjects[i]);
}

PowerUp::PowerUp()
	: GameObj(Game::Get().GetD3D()), pwrut(PowerUpType::DoubleDamage)
{
	MyD3D& d3d = Game::Get().GetD3D();
	vector<TexCache::TexData::Sprite> frames(coinSpin, coinSpin + sizeof(coinSpin) / sizeof(coinSpin[0]));
	ID3D11ShaderResourceView* p = d3d.GetCache().LoadTexture(&d3d.GetDevice(), "coin.dds", "coin", true, &frames);
	mSpr.SetTex(*p);
	mSpr.SetScale(Vector2(2, 2));
	mSpr.GetAnim().Init(0, 7, 15, true);
	int w, h;
	WinUtil::Get().GetClientExtents(w, h);
	mSpr.mPos = Vector2(w * 0.8f, h * 0.85f);
	mSpr.GetAnim().Play(true);
}

void PowerUp::Render(float dTime, DirectX::SpriteBatch& batch)
{
	GameObj::Render(dTime, batch);
}

void PowerUp::Update(float dTime)
{
	if (mActive)
	{
		int w, h;
		WinUtil::Get().GetClientExtents(w, h);
		mSpr.mPos.y += 100.f * dTime;
		if (mSpr.mPos.y > h)
			mActive = false;
		mSpr.GetAnim().Update(dTime);
	}
}

void PowerUp::SetType(PowerUpType type)
{
	pwrut = type;
}

PowerUpType PowerUp::GetType()
{
	return(pwrut);
}
