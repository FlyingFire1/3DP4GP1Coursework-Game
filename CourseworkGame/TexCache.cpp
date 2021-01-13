#include <DDSTextureLoader.h>
#include <filesystem>

#include "TexCache.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

void TexCache::Release()
{
	for (auto& pair : mCache) 
		ReleaseCOM(pair.second.pTex);
	mCache.clear();
}

ID3D11ShaderResourceView* TexCache::LoadTexture(ID3D11Device*pDevice, const std::string& fileName, const std::string& texName, 
										bool appendPath, const vector<TexData::Sprite> *frames)
{
	string name = texName;;
	if (name.empty())
	{
		std::filesystem::path p(fileName);
		name = p.stem().string();
	}

	//search the cache
	MyMap::iterator it = mCache.find(name);
	if (it != mCache.end())
		return (*it).second.pTex;

	//prepare the path for loading
	const string *pPath = &fileName;
	string path;
	if (appendPath)
	{
		path = mAssetPath + fileName;
		pPath = &path;
	}
	std::wstring ws(pPath->begin(), pPath->end());
	//load it
	DDS_ALPHA_MODE alpha;
	ID3D11ShaderResourceView *pT;
	if (CreateDDSTextureFromFile(pDevice, ws.c_str(), nullptr, &pT, 0, &alpha) != S_OK)
	{
		DBOUT("Cannot load " << *pPath << "\n");
		assert(false);
	}
	//save it
	assert(pT);
	mCache.insert(MyMap::value_type(name,TexData(fileName, pT, GetDimensions(pT), frames)));
	return pT;
}


//slowly find a texture by handle

const TexCache::TexData & TexCache::Get(ID3D11ShaderResourceView * pTex) {

	MyMap::iterator it = mCache.begin();
	TexData *p = nullptr;
	while (it != mCache.end() && !p)
	{
		if ((*it).second.pTex == pTex)
			p = &(*it).second;
		++it;
	}
	assert(p);
	return *p;
}

Vector2 TexCache::GetDimensions(ID3D11ShaderResourceView* pTex)
{
	assert(pTex);
	ID3D11Resource* res = nullptr;
	pTex->GetResource(&res);
	assert(res);
	ID3D11Texture2D* texture2d = nullptr;
	HRESULT hr = res->QueryInterface(&texture2d);
	Vector2 dim(0, 0);
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		texture2d->GetDesc(&desc);
		dim.x = static_cast<float>(desc.Width);
		dim.y = static_cast<float>(desc.Height);
	}
	ReleaseCOM(texture2d);
	ReleaseCOM(res);
	return dim;
}


void TexCache::Render(SpriteBatch& spriteBatch, const string& textureName, const RECT& dest, int spriteID, const Vector4& tint, float angle, float depth)
{
	const TexData &data = Get(textureName);
	const TexData::Sprite* sprite = &data.GetSprite(spriteID);

	if (!sprite || !sprite->stretchedSides)
	{
		//no sprite, just render the whole texture or there is a sprite it's one image
		spriteBatch.Draw(data.pTex, dest, (sprite) ? &sprite->dim : nullptr, tint, angle, Vector2(0, 0), SpriteEffects_None, depth);
	}
	else
	{
		//it's a stretched image
		assert(angle == 0);
		int widths[] = { sprite->dim.left, sprite->dim.left + (int)sprite->corner.x,
			sprite->dim.right - (int)sprite->corner.x, (int)sprite->dim.right };
		int heights[] = { sprite->dim.top, sprite->dim.top + (int)sprite->corner.y,
			sprite->dim.bottom - (int)sprite->corner.y, sprite->dim.bottom };

		int dwidths[] = { dest.left, dest.left + (int)sprite->corner.x,
			dest.right - (int)sprite->corner.x, dest.right };
		int dheights[] = { dest.top, dest.top + (int)sprite->corner.y,
			dest.bottom - (int)sprite->corner.y, dest.bottom };

		for (int x = 0; x < 3; ++x)
			for (int y = 0; y < 3; ++y)
			{
				RECT src{ widths[x], heights[y],widths[x + 1], heights[y + 1] };
				RECT dst{ dwidths[x], dheights[y],dwidths[x + 1], dheights[y + 1] };
				spriteBatch.Draw(data.pTex, dst, &src, tint, 0, Vector2(0, 0), SpriteEffects_None, depth);
			}
	}
}
