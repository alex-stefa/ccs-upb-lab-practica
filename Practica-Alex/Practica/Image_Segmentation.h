

#pragma once

class KImageClustering
{
public:
	static void DoClustering(KImage& image, KImagePage& imagePage, /*OUT*/ CArray<KEntityCollection*, KEntityCollection*>& clusters);
private:
	static bool IsValidImage(KImage& image);
};