#pragma once

#include "pch.h"
#include "SpriteConfiguration.h"
#include "D2DHelper.h"
#include "Enums.h"

using namespace winrt;
using namespace winrt::Windows;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::FileProperties;

class DllExport GameEngine {
public:
	int test;
	GameEngine(winrt::hstring); 
private:
	winrt::hstring InputConfigPath;
	winrt::hstring AssetLibraryPath;
	winrt::hstring EntryPoint;
};