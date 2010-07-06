
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

bool KEntityDrawing::IsValidImage(KImage& image)
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

void KEntityDrawing::DrawEntity(KImage& image, KGenericEntity& entity, KRGBColor* color,  bool hasDirectAccess)
{
	if (!hasDirectAccess && !IsValidImage(image)) return;
	
	if (!image.HasDirectAccess()) image.BeginDirectAccess(true);

	KRGBColor* drawColor = color;
	if (drawColor == NULL) 
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

void KEntityDrawing::DrawEntityArray(KImage& image, KEntityPointersArray& entities, KRGBColor* color,  bool hasDirectAccess)
{
	if (!hasDirectAccess && !IsValidImage(image)) return;
	
	if (!image.HasDirectAccess()) image.BeginDirectAccess(true);

	KRGBColor* drawColor = color;
	if (drawColor == NULL) 
	{
		drawColor = new KRGBColor();
		GetRandColor(*drawColor);
	}

	for (int i = 0; i < entities.GetSize(); ++i)
		DrawEntity(image, *((KGenericEntity*) entities[i]), drawColor, true);

	if (color == NULL) delete drawColor;

	if (!hasDirectAccess) image.EndDirectAccess();
}

void KEntityDrawing::DrawEntityClusters(KImage& image, CArray<KEntityCollection*, KEntityCollection*>& clusters)
{
	if (!IsValidImage(image)) return;
	
	if (!image.HasDirectAccess()) image.BeginDirectAccess(true);

	for (int i = 0; i < clusters.GetSize(); ++i)
		if (clusters[i]->GetChildren() != NULL)
			DrawEntityArray(image, *(clusters[i]->GetChildren()), NULL, true);

	image.EndDirectAccess();
}
