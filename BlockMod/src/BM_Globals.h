#ifndef BM_GLOBALS_H
#define BM_GLOBALS_H

namespace BLOCKMOD {

class Globals {
public:

	/*! Tests, if a given grid distance is approximately zero (with respect to grid spacing).
		Function accounts for rounding errors.
	*/
	static bool nearZero(double gridDistance);

	static double GridSpacing;

	static double LabelFontSize;
};

} // namespace BLOCKMOD


#endif // BM_GLOBALS_H
