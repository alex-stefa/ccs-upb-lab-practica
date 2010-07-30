// Practica.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tests.h"

#include "Image_Utils.h"
#include "Entity_Utils.h"
#include "Text_Filters.h"
#include "Text_Lines.h"
#include "Area_Voronoi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INPUT_FOLDER CString("..\\Test_Images\\input\\")
#define OUTPUT_FOLDER CString("..\\Test_Images\\input\\")
#define DEFAULT_EXTENSION CString(".tif")

#define FULL_INPUT_FILE(filename) INPUT_FOLDER + CString(filename) + DEFAULT_EXTENSION
#define FULL_OUTPUT_FILE(filename) OUTPUT_FOLDER + CString(filename) + DEFAULT_EXTENSION

#define BASE_FILE "01BPP"
//#define BASE_FILE "01BPP2"
//#define BASE_FILE "otsu2d"

#define INPUT_FILE FULL_INPUT_FILE(BASE_FILE)
#define OUTPUT_FILE(sufix) FULL_OUTPUT_FILE(CString(BASE_FILE) + CString(sufix))

using namespace std;


int main(int argc, char* argv)
{
	clock_t start_time = clock();

	KImage* pTestImg = new KImage(INPUT_FILE, NULL);
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

	//{
	//	KImage outImg(pTestImg->GetPixelSize(), 24);
	//	outImg.Invert();
	//	KEntityDrawing::DrawEntityArray(outImg, *entities, 
	//		KEntityDrawing::BOUNDING_RECTANGLE + KEntityDrawing::ENTITY_PIXELS);
	//	outImg.WriteImage(OUTPUT_FILE("-test-initial-entities"));
	//}

	TRACE("Initial Entity count: %d\n", entities->GetSize());
	//KTextFilters::FilterLetters(*entities); // just inside filter !!
	TRACE("Merged Entity count: %d\n", entities->GetSize());
	
	//{
	//	KImage outImg(pTestImg->GetPixelSize(), 24);
	//	outImg.Invert();
	//	KEntityDrawing::DrawEntityArray(outImg, *entities, 
	//		KEntityDrawing::BOUNDING_RECTANGLE + KEntityDrawing::ENTITY_PIXELS);
	//	outImg.WriteImage(OUTPUT_FILE("-test-filters-inside"));
	//}

	KAreaVoronoi* voronoize = new KAreaVoronoi(*entities, pTestImg->GetPixelWidth(), pTestImg->GetPixelHeight());
	voronoize->BuildAreaVoronoiDiagram();
	TRACE("Voronoi Cells: %d\n", voronoize->GetCellCount());
	{
		KImage outImg(pTestImg->GetPixelSize(), 24);
		outImg.Invert();
		voronoize->DrawVoronoiDiagram(outImg);
		outImg.WriteImage(OUTPUT_FILE("-test-voronoi"));
	}
	int removed = voronoize->MergeEntities();
	TRACE("Edges Removed: %d\n", removed);
	{
		KImage outImg(pTestImg->GetPixelSize(), 24);
		outImg.Invert();
		voronoize->DrawVoronoiDiagram(outImg);
		outImg.WriteImage(OUTPUT_FILE("-test-voronoi-merged"));
	}
	delete voronoize;

	//KEntityPointersArray* lines = new KEntityPointersArray();
	//KTextLines::BuildLines(*entities, *lines);
	//TRACE("Text Lines: %d\n", lines->GetSize());
	//{
	//	KImage outImg(pTestImg->GetPixelSize(), 24);
	//	outImg.Invert();
	//	KEntityDrawing::DrawEntityArray(outImg, *lines, 
	//		KEntityDrawing::ENTITY_PIXELS + KEntityDrawing::BOUNDING_RECTANGLE);
	//	outImg.WriteImage(OUTPUT_FILE("-test-lines"));
	//}
	//delete lines;

	KTextLines::DoCleanup();
	KTextFilters::DoCleanup();

	delete pImgPage;
	delete entities;
	delete pTestImg;

	TRACE("\n\nALL DONE IN %.3f (s)!", (float) (clock() - start_time) / CLOCKS_PER_SEC);
	getchar();

	ExitCCSData();  // DO NOT REMOVE!

	return 0;
}
