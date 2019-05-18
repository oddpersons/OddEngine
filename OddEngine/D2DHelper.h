#pragma once

#include "pch.h"

winrt::com_ptr<ID2D1Factory1> DllExport CreateFactory();

winrt::com_ptr<ID3D11Device> DllExport CreateDevice();

winrt::com_ptr<ID2D1DeviceContext> DllExport CreateRenderTarget(
	winrt::com_ptr<ID2D1Factory1> const&,
	winrt::com_ptr<ID3D11Device> const&);

winrt::com_ptr<IDXGIFactory2> DllExport GetDxgiFactory(winrt::com_ptr<ID3D11Device> const&);

void DllExport CreateDeviceSwapChainBitmap(
	winrt::com_ptr<IDXGISwapChain1> const&,
	winrt::com_ptr<ID2D1DeviceContext> const&);

winrt::com_ptr<IDXGISwapChain1> DllExport CreateSwapChainForCoreWindow(winrt::com_ptr<ID3D11Device> const&);