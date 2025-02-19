#include "D3DUtil.h"
#include "D3D.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

static float gTime = 0;

float GetClock()
{
	return gTime;
}

void AddSecToClock(float sec)
{
	gTime += sec;
}

float GetRandom(float min, float max)
{
	float res = (float)rand() / RAND_MAX;
	res = min + res * (max - min);
	return res;
}


