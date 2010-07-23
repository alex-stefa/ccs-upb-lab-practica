/*! \addtogroup ImgEnt
 \{ */ 
//! \file Italics.cpp \brief Implementation file for KItalics class

#include "StdAfx.h"
#include "Italics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//! Rotates a point in a plane relative to specified origin
static void RotatePoint(KPagePoint& point, float xOrigin, float yOrigin, float sinVal, float cosVal, 
		/* OUT */ float* xRot, float* yRot)
{
	float xTrans = (float) point.x - xOrigin;
	float yTrans = (float) point.y - yOrigin;

	*xRot = xTrans * cosVal - yTrans * sinVal + xOrigin;
	*yRot = xTrans * sinVal + yTrans * cosVal + yOrigin;
}


//! Creates a black image the size of the given entity and sets the entity pixels to white
static KImage BuildImage(KGenericEntity& entity)
{
	KImage img(CSize(entity.boundingRectangle.Width(), entity.boundingRectangle.Height()), 1);

	KPropValue propValue;
	entity.GetPropertyValue(CCS_SEGMENTS, propValue);
	KEntityPointersArray* pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
	ASSERT(pSegments != NULL);

	KRowSegment* segment;
	for (int i = pSegments->GetUpperBound(); i >= 0; --i)
	{
		segment = (KRowSegment*) (pSegments->GetAt(i));
		img.Fill(PixelRect(
					segment->intStartColumn - entity.boundingRectangle.left, 
					segment->intRow - entity.boundingRectangle.top,
					segment->intStopColumn - entity.boundingRectangle.left,
					segment->intRow - entity.boundingRectangle.top), true);
	}

	return img;
}


//! Gets longest vertical run on a bitonal image image
static int LongestVerticalRun(KImage& image, bool whiteRun = true)
{
	#define WHITE (whiteRun ? 1 : 0)
	#define BLACK (whiteRun ? 0 : 1)

	if (!image.IsBitonal())
	{
		TRACE("Input image is not bitonal!\n");
		ASSERT(false);
		return -1;
	}

	int height = image.GetPixelHeight();
	int width = image.GetPixelWidth();

	BYTE** imageData = new BYTE*[height];

	image.BeginDirectAccess(false);
	for (int i = 0; i < height; i++)
		imageData[i] = image.GetLinePtr(i);
	image.EndDirectAccess();

	int maxLength = -1;
	int start, stop;
	for (int column = 0; column < width; ++column)
	{
		start = stop = 0;
		while (stop < height)
		{
			while ((start < height) && ((imageData[start][column >> 3] & (1 << (7 - (column & 0x07)))) ==  BLACK)) ++start;
			if (start == height) break;
			stop = start;
			while ((stop < height) && ((imageData[stop][column >> 3] & (1 << (7 - (column & 0x07)))) ==  WHITE)) ++stop;
			if (stop - start > maxLength) maxLength = stop - start;
			start = stop;
		}
	}

	delete[] imageData;

	return maxLength;
}


/*!
Computes the maximum width of the convex hull of the entities in the 
KEntityPointersArray, rotates the convex hull by given angle degrees and recomputes
the width.
Return: percentage of italic entities in the array
*/
double KItalics::WidthMethod(KEntityPointersArray& letters, float degAngle)
{
	if (letters.GetSize() <= 0) return 0;

	float sinPlus = sin(KMathematics::DegreesToRadians(+degAngle));
	float cosPlus = cos(KMathematics::DegreesToRadians(+degAngle));
	float sinMinus = sin(KMathematics::DegreesToRadians(-degAngle));
	float cosMinus = cos(KMathematics::DegreesToRadians(-degAngle));

	long italics = 0;

	float initialWidth, leftMinus, rightMinus, leftPlus, rightPlus;
	KGenericEntity* entity;
	KPropValue val;
	KPointSet* convexHull;
	float xRot, yRot, xCenter, yCenter;

	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];

		initialWidth = entity->boundingRectangle.Width();

		entity->GetPropertyValue(CCS_CONVEX_HULL, val);
		convexHull = (KPointSet*) (KPropertyInspector*) val;

		entity->GetPropertyValue(CCS_WEIGHTCENTER_X, val);
		xCenter = (float) (double) val;
		entity->GetPropertyValue(CCS_WEIGHTCENTER_Y, val);
		yCenter = (float) (double) val;

		leftMinus = rightMinus = leftPlus = rightPlus = -1;

		for (int p = 0; p < convexHull->GetSize(); ++p)
		{
			RotatePoint((*convexHull)[p], xCenter, yCenter, sinMinus, cosMinus, &xRot, &yRot);
			if (leftMinus > xRot || leftMinus < 0) leftMinus = xRot;
			if (rightMinus < xRot || rightMinus < 0) rightMinus = xRot;

			RotatePoint((*convexHull)[p], xCenter, yCenter, sinPlus, cosPlus, &xRot, &yRot);
			if (leftPlus > xRot || leftPlus < 0) leftPlus = xRot;
			if (rightPlus < xRot || rightPlus < 0) rightPlus = xRot;

			if (((rightMinus - leftMinus) < initialWidth) && ((rightPlus - leftPlus) > initialWidth)) ++italics;
		}
	}

	return (double) italics / letters.GetSize();
}


/*!
Computes the initial longest vertical black line of an entity
in the array, rotates the image of segments of the entity 
by given angle and recomputes the longest vertical line in each case.
Return: percentage of italic entities.
*/
double KItalics::ChainMethod(KEntityPointersArray& letters, float degAngle)
{
	if (letters.GetSize() <= 0) return 0;

	long italics = 0;

	KGenericEntity* entity;
	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];

		KImage img = BuildImage(*entity);
		//int originalLenght = LongestVerticalRun(img);

		KImage imgcc(img);
		imgcc.RotateGrow(+degAngle); // clockwise
		int plusLength = LongestVerticalRun(imgcc);

		img.RotateGrow(-degAngle); // counter-clockwise
		int minusLength = LongestVerticalRun(img);

		if (plusLength < 0.8 * minusLength) ++italics;
	}

	return (double) italics / letters.GetSize();
}
/*! \} */