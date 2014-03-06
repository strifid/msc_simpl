/*
 * Image.h
 *
 *  Created on: Jul 7, 2012
 *      Author: trifid
 */

#ifndef IMAGE_H_
#define IMAGE_H_
#include "opencv2/core/core.hpp"
#include "Subset.h"
#include "Pixel.h"
using cv::Mat;

class Image: public mt::Logable {
public:

	static const uint32_t m_enlargeFactor = 60;
	enum BrushColor {
		RED, GREEN, BLUE
	};

	Image();
	virtual ~Image();
	virtual bool initFits(const std::string & path);
	virtual bool init(const std::string& path);

	void setValue(const Pixel & pt, int32_t val);

	Pixels getAllAround(const Pixel& pxl);
	Pixels getOneConnectedAround(const Pixel& pxl);

	bool isBoundaryPixel(const Pixel& pt);

	void saveAs(const std::string& path, bool show = false);

	std::string& imageName() {
		return m_imageName;
	}
	void imageName(const std::string& str) {
		m_imageName = str;
	}

	void drawCircle(Point point, cv::Scalar color, uint32_t radius = 1, uint32_t thickness = 1);

	void paintPixel(Pixel* pxl, BrushColor color = RED);

	int32_t comparePixels(const Pixel& a, const Pixel& b);

	uint32_t width() {
		return m_width;
	}
	uint32_t height() {
		return m_height;
	}

	uint32_t sensivity() {
		return m_sensivity;
	}
	void sensivity(uint32_t i) {
		m_sensivity = i;
	}

	void resetPainting();
	int32_t value(const Pixel&);

protected:

	uint32_t m_sensivity;

	void paint(Mat&);
	Pixels m_redPxls;
	Pixels m_bluePxls;
	Pixels m_greenPxls;

	std::string m_imageName;
	std::string m_imagePath;

	Mat m_img;
	Mat m_forChanges;
	PointsMap m_analysedPointsMap;

	uint32_t m_height;
	uint32_t m_width;

	Point m_current;

};

#endif /* IMAGE_H_ */
