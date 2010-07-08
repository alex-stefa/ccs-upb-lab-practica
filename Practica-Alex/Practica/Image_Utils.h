

#pragma once

class KEntityDrawing
{
public:
	const enum DrawMode { ENTITY_PIXELS = 1, BOUNDING_RECTANGLE = 2 };
	static void DrawEntity(KImage& image, KGenericEntity& entity, KRGBColor* color = NULL, bool hasDirectAccess = false);
	static void DrawBoundingRectangle(KImage& image, KGenericEntity& entity, KRGBColor* color = NULL, bool hasDirectAccess = false);
	static void DrawEntityArray(KImage& image, KEntityPointersArray& entities, int drawMode = (int) KEntityDrawing::ENTITY_PIXELS);
	static void GetRandColor(KRGBColor& color);
};