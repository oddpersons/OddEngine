#include "pch.h"
#include "SpriteConfiguration.h"

using namespace winrt;

using namespace Windows;
using namespace Windows::Graphics::Display;
using namespace Windows::Data::Json;

SpriteConfiguration::SpriteConfiguration(std::wstring JSONConfig, IWICImagingFactory* pFactory)
{
	HRESULT hr;
	UINT cFrames = 0;
	com_ptr<IWICStream> pStream;
	com_ptr<IWICBitmapDecoder> pDecoder;

	//Parse JSON
	JsonObject SpriteMetadata = JsonObject::Parse(JSONConfig.c_str());
	if (SpriteMetadata.HasKey(L"Path"))
	{
		Path = SpriteMetadata.GetNamedString(L"Path");
	}
	else
	{
		throw new std::invalid_argument("Path not defined");
	}

	if (SpriteMetadata.HasKey(L"PerRow"))
	{
		SpritesPerRow = SpriteMetadata.GetNamedNumber(L"PerRow");
	}
	else
	{
		throw new std::invalid_argument("PerRow not defined");
	}

	if (SpriteMetadata.HasKey(L"Height"))
	{
		SpriteHeight = SpriteMetadata.GetNamedNumber(L"Height");
	}
	else
	{
		throw new std::invalid_argument("Height not defined");
	}

	if (SpriteMetadata.HasKey(L"UnusedSprites"))
	{
		UnusedSprites = SpriteMetadata.GetNamedNumber(L"Height");
	}

	//Process image
	hr = pFactory->CreateStream(pStream.put());
	winrt::check_hresult(hr);

	hr = pStream->InitializeFromFilename(Path.c_str(), GENERIC_READ);
	winrt::check_hresult(hr);

	hr = pFactory->CreateDecoder(
		GUID_ContainerFormatPng,
		NULL,
		pDecoder.put());
	winrt::check_hresult(hr);

	hr = pDecoder->Initialize(
		pStream.get(),
		WICDecodeMetadataCacheOnDemand);
	winrt::check_hresult(hr);
	
	hr = pDecoder->GetFrameCount(&cFrames);
	winrt::check_hresult(hr);

	if (cFrames == 0)
	{
		throw "No frames presnet in bitmap";
	}

	hr = pDecoder->GetFrame(0, pBitmapFrame.put());
	winrt::check_hresult(hr);

	

	pFactory->CreateFormatConverter(converter.put());
	winrt::check_hresult(hr);

	hr = converter->Initialize(pBitmapFrame.get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeCustom);
	winrt::check_hresult(hr);

	//Set some metadata for the class
	UINT x, y;
	hr = pBitmapFrame->GetSize(&x, &y);
	winrt::check_hresult(hr);

	ParentBitmapX = x;
	ParentBitmapY = y;

	if (SpriteHeight == 0)
		SpriteHeight = y;

	SpritesPerColumn = y / SpriteHeight;
	SpriteWidth = x / SpritesPerRow;
	SpriteCount = (SpritesPerRow * SpritesPerColumn) - UnusedSprites;

	FrameList.resize(SpriteCount);

	return;
}


SpriteConfiguration::~SpriteConfiguration()
{
}

void SpriteConfiguration::CreateDeviceDependentBitmap(ID2D1DeviceContext* DeviceContext)
{
	HRESULT hr;
	D2D1_SIZE_U SpriteSize;
	SpriteSize.height = SpriteHeight;
	SpriteSize.width = SpriteWidth;

	WICRect wrect;
	wrect.Height = SpriteHeight;
	wrect.Width = SpriteWidth;
	wrect.X = 0;
	wrect.Y = 0;

	double DPIX, DPIY;
	hr = converter->GetResolution(&DPIX, &DPIY);
	winrt::check_hresult(hr);

	UINT SpriteByteCount = SpriteWidth * SpriteHeight * 32;
	
	std::unique_ptr<BYTE[]> buffer{ new BYTE[SpriteByteCount] };

	D2D1_BITMAP_PROPERTIES bmpprop;
	
	winrt::com_ptr<ID2D1Bitmap> DevDependentBitmap2;
	hr = DeviceContext->CreateBitmapFromWicBitmap(converter.get(), NULL, DevDependentBitmap2.put());
	winrt::check_hresult(hr);

	D2D1_POINT_2U StartPoint;
	StartPoint.x = 0;
	StartPoint.y = 0;

	
	bmpprop.pixelFormat = DevDependentBitmap2->GetPixelFormat();
	bmpprop.dpiX = DPIX;
	bmpprop.dpiY = DPIY;


	for (int i = 0; i < SpriteCount; i++) 
	{
		int CurrentFrameRow = i % SpritesPerRow;
		int CurrentFrameHeight = i / SpritesPerRow;
		
		D2D1_RECT_U CopyRect;
		CopyRect.top = CurrentFrameHeight * SpriteHeight;
		CopyRect.left = CurrentFrameRow * SpriteWidth;
		CopyRect.right = SpriteWidth + CopyRect.left;
		CopyRect.bottom = SpriteHeight + CopyRect.top;

		winrt::com_ptr<ID2D1Bitmap> DevDependentBitmap;
		
		DeviceContext->CreateBitmap(SpriteSize, bmpprop, DevDependentBitmap.put());
		DevDependentBitmap->CopyFromBitmap(&StartPoint, DevDependentBitmap2.get(), &CopyRect);
		winrt::check_hresult(hr);

		FrameList[i] = DevDependentBitmap;
	}
}

ID2D1Bitmap* SpriteConfiguration::GetBitmap()
{
	return FrameList[0].get();
}

ID2D1Bitmap* SpriteConfiguration::NextSprite() 
{
	CurrentSprite++;
	CurrentSprite %= SpriteCount;

	return FrameList[CurrentSprite].get();
}