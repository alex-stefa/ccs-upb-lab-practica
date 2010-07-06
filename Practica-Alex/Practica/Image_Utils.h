

#pragma once

class KEntityDrawing
{
public:
	static void DrawEntity(KImage& image, KGenericEntity& entity, KRGBColor* color = NULL, bool hasDirectAccess = false);
	static void DrawEntityArray(KImage& image, KEntityPointersArray& entities, KRGBColor* color = NULL, bool hasDirectAccess = false);
	static void DrawEntityClusters(KImage& image, CArray<KEntityCollection*, KEntityCollection*>& clusters);
private:
	static bool IsValidImage(KImage& image);
	static void GetRandColor(KRGBColor& color);
};