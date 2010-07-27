
#include "StdAfx.h"
#include "Area_Voronoi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


KAreaVoronoi::KVoronoiPoint KAreaVoronoi::KVoronoiCell::GetCenterPoint()
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
	toDelete = new KEntityPointersArray();

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

	EdgeIterator it = voronoiEdges.begin();
	while (it != voronoiEdges.end())
	{
		delete *it;
		it = voronoiEdges.erase(it);
	}

	toDelete->DestroyAllEntities();
	delete toDelete;
}


bool KAreaVoronoi::DeleteEdge(KVoronoiEdge* edge)
{
	for (EdgeIterator it = voronoiEdges.begin(); it != voronoiEdges.end(); ++it)
		if (*it == edge)
		{
			delete *it;
			it = voronoiEdges.erase(it);
			return true;
		}
	return false;
}


bool KAreaVoronoi::IsValid(KVoronoiEdge& edge)
{
	for (EdgeIterator it = voronoiEdges.begin(); it != voronoiEdges.end(); ++it)
		if (*it == &edge)
			return true;
	return false;
}


void KAreaVoronoi::BuildAreaVoronoiDiagram(int sampleRate, float voronoiMinDist)
{
	EdgeIterator it = voronoiEdges.begin();
	while (it != voronoiEdges.end())
	{
		delete *it;
		it = voronoiEdges.erase(it);
	}

	KEntityPixelMapper* mapper = new KEntityPixelMapper(width, height);

	for (int i = 0; i < voronoiCells->GetSize(); ++i)
	{
		KGenericEntity* entity = voronoiCells->GetAt(i)->entity;
		mapper->AddEntity(*entity);
	}

	CArray<CPoint, CPoint>* contour = new CArray<CPoint, CPoint>();
	mapper->GetContourPoints(*contour, KEntityPixelMapper::CONNECTED_8);

	//TRACE("\nContour points: %d\n", contour->GetSize());

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
    vdg.generateVoronoi(x, y, (long) size, 0.0f, (float) width, 0.0f, (float) height, voronoiMinDist);

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
				voronoiEdges.push_back(newEdge);
				cell1->edges[cell2] = newEdge;
				cell2->edges[cell1] = newEdge;
				newEdge->lines->Add(new KVoronoiLine(x1, y1, x2, y2, 
					(short) xs1, (short) ys1, (short) xs2, (short) ys2));
			}
			else
			{
				if (edge1->cell1 == cell1)
					edge1->lines->Add(new KVoronoiLine(x1, y1, x2, y2,
						(short) xs1, (short) ys1, (short) xs2, (short) ys2));
				else
					edge1->lines->Add(new KVoronoiLine(x1, y1, x2, y2, 
						(short) xs2, (short) ys2, (short) xs1, (short) ys1));
			}
		}
    }

	delete mapper;
}

KAreaVoronoi::KVoronoiCell* KAreaVoronoi::MergeCells(KVoronoiCell& vcell1, KVoronoiCell& vcell2)
{
	int index1 = -1;
	int index2 = -1;

	KVoronoiCell* cell1 =  &vcell1;
	KVoronoiCell* cell2 =  &vcell2;
	if (cell1 == cell2) return cell1;

	for (int i = 0; i < voronoiCells->GetSize(); ++i)
	{
		if (voronoiCells->GetAt(i) == cell1) index1 = i;
		if (voronoiCells->GetAt(i) == cell2) index2 = i;
	}
	if (index1 < 0 || index2 < 0) return NULL;

	KVoronoiCell::EdgeMap::iterator edge_it1 = cell1->edges.find(cell2);
	KVoronoiCell::EdgeMap::iterator edge_it2 = cell2->edges.find(cell1);

	if (edge_it1 == cell1->edges.end() || edge_it2 == cell2->edges.end()) return NULL;

	ASSERT(edge_it1->second == edge_it2->second);

	KVoronoiEdge* common_edge = edge_it1->second;

	cell1->edges.erase(edge_it1);
	cell2->edges.erase(edge_it2);

	DeleteEdge(common_edge);

	ASSERT(cell1->entity->ImagePageOwner == cell2->entity->ImagePageOwner);

	KEntityCollection* collection = new KEntityCollection(cell1->entity->ImagePageOwner);
	toDelete->Add(collection);
	collection->AddChild(cell1->entity);
	collection->AddChild(cell2->entity);

	KVoronoiCell* merged_cell = cell1;
	KVoronoiCell* removed_cell = cell2;

	merged_cell->entity = collection;
	voronoiCells->RemoveAt(index2);

	cellMap.erase(cell1->entity);
	cellMap.erase(cell2->entity);
	cellMap[collection] = merged_cell;

	KVoronoiCell::EdgeMap::iterator rem_it = removed_cell->edges.begin();
	while (rem_it != removed_cell->edges.end())
	{
		KVoronoiEdge* mov_edge = rem_it->second;
		KVoronoiCell* other_cell = rem_it->first;

		KVoronoiCell::EdgeMap::iterator mer_it = merged_cell->edges.find(other_cell);

		other_cell->edges.erase(removed_cell);

		if (mer_it != merged_cell->edges.end())
		{
			for (int i = mov_edge->lines->GetSize()-1; i >= 0; --i)
				mer_it->second->lines->Add(mov_edge->lines->GetAt(i));
			mov_edge->lines->RemoveAll();
			DeleteEdge(mov_edge);
		}
		else
		{
			merged_cell->edges[other_cell] = mov_edge;
			other_cell->edges[merged_cell] = mov_edge;
			if (mov_edge->cell1 == removed_cell) 
				mov_edge->cell1 = merged_cell;
			else
				mov_edge->cell2 = merged_cell;
		}

		rem_it = removed_cell->edges.erase(rem_it);
	}

	delete removed_cell;
	return merged_cell;
}


void KAreaVoronoi::GetEntities(/*OUT*/ KEntityPointersArray& entities)
{
	entities.RemoveAll();
	entities.SetSize(voronoiCells->GetSize());
	for (int i = 0; i < voronoiCells->GetSize(); ++i)
		entities[i] = voronoiCells->GetAt(i)->entity;
}

#define VALID_W(x) max(0.0f, min(x, width-1))
#define VALID_H(x) max(0.0f, min(x, height-1))

void KAreaVoronoi::DrawVoronoiDiagram(KImage& image, KRGBColor& edgeColor)
{
	KEntityPointersArray* entities = new KEntityPointersArray();
	GetEntities(*entities);
	KEntityDrawing::DrawEntityArray(image, *entities, KEntityDrawing::ENTITY_PIXELS);

	image.BeginDirectAccess(true);

	for (EdgeIterator it = voronoiEdges.begin(); it != voronoiEdges.end(); ++it)
		for (int j = (*it)->lines->GetSize()-1; j >= 0; --j)
		{
			KVoronoiLine* line = (*it)->lines->GetAt(j);
			KIterators::BresenhamLineIterator(
				VALID_W(line->point1.x), VALID_H(line->point1.y), 
				VALID_W(line->point2.x), VALID_H(line->point2.y), 
				__KIterators__Put24BPPPixel, &image, &edgeColor);
		}

	image.EndDirectAccess();
}


float KAreaVoronoi::KVoronoiEdge::GetMinDistance()
{
	// TODO: finish the rest

	return 0;
}


float KAreaVoronoi::KVoronoiCell::GetMinDistance()
{
	// TODO: finish the rest

	return 0;
}




// UNUSED CODE:

//void KAreaVoronoi::BuildDelaunayDiagram()
//{
//    if (objs.size() == 0)
//        fillObjects();
//
//    VertexSet centers;
//    TriangleSet triangles;
//    EdgeSet edges;
//    QVector<QLineF> rEdges;
//
//	for (int i = 0; i < voronoiCells->GetSize(); ++i)
//	{
//		double x, y;
//		KGenericEntity* entity = voronoiCells->GetAt(i)->entity;
//		KPropValue val;
//		entity->GetPropertyValue(CCS_WEIGHTCENTER_X, val);
//		x = (double) val;
//		entity->GetPropertyValue(CCS_WEIGHTCENTER_Y, val);
//		y = (double) val;
//        centers.insert(x, y);
//    }
//
//    Delaunay d;
//    d.Triangulate(centers, triangles);
//    d.TrianglesToEdges(triangles, edges);
//
//    for (EdgeSet::const_iterator eIt = edges.begin(); eIt != edges.end(); eIt++)
//        rEdges.append(QLineF(eIt->v1->GetPoint(), eIt->v2->GetPoint()));
//
//    return rEdges;
//
//}