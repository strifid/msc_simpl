#ifndef PIXEL_H_
#define PIXEL_H_

#include "opencv2/core/core.hpp"

using cv::Point;

class Pixel: public Point {
public:

	std::string toString() const;
	Pixel(int x, int y);
	Pixel();
	virtual ~Pixel();
};

typedef std::vector<Pixel> Pixels;
struct PixelComparator {
	bool operator()(const Pixel& a, const Pixel& b) const {
		if (a.x == b.x)
			return a.y < b.y;
		return a.x < b.x;
	}
};
typedef std::map<Pixel, bool, PixelComparator> StdPointsMap;

class PointsMap: public StdPointsMap {
public:

	void addPointsMap(PointsMap& ptmap) {
		for (PointsMap::iterator it = ptmap.begin(); it != ptmap.end(); it++) {
			putInMap(it->first);
		}
	}
	bool isInMap(uint32_t x, uint32_t y) {
		return isInMap(Pixel(x, y));
	}

	bool isInMap(const Pixel& pxl) {
		iterator it = find(pxl);
		if (it != end())
			return true;
		return false;
	}

	void putInMap(const Pixel& pxl) {
		insert(std::pair<const Pixel, bool>(pxl, true));
	}

	std::string toString() {
		std::string strOut;
		iterator it = begin();
		while (it != end()) {
			strOut += it->first.toString();
			it++;
		}
		return strOut;
	}
};

#endif /* DISCRETEMORSEPIXEL_H_ */
