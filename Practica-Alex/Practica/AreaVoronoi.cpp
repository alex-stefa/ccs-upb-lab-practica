
#include "StdAfx.h"
#include "AreaVoronoi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


KVoronoiPoint KVoronoiCell::GetCenterPoint()
{
	KVoronoiPoint center;
	KPropValue val;
	entity->GetPropertyValue(CCS_WEIGHTCENTER_X, val);
	center.x = (float) (double) val;
	entity->GetPropertyValue(CCS_WEIGHTCENTER_Y, val);
	center.y = (float) (double) val;
	return center;
}


KAreaVoronoi::KAreaVoronoi(KImage& image)
{
	if (!image.IsValid())
	{
		TRACE("\nInvalid input image!\n");
		ASSERT(false);
	}
	if (!image.IsBitonal())
	{
		TRACE("\nInput image must be bitonal!\n");
		ASSERT(false);
	}

	width = image.GetPixelWidth();
	height = image.GetPixelHeight();

	pImgPage = new KImagePage(&image);
	pImgPage->SetAsActivePage();
	
	hash = new KHash2D(width, height);
	voronoiCells = new CArray<KVoronoiCell*, KVoronoiCell*>();
	voronoiEdges = new CArray<KVoronoiEdge*, KVoronoiEdge*>();

	KEntityPointersArray* entities = pImgPage->GetEntities();

	for (int i = entities->GetUpperBound(); i >= 0; --i)
	{
		KGenericEntity* entity = (KGenericEntity*) entities->GetAt(i);
		hash->AddBySegments(entity);
		KVoronoiCell* cell = new KVoronoiCell(entity);
		cellMap[entity] = cell;
		voronoiCells->Add(cell);
	}
}


KAreaVoronoi::~KAreaVoronoi()
{
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		delete voronoiCells->GetAt(i);
	delete voronoiCells;

	for (int i = 0; i < voronoiEdges->GetSize(); ++i)
		delete voronoiEdges->GetAt(i);
	delete voronoiEdges;

	delete hash;

	delete pImgPage;
}


void KAreaVoronoi::BuildAreaVoronoiDiagram(int sampleRate, float voronoiMinDist)
{
	CArray<KVoronoiPoint, KVoronoiPoint>* points = new CArray<KVoronoiPoint, KVoronoiPoint>();

	int index = 0;
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
	{
		KPointSet exterior, interior;
		KEntityUtils::GenerateEntityContours(*(voronoiCells->GetAt(i)->entity), exterior, interior);

		for (int p = 0; p < exterior.GetSize(); ++p)
		{
			if (index == 0) points->Add(KVoronoiPoint((float) exterior[p].x, (float) exterior[p].y));
			index = (index + 1) % sampleRate;
		}
		for (int p = 0; p < interior.GetSize(); ++p)
		{
			if (index == 0) points->Add(KVoronoiPoint((float) interior[p].x, (float) interior[p].y));
			index = (index + 1) % sampleRate;
		}
	}

	int size = points->GetSize();
	float* x = new float[size];
	float* y = new float[size];

	for (int i = 0; i < size; ++i)
	{
		x[i] = points->GetAt(i).x;
		y[i] = points->GetAt(i).y;
	}

	VoronoiDiagramGenerator vdg;
    
    float x1, y1, x2, y2, xs1, ys1, xs2, ys2;
    vdg.generateVoronoi(x, y, (long) size, 0, width, 0, height, voronoiMinDist);
    
    delete[] x;
    delete[] y;

 //   vdg.resetIterator();
 //   
 //   while(vdg.getNext(x1, y1, x2, y2, xs1, ys1, xs2, ys2))
	//{
	//	KEntityPointersArray* ents1 = hash->
 //       if (map.at((int)xs1, (int)ys1) != map.at((int)xs2, (int)ys2))
 //           lines.append(QLineF(x1, y1, x2, y2));
 //   }

 //   return lines;








}


void KAreaVoronoi::BuildDelaunayDiagram()
{

    //if (objs.size() == 0)
    //    fillObjects();

    //VertexSet centers;
    //TriangleSet triangles;
    //EdgeSet edges;
    //QVector<QLineF> rEdges;

    //foreach(VzGraphicsObj* obj, objs) {
    //    centers.insert(GetCenter(obj));
    //}

    //Delaunay d;
    //d.Triangulate(centers, triangles);
    //d.TrianglesToEdges(triangles, edges);

    //for (EdgeSet::const_iterator eIt = edges.begin(); eIt != edges.end(); eIt++)
    //    rEdges.append(QLineF(eIt->v1->GetPoint(), eIt->v2->GetPoint()));

    //return rEdges;

}


void KAreaVoronoi::GetVoronoiCells(CArray<KVoronoiCell*, KVoronoiCell*>& cells)
{
	cells.RemoveAll();
	cells.SetSize(voronoiCells->GetSize());
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		cells[i] = voronoiCells->GetAt(i);
}


void KAreaVoronoi::GetVoronoiEdges(CArray<KVoronoiEdge*, KVoronoiEdge*>& edges)
{
	edges.RemoveAll();
	edges.SetSize(voronoiEdges->GetSize());
	for (int i = 0; i < voronoiEdges->GetSize(); ++i)
		edges[i] = voronoiEdges->GetAt(i);
}


