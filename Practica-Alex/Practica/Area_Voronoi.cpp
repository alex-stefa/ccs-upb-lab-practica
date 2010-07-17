
#include "StdAfx.h"
#include "Area_Voronoi.h"

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


KAreaVoronoi::KAreaVoronoi(KEntityPointersArray& entities, int width, int height) : width(width), height(height)
{
	voronoiCells = new CArray<KVoronoiCell*, KVoronoiCell*>();
	voronoiCells->SetSize(entities.GetSize());

	for (int i = 0; i < entities.GetSize(); ++i)
	{
		KGenericEntity* entity = (KGenericEntity*) entities[i];
		KVoronoiCell* cell = new KVoronoiCell(*entity);
		cellMap[entity] = cell;
		voronoiCells->SetAt(i, cell);
	}
}


KAreaVoronoi::~KAreaVoronoi()
{
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		delete voronoiCells->GetAt(i);
	delete voronoiCells;
}


void KAreaVoronoi::GetEntities(/*OUT*/ KEntityPointersArray& entities)
{
	entities.RemoveAll();
	entities.SetSize(voronoiCells->GetSize());
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		entities[i] = voronoiCells->GetAt(i)->entity;
}


int KAreaVoronoi::GetEntityCount()
{
	return voronoiCells->GetSize();
}


void KAreaVoronoi::BuildAreaVoronoiDiagram(int sampleRate, float voronoiMinDist)
{
	KEntityPixelMapper* mapper = new KEntityPixelMapper(width, height);

	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		mapper->AddEntity(*(voronoiCells->GetAt(i)->entity));

	CArray<CPoint, CPoint>* contour = new CArray<CPoint, CPoint>();
	mapper->GetContourPoints(*contour, KEntityPixelMapper::CONNECTED_8);

	TRACE("\nContour points: %d\n", contour->GetSize());

	int size = contour->GetSize() / sampleRate;
	float* x = new float[size];
	float* y = new float[size];

	for (int i = 0; i < size; ++i)
	{
		x[i] = (float) contour->GetAt(i * sampleRate).x;
		y[i] = (float) contour->GetAt(i * sampleRate).y;
	}

	delete contour;

	VoronoiDiagramGenerator vdg;
    
    float x1, y1, x2, y2, xs1, ys1, xs2, ys2;
    vdg.generateVoronoi(x, y, (long) size, 0, width, 0, height, voronoiMinDist);

    delete[] x;
    delete[] y;

    vdg.resetIterator();

    while (vdg.getNext(x1, y1, x2, y2, xs1, ys1, xs2, ys2))
	{
		KGenericEntity* ent1 = mapper->GetEntityAtPixel((short) xs1, (short) ys1);
		KGenericEntity* ent2 = mapper->GetEntityAtPixel((short) xs2, (short) ys2);
		ASSERT(ent1 != NULL);
		ASSERT(ent2 != NULL);

		if (ent1 != ent2)
		{
			KVoronoiCell* cell1 = cellMap[ent1];
			KVoronoiCell* cell2 = cellMap[ent2];
			KVoronoiEdge* edge1 = cell1->edges[cell2];
			KVoronoiEdge* edge2 = cell2->edges[cell1];

			ASSERT(edge1 == edge2);

			if (edge1 == NULL || edge2 == NULL)
			{
				KVoronoiEdge* newEdge = new KVoronoiEdge(*cell1, *cell2);
				cell1->edges[cell2] = newEdge;
				cell2->edges[cell1] = newEdge;
				newEdge->lines->Add(new KVoronoiLine(x1, y1, x2, y2));
			}
			else
				edge1->lines->Add(new KVoronoiLine(x1, y1, x2, y2));
		}
    }

	delete mapper;
}


void KAreaVoronoi::BuildDelaunayDiagram()
{
 //   if (objs.size() == 0)
 //       fillObjects();

 //   VertexSet centers;
 //   TriangleSet triangles;
 //   EdgeSet edges;
 //   QVector<QLineF> rEdges;

	//for (int i = 0; i < voronoiCells->GetSize(); ++i)
	//{
	//	double x, y;
	//	KGenericEntity* entity = voronoiCells->GetAt(i)->entity;
	//	KPropValue val;
	//	entity->GetPropertyValue(CCS_WEIGHTCENTER_X, val);
	//	x = (double) val;
	//	entity->GetPropertyValue(CCS_WEIGHTCENTER_Y, val);
	//	y = (double) val;
 //       centers.insert(x, y);
 //   }

 //   Delaunay d;
 //   d.Triangulate(centers, triangles);
 //   d.TrianglesToEdges(triangles, edges);

 //   for (EdgeSet::const_iterator eIt = edges.begin(); eIt != edges.end(); eIt++)
 //       rEdges.append(QLineF(eIt->v1->GetPoint(), eIt->v2->GetPoint()));

 //   return rEdges;

}


void KAreaVoronoi::GetVoronoiCells(/*OUT*/ CArray<KVoronoiCell*, KVoronoiCell*>& cells)
{
	cells.RemoveAll();
	cells.SetSize(voronoiCells->GetSize());
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		cells[i] = voronoiCells->GetAt(i);
}


