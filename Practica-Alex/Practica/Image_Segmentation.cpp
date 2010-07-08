
#include "StdAfx.h"
#include "Image_Segmentation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static bool IsValidImage(KImage& image)
{
	if (!image.IsValid())
	{
		TRACE("Invalid image!");
		//ASSERT(false);
		return false;
	}

	if (!image.IsBitonal())
	{
		TRACE("Input image must be bitonal!");
		//ASSERT(false);
		return false;
	}
}

void KImageClustering::DoClustering(KImage& image, KImagePage& imagePage, /*OUT*/ KEntityPointersArray& clusters)
{
	// no clustering, just basic example on how to use framework classes..

	if (!IsValidImage(image)) return;

	//imagePage.RebuildEntities(); // auto performed on GetEntities()
	KEntityPointersArray* pEntities = imagePage.GetEntities();
	ASSERT(pEntities != NULL);

	KEntityCollection* topLeft = new KEntityCollection(&imagePage);
	KEntityCollection* topRight = new KEntityCollection(&imagePage);
	KEntityCollection* bottomLeft = new KEntityCollection(&imagePage);
	KEntityCollection* bottomRight = new KEntityCollection(&imagePage);
	clusters.Add(topLeft);
	clusters.Add(topRight);
	clusters.Add(bottomLeft);
	clusters.Add(bottomRight);

	int width = image.GetPixelWidth();
	int height = image.GetPixelHeight();

	for (int i = 0; i < pEntities->GetSize(); ++i)
	{
		KEntity* pEntity = (KEntity*) pEntities->GetAt(i);
		//pEntity->RebuildOBB(); // auto performed on RebuildEntities()
		KPageRectangle rect = pEntity->boundingRectangle;
	
		if (rect.right < width/2 && rect.bottom < height/2)
			topLeft->AddChild(pEntity);
		if (rect.left > width/2 && rect.bottom < height/2)
			topRight->AddChild(pEntity);
		if (rect.right < width/2 && rect.top > height/2)
			bottomLeft->AddChild(pEntity);
		if (rect.left > width/2 && rect.top > height/2)
			bottomRight->AddChild(pEntity);
	}
}
