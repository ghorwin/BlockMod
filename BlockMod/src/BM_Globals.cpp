#include "BM_Globals.h"

#include <cmath>

namespace BLOCKMOD {

bool Globals::nearZero(double gridDistance) {
	return std::fabs(gridDistance/Globals::GridSpacing) < 1e-6;
}

double Globals::GridSpacing = 8; // in pixel
double Globals::LabelFontSize = 8;

} // namespace BLOCKMOD
