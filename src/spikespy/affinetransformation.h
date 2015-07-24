#ifndef AFFINETRANSFORMATION_H
#define AFFINETRANSFORMATION_H

#include "cvcommon.h"

class AffineTransformationPrivate;
class AffineTransformation
{
public:
	friend class AffineTransformationPrivate;
	AffineTransformation();
	AffineTransformation(const AffineTransformation &other);
	~AffineTransformation();
	void operator=(const AffineTransformation &other);

	CVPoint map(const CVPoint &p);

	void rotateX(float theta,bool left=true);
	void rotateY(float theta,bool left=true);
	void rotateZ(float theta,bool left=true);

	void scale(float sx,float sy,float sz,bool left=true);

	AffineTransformation inverse() const;

private:
	AffineTransformationPrivate *d;

};

#endif // AFFINETRANSFORMATION_H
