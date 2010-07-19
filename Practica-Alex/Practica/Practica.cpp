// Practica.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tests.h"

#include "Image_Utils.h"
#include "Entity_Utils.h"
#include "Text_Filters.h"
#include "Area_Voronoi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

int main(int argc, char* argv)
{
	clock_t start_time = clock();

	KImage* pTestImg = new KImage(CString("..\\Test_Images\\input\\01BPP.tif"), NULL);
	ASSERT(pTestImg->IsBitonal());

	KImagePage* pImgPage = new KImagePage(pTestImg);
	pImgPage->SetAsActivePage(true);
	
	KEntityPointersArray* entities = new KEntityPointersArray();
	KEntityUtils::CopyEntityArray(*(pImgPage->GetEntities()), *entities);

	//float avg_width_mean = KEntityUtils::GetAverageWidth(*entities, KEntityUtils::ARITHMETIC_MEAN);
	//float avg_width_hist = KEntityUtils::GetAverageWidth(*entities, KEntityUtils::HISTOGRAM_PEAK);
	//float avg_height_mean = KEntityUtils::GetAverageHeight(*entities, KEntityUtils::ARITHMETIC_MEAN);
	//float avg_height_hist = KEntityUtils::GetAverageHeight(*entities, KEntityUtils::HISTOGRAM_PEAK);

	//TRACE("Entity count: %d\n", entities->GetSize());
	//
	//TRACE("Avg Width [mean]: %.3f\n", avg_width_mean);
	//TRACE("Avg Width [histogram]: %.3f\n", avg_width_hist);
	//TRACE("Avg Height [mean]: %.3f\n", avg_height_mean);
	//TRACE("Avg Height [histogram]: %.3f\n", avg_height_hist);

	//TRACE("Removed if not in [0..5]*avg_size: %d\n", KEntityUtils::FilterBySize(*entities, *entities, 
	//	0, 0, 5 * avg_width_mean, 5 * avg_height_mean));

	//KImage outImg(pTestImg->GetPixelSize(), 24);
	//outImg.Invert();
	//KEntityDrawing::DrawEntityArray(outImg, *entities, KEntityDrawing::BOUNDING_RECTANGLE + KEntityDrawing::ENTITY_PIXELS);
	//outImg.WriteImage(CString("..\\Test_Images\\input\\01BPP-test-ent.tif"));

	//KTextFilters::FilterLetters(*entities);
	//TRACE("Entity count: %d\n", entities->GetSize());
	
	//KImage outImg2(pTestImg->GetPixelSize(), 24);
	//outImg2.Invert();
	//KEntityDrawing::DrawEntityArray(outImg2, *entities, KEntityDrawing::BOUNDING_RECTANGLE + KEntityDrawing::ENTITY_PIXELS);
	//outImg2.WriteImage(CString("..\\Test_Images\\input\\01BPP-test-filters-inside.tif"));

	//TRACE("\nNumber of Pixels:\n");
	//for (int i = 0; i < entities->GetSize(); ++i)
	//{
	//	KGenericEntity* pEntity = (KGenericEntity*) entities->GetAt(i);
	//	KPropValue val;
	//	pEntity->GetPropertyValue(CCS_NUMBER_OF_PIXELS, val);
	//	TRACE("%d ", (int) val);
	//}

	//TRACE("\nConvex Hull:\n");
	//for (int i = 0; i < entities->GetSize(); ++i)
	//{
	//	KGenericEntity* pEntity = (KGenericEntity*) entities->GetAt(i);
	//	KPropValue val;
	//	pEntity->GetPropertyValue(CCS_CONVEX_HULL, val);
	//	KPointSet* hull = (KPointSet*) (KPropertyInspector*) val;
	//	TRACE("%d ", hull->GetSize());
	//}

	//KTextFilters::DoCleanup();

	KAreaVoronoi* voronoize = new KAreaVoronoi(*entities, pTestImg->GetPixelWidth(), pTestImg->GetPixelHeight());
	voronoize->BuildAreaVoronoiDiagram();
	TRACE("Voronoi Cells: %d\n", voronoize->GetCellCount());
	KImage outImg3(pTestImg->GetPixelSize(), 24);
	outImg3.Invert();
	voronoize->DrawVoronoiDiagram(outImg3);
	outImg3.WriteImage(CString("..\\Test_Images\\input\\01BPP-test-voronoi.tif"));

	delete voronoize;


	delete pImgPage;
	delete entities;
	delete pTestImg;

	TRACE("\n\nALL DONE IN %.3f (s)!", (float) (clock() - start_time) / CLOCKS_PER_SEC);
	getchar();

	ExitCCSData();  // DO NOT REMOVE!

	return 0;
}
