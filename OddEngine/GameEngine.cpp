#include "pch.h"
#include "GameEngine.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;

GameEngine::GameEngine(winrt::hstring GameSolutionPath)
{
	StorageFile x = StorageFile::GetFileFromPathAsync(GameSolutionPath).get();
	winrt::hstring z = FileIO::ReadTextAsync(x).get();
	JsonObject GameMetadata = JsonObject::Parse(z.c_str());

	//input bindings to events file
	//Assets library path
	//game entrypoint

	if (GameMetadata.HasKey(L"InputConfigPath"))
		InputConfigPath = GameMetadata.GetNamedString(L"InputConfigPath");
	else
		InputConfigPath = L"";

	if (GameMetadata.HasKey(L"AssetLibraryPath"))
		InputConfigPath = GameMetadata.GetNamedString(L"AssetLibraryPath");
	else
		InputConfigPath = L"";

	if (GameMetadata.HasKey(L"EntryPoint"))
		InputConfigPath = GameMetadata.GetNamedString(L"EntryPoint");
	else
		InputConfigPath = L"";


}