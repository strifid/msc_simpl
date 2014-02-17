/*
 * FitsReader.h
 *
 *  Created on: Feb 17, 2014
 *      Author: trifid
 */

#ifndef FITSREADER_H_
#define FITSREADER_H_

class FitsReader {
public:

	bool init(const std::string& path);
	FitsReader();
	virtual ~FitsReader();
};

#endif /* FITSREADER_H_ */
