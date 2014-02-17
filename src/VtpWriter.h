/*
 * VtpWriter.h
 *
 *  Created on: Sep 2, 2013
 *      Author: trifid
 */

#ifndef VTPWRITER_H_
#define VTPWRITER_H_
#include "GradientProcessorQuad.h"

class VtpWriter {
public:
	VtpWriter();
	virtual ~VtpWriter();
	void saveAsc(AscArcStorage &ascArcStorage);
//	void saveDesc(DescArcStorage &ascArcStorage);

};

#endif /* VTPWRITER_H_ */
