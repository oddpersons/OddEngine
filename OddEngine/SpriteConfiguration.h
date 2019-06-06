#pragma once
#include "pch.h"

class DllExport SpriteConfiguration
{
public:
	SpriteConfiguration(std::wstring, IWICImagingFactory*);
	~SpriteConfiguration();

	void CreateDeviceDependentBitmap(ID2D1DeviceContext*);

	ID2D1Bitmap* GetBitmap();
	ID2D1Bitmap* NextSprite();

private:

	winrt::hstring _ConfigJSON;
	int SpritesPerRow = 0;
	int SpritesPerColumn = 0;
	int UnusedSprites = 0;
	int SpriteCount = 0;
	int SpriteWidth = 0;
	int SpriteHeight = 0;
	int ParentBitmapX;
	int ParentBitmapY;

	int CurrentSprite = 0;

	winrt::com_ptr<IWICFormatConverter> converter;
	winrt::com_ptr<IWICBitmapFrameDecode> pBitmapFrame;
	
	std::wstring Path;
	
	std::vector<winrt::com_ptr<ID2D1Bitmap>> FrameList;
};

