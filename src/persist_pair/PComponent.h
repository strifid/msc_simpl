/*
 * PComponent.h
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#ifndef PCOMPONENT_H_
#define PCOMPONENT_H_

class PComponent {
public:
	PComponent();
	virtual ~PComponent();


	enum Type{
		TYPE_0,
		TYPE_1,
		TYPE_2
	};

protected:


};

typedef PComponent* PComponentPtr;
#endif /* PCOMPONENT_H_ */
