/*! \addtogroup Imgent
\{ */ 
//! \file Text_Filters.cpp \brief Implementation file for KTextFilters class

#include "StdAfx.h"
#include "Text_Filters.h"
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


#define MARK_DELETE(pEntity) \
	if (KTextFilters::toDelete == NULL) KTextFilters::toDelete = new KEntityPointersArray(); \
	KTextFilters::toDelete->Add(pEntity);


bool KTextFilters::DebugEnabled = false;
char* KTextFilters::DebugOutputPath = ".\\";
char* KTextFilters::DebugFilenamePrefix = "text-filters";
KEntityPointersArray* KTextFilters::toDelete = NULL;


void KTextFilters::DoCleanup()
{
	if (toDelete != NULL)
	{
		toDelete->DestroyAllEntities();
		delete toDelete;
		toDelete = NULL;
	}
}

void KTextFilters::InsideFilter(KEntityPointersArray& initialEntities, 
								/*OUT*/ KEntityPointersArray& filteredEntities)
{
	list<KGenericEntity*> entities;
	for (int i = 0; i < initialEntities.GetSize(); ++i)
	{
		KGenericEntity* entity = (KGenericEntity*) initialEntities[i];
		ASSERT(entity != NULL);
		entities.push_back(entity);
	}

	typedef list<KGenericEntity*>::iterator EntityIter;

	EntityIter iter1 = entities.begin();
	while (iter1 != entities.end())
	{
		EntityIter iter2 = iter1;
		++iter2;
		while (iter2 != entities.end())
		{
			if ((*iter1)->boundingRectangle.PtInRect((*iter2)->boundingRectangle.CenterPoint()) ||
				(*iter2)->boundingRectangle.PtInRect((*iter1)->boundingRectangle.CenterPoint()))
			{
				KEntityCollection* collection = new KEntityCollection((*iter1)->ImagePageOwner);
				MARK_DELETE(collection);
				collection->AddChild(*iter1);
				collection->AddChild(*iter2);
				iter1 = entities.erase(iter1);
				*iter2 = collection;
				break; // stop at first entity inclusion
			}
			++iter2;
		}
		if (iter2 == entities.end()) // if no merging performed..
			++iter1;
	}

	filteredEntities.RemoveAll();
	filteredEntities.SetSize(entities.size());

	int position = 0;
	for (EntityIter iter = entities.begin(); iter != entities.end(); ++iter)
		filteredEntities[position++] = *iter;
}


void KTextFilters::FilterLetters(KEntityPointersArray& letters, 
		bool useWidthFilter, bool useInsideFilter, bool useMergeFilter)
{
	if (letters.GetSize() <= 0) return;

	if (useInsideFilter) InsideFilter(letters, letters);

}

/*  \} */