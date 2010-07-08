
#include "StdAfx.h"
#include "Image_Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void KEntityDrawing::GetRandColor(KRGBColor& color)
{
	color.r = BYTE(double(rand() * 0x80) / RAND_MAX + 0.5) + 0x40;
	color.g = BYTE(double(rand() * 0x80) / RAND_MAX + 0.5) + 0x40;
	color.b = BYTE(double(rand() * 0x80) / RAND_MAX + 0.5) + 0x40;
}

static bool IsValidImage(KImage& image)
{
	if (!image.IsValid())
	{
		TRACE("Invalid image!");
		//ASSERT(false);
		return false;
	}

	if (!image.IsHighColor())
	{
		TRACE("Output image must be high color!");
		//ASSERT(false);
		return false;
	}
}

void KEntityDrawing::DrawEntity(KImage& image, KGenericEntity& entity, KRGBColor* color, bool hasDirectAccess)
{
	if (!hasDirectAccess && !IsValidImage(image)) return;
	
	if (!image.HasDirectAccess()) image.BeginDirectAccess(true);

	KRGBColor* drawColor = color;
	if (color == NULL) 
	{
		drawColor = new KRGBColor();
		GetRandColor(*drawColor);
	}

	KPropValue propValue;
	entity.GetPropertyValue(CCS_SEGMENTS, propValue);
	KEntityPointersArray* pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
	ASSERT(pSegments != NULL);

	for (int i = pSegments->GetUpperBound(); i >= 0; --i)
	{
		KRowSegment* pSegment = (KRowSegment*) (pSegments->GetAt(i));
		for (int i = pSegment->intStartColumn; i <= pSegment->intStopColumn; ++i)
			image.Put24BPPPixel(i, pSegment->intRow, drawColor);
	}

	if (color == NULL) delete drawColor;

	if (!hasDirectAccess) image.EndDirectAccess();
}

void KEntityDrawing::DrawBoundingRectangle(KImage& image, KGenericEntity& entity, KRGBColor* color, bool hasDirectAccess)
{
	if (!hasDirectAccess && !IsValidImage(image)) return;
	
	if (!image.HasDirectAccess()) image.BeginDirectAccess(true);

	KRGBColor* drawColor = color;
	if (drawColor == NULL) drawColor = new KRGBColor(0, 0, 0);

	for (int column = entity.boundingRectangle.left; column <= entity.boundingRectangle.right; ++column)
	{
		image.Put24BPPPixel(column, entity.boundingRectangle.top, drawColor);
		image.Put24BPPPixel(column, entity.boundingRectangle.bottom, drawColor);
	}
	for (int row = entity.boundingRectangle.top; row <= entity.boundingRectangle.bottom; ++row)
	{
		image.Put24BPPPixel(entity.boundingRectangle.left, row, drawColor);
		image.Put24BPPPixel(entity.boundingRectangle.right, row, drawColor);
	}

	if (color == NULL) delete drawColor;

	if (!hasDirectAccess) image.EndDirectAccess();
}

void KEntityDrawing::DrawEntityArray(KImage& image, KEntityPointersArray& entities, int drawMode)
{
	if (!IsValidImage(image)) return;

	if (!image.HasDirectAccess()) image.BeginDirectAccess(true);

	KRGBColor drawColor;

	if (drawMode | (int) KEntityDrawing::ENTITY_PIXELS) 
	{
		for (int i = 0; i < entities.GetSize(); ++i)
		{
			GetRandColor(drawColor);
			KGenericEntity* entity = (KGenericEntity*) entities[i];
			DrawEntity(image, *entity, &drawColor, true);
		}
	}
	if (drawMode | (int) KEntityDrawing::BOUNDING_RECTANGLE)
	{
		for (int i = 0; i < entities.GetSize(); ++i)
		{
			KGenericEntity* entity = (KGenericEntity*) entities[i];
			DrawBoundingRectangle(image, *entity, NULL, true);
		}
	}

	image.EndDirectAccess();
}

