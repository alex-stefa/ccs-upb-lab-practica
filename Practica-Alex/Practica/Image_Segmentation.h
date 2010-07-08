

#pragma once

class KImageClustering
{
public:
	static void DoClustering(KImage& image, KImagePage& imagePage, /*OUT*/ KEntityPointersArray& clusters);
};