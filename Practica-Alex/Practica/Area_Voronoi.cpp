
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
	//KVoronoiPoint center;
	//KPropValue val;
	//entity->GetPropertyValue(CCS_WEIGHTCENTER_X, val);
	//center.x = (float) (double) val;
	//entity->GetPropertyValue(CCS_WEIGHTCENTER_Y, val);
	//center.y = (float) (double) val;
	//return center;

	KPagePoint center = entity->boundingRectangle.CenterPoint();
	return KVoronoiPoint(center.x, center.y);
}


KAreaVoronoi::KAreaVoronoi(KEntityPointersArray& entities, int width, int height) : width(width), height(height)
{
	toDelete = new KEntityPointersArray();

	voronoiCells.SetSize(entities.GetSize());

	for (int i = 0; i < entities.GetSize(); ++i)
	{
		KGenericEntity* entity = (KGenericEntity*) entities[i];
		KVoronoiCell* cell = new KVoronoiCell(*entity);
		cellMap[entity] = cell;
		voronoiCells[i] = cell;
	}
}


KAreaVoronoi::~KAreaVoronoi()
{
	for (int i = 0; i < voronoiCells.GetSize(); ++i)
		delete voronoiCells[i];

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


bool KAreaVoronoi::IsValid(KVoronoiEdge* edge)
{
	if (edge == NULL) return false;
	for (EdgeIterator it = voronoiEdges.begin(); it != voronoiEdges.end(); ++it)
		if (*it == edge)
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

	for (int i = 0; i < voronoiCells.GetSize(); ++i)
	{
		voronoiCells[i]->SetNeedRecompute();
		voronoiCells[i]->edges.clear();
		KGenericEntity* entity = voronoiCells[i]->entity;
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
				newEdge->lines.Add(KVoronoiLine(x1, y1, x2, y2, 
					(short) xs1, (short) ys1, (short) xs2, (short) ys2));
			}
			else
			{
				if (edge1->cell1 == cell1)
					edge1->lines.Add(KVoronoiLine(x1, y1, x2, y2,
						(short) xs1, (short) ys1, (short) xs2, (short) ys2));
				else
					edge1->lines.Add(KVoronoiLine(x1, y1, x2, y2, 
						(short) xs2, (short) ys2, (short) xs1, (short) ys1));
			}
		}
    }

	delete mapper;
}


KAreaVoronoi::KVoronoiCell* KAreaVoronoi::RemoveEdge(KAreaVoronoi::KVoronoiEdge* edge)
{
	if (!IsValid(edge)) return NULL;

	KVoronoiCell* cell1 =  edge->cell1;
	KVoronoiCell* cell2 =  edge->cell2;
	ASSERT(cell1 != cell2);

	int index1 = -1;
	int index2 = -1;

	for (int i = 0; i < voronoiCells.GetSize(); ++i)
	{
		if (voronoiCells[i] == cell1) index1 = i;
		if (voronoiCells[i] == cell2) index2 = i;
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
	voronoiCells.RemoveAt(index2);

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
			for (int i = mov_edge->lines.GetSize()-1; i >= 0; --i)
				mer_it->second->lines.Add(mov_edge->lines[i]);
			mov_edge->lines.RemoveAll();
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

	//merged_cell->SetNeedRecompute();

	delete removed_cell;
	return merged_cell;
}


void KAreaVoronoi::GetEntities(/*OUT*/ KEntityPointersArray& entities)
{
	entities.RemoveAll();
	entities.SetSize(voronoiCells.GetSize());
	for (int i = 0; i < voronoiCells.GetSize(); ++i)
		entities[i] = voronoiCells[i]->entity;
}


#define VALID_W(x) max(0.0f, min(x, width-1))
#define VALID_H(x) max(0.0f, min(x, height-1))

void KAreaVoronoi::DrawVoronoiDiagram(KImage& image, KRGBColor& edgeColor)
{
	KEntityPointersArray* entities = new KEntityPointersArray();
	GetEntities(*entities);
	KEntityDrawing::DrawEntityArray(image, *entities, KEntityDrawing::ENTITY_PIXELS);
	delete entities;

	image.BeginDirectAccess(true);

	for (EdgeIterator it = voronoiEdges.begin(); it != voronoiEdges.end(); ++it)
		for (int j = (*it)->lines.GetSize()-1; j >= 0; --j)
		{
			KVoronoiLine& line = (*it)->lines[j];
			KIterators::BresenhamLineIterator(
				(int) VALID_W(line.point1.x), (int) VALID_H(line.point1.y), 
				(int) VALID_W(line.point2.x), (int) VALID_H(line.point2.y), 
				__KIterators__Put24BPPPixel, &image, &edgeColor);
		}

	image.EndDirectAccess();
}


static inline float EuclidDist(KAreaVoronoi::KVoronoiPoint& point1, KAreaVoronoi::KVoronoiPoint& point2)
{
	return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
}


static inline float ManhattanDist(KAreaVoronoi::KVoronoiPoint point1, KAreaVoronoi::KVoronoiPoint point2)
{
	return abs(point1.x - point2.x) + abs(point1.y - point2.y);
}


static float GetDistance(KAreaVoronoi::KVoronoiPoint& point, KAreaVoronoi::KVoronoiLine& line, bool manhattan = false)
{
	float A = line.point2.y - line.point1.y;
	float B = line.point1.x - line.point2.x;
	float C = (line.point1.y * line.point2.x) - (line.point1.x * line.point2.y);

	float distLine = abs(A * point.x + B * point.y + C) / sqrt(A * A + B * B);

	float dist1 = (manhattan) ? ManhattanDist(point, line.point1) : EuclidDist(point, line.point1);
	float dist2 = (manhattan) ? ManhattanDist(point, line.point2) : EuclidDist(point, line.point2);

	float AP = 1;
	float BP = - A/B;
	float CP = - ((AP * point.x) + (BP * point.y));

	float sign1 = AP * line.point1.x + BP * line.point1.y + CP;
	float sign2 = AP * line.point2.x + BP * line.point2.y + CP;

	if (sign1 * sign2 > 0)
		return min(dist1, dist2);
	return distLine;
}


float KAreaVoronoi::KVoronoiEdge::GetMinDistance()
{
	if (needRecompute)
	{
		minDistance = -1;

		float currDist, dist1, dist2;

		for (int i = 0; i < lines.GetCount(); ++i)
		{
			dist1 = GetDistance(this->cell1->GetCenterPoint(), lines[i]);
			dist2 = GetDistance(this->cell2->GetCenterPoint(), lines[i]);
			currDist = min(dist1, dist2);
			if (minDistance > currDist || minDistance < 0) minDistance = currDist;
		}

		needRecompute = false;
	}

	return minDistance;
}


float KAreaVoronoi::KVoronoiCell::GetMinDistance()
{
	if (needRecompute)
	{
		minDistance = -1;

		for (EdgeMap::iterator it = edges.begin(); it != edges.end(); ++it)
			if (minDistance > it->second->GetMinDistance() || minDistance < 0)
				minDistance = it->second->GetMinDistance();

		needRecompute = false;
	}

	return minDistance;
}


bool KAreaVoronoi::ShouldRemoveEdge(KVoronoiEdge& edge)
{
	KPageRectangle& rect1 = edge.cell1->entity->boundingRectangle;
	KPageRectangle& rect2 = edge.cell2->entity->boundingRectangle;
	if (rect1.top > rect2.bottom || rect1.bottom < rect2.top)
		return false;

	if (edge.GetMinDistance() > 2 * min(edge.cell1->GetMinDistance(), edge.cell2->GetMinDistance()))
		return false;

	//if (..) // need more conditions..
	//	return false;

	return true;
}


int KAreaVoronoi::MergeEntities()
{
	list<KVoronoiEdge*> marked;

	for (EdgeIterator it = voronoiEdges.begin(); it != voronoiEdges.end(); ++it)
		if (ShouldRemoveEdge(**it))
			marked.push_back(*it);

	int removed = 0;
	for (EdgeIterator it = marked.begin(); it != marked.end(); ++it)
		if (RemoveEdge(*it) != NULL)
			++removed;

	return removed;
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