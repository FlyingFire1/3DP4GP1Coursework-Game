#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <d3d11.h>

#include "D3DUtil.h"
#include "SpriteBatch.h"

//handy rectangle definer
struct RECTF
{
	float left, top, right, bottom;
	RECTF()
		:left(0), top(0), right(0), bottom(0)
	{};
	RECTF(float l, float t, float r, float b)
		:left(l), top(t), right(r), bottom(b)
	{};
	RECTF(const RECT& r)
		:left((float)r.left), top((float)r.top), right((float)r.right), bottom((float)r.bottom)
	{};
	operator RECT() {
		return RECT{ (int)left,(int)top,(int)right,(int)bottom };
	}
};

//we only ever want one unique texture to be loaded
//it can then be shared between any meshes that need it
class TexCache
{
public:
	//associate a file name with a texture resource
	struct TexData
	{
		std::string fileName;
		ID3D11ShaderResourceView* pTex = nullptr;
		DirectX::SimpleMath::Vector2 dim;
		struct Sprite {
			DirectX::SimpleMath::Vector2 corner;
			bool stretchedSides = false;
			RECT dim;
		};
		std::vector<Sprite> frames;

		TexData() {}
		TexData(const std::string& fName, ID3D11ShaderResourceView*p, const DirectX::SimpleMath::Vector2& _dim)
			: fileName(fName), pTex(p), dim(_dim)
		{
			frames.clear();
		}
		TexData(const std::string& fName, ID3D11ShaderResourceView*p, const DirectX::SimpleMath::Vector2& _dim, const std::vector<Sprite> *_frames)
			:fileName(fName), pTex(p), dim(_dim)

		{
			if (_frames)
				frames = *_frames;
		}
		const Sprite& GetSprite(int id) const {
			assert(id >= 0 && id < (int)frames.size());
			return frames[id];
		}
	};

	//tidy up at the end
	void Release();
	//if this texture is new load it in, otherwise find it and return a handle
	ID3D11ShaderResourceView* LoadTexture(ID3D11Device*pDevice, const std::string& fileName, const std::string& texName="",
											bool appendPath=true, const std::vector<TexData::Sprite> *_frames = nullptr);
	//usually we just have a texture file name, but they're all in a sub folder
	void SetAssetPath(const std::string& path) {
		mAssetPath = path;
	}
	//pull out a texture by nickname = fast
	TexData& Get(const std::string& texName) {
		return mCache.at(texName);
	}
	//slowly find a texture by handle
	const TexData& Get(ID3D11ShaderResourceView *pTex);

	void Render(DirectX::SpriteBatch& spriteBatch, const std::string& textureName, const RECT& dest, const int spriteID, 
						const DirectX::SimpleMath::Vector4& tint = DirectX::SimpleMath::Vector4(1,1,1,1), float angle=0, float depth=0);


private:
	DirectX::SimpleMath::Vector2 GetDimensions(ID3D11ShaderResourceView* pTex);
	//array of texture data
	typedef std::unordered_map<std::string, TexData> MyMap;
	MyMap mCache;

	//some data sub folder with all the textures in
	std::string mAssetPath;
};


