#pragma once

#include "defs.h"
#include "util.h"

namespace Rays {

	extern U64 rays[8][64];

	void initRays();
	U64 getRay(int dir, int x);

}
