#pragma once

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)
#define clamp(vl, v, vh) max(vl, min(v, vh))