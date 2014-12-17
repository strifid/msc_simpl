/*
 * Image.h
 *
 *  Created on: Jul 7, 2012
 *      Author: trifid
 */

#ifndef IMAGE_H_
#define IMAGE_H_
#include "opencv2/core/core.hpp"
#include "Vertex.h"
using cv::Mat;
using cv::Point;

class Image{
public:

	static const uint32_t m_enlargeFactor = 50;
	enum BrushColor {
		RED, GREEN, BLUE
	};

	Image();
	virtual ~Image();
	virtual bool initFits(const std::string & path);
	virtual bool init(const std::string& path);

	void saveAs(const std::string& path, bool show = false);

	void drawCircle(Point point, cv::Scalar color, uint32_t radius = 1, uint32_t thickness = 1);
	void paintPixel(Vertex* pxl, BrushColor color = RED);

	uint32_t width() {
		return m_width;
	}
	uint32_t height() {
		return m_height;
	}

	void resetPainting();
	int32_t value(uint32_t x, uint32_t y);

	static uint32_t m_height;
	static uint32_t m_width;

protected:
	void mirrorEdges(Mat&);

	void paint(Mat&);
	std::vector<Point> m_redPxls;
	std::vector<Point> m_bluePxls;
	std::vector<Point> m_greenPxls;

	std::string m_imageName;
	std::string m_imagePath;

	Mat m_img;
	Mat m_forChanges;

};

#endif /* IMAGE_H_ */
