#include "pch.h"
#include "D2DHelper.h"

winrt::com_ptr<ID2D1Factory1> CreateFactory()
{
	D2D1_FACTORY_OPTIONS options{};

#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	winrt::com_ptr<ID2D1Factory1> factory;

	winrt::check_hresult(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options,
		factory.put()));

	return factory;
}

HRESULT CreateDevice(D3D_DRIVER_TYPE const type, winrt::com_ptr<ID3D11Device>& device)
{
	WINRT_ASSERT(!device);

	return D3D11CreateDevice(
		nullptr,
		type,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr, 0,
		D3D11_SDK_VERSION,
		device.put(),
		nullptr,
		nullptr);
}

winrt::com_ptr<ID3D11Device> CreateDevice()
{
	winrt::com_ptr<ID3D11Device> device;
	HRESULT hr{ CreateDevice(D3D_DRIVER_TYPE_HARDWARE, device) };

	if (DXGI_ERROR_UNSUPPORTED == hr)
	{
		hr = CreateDevice(D3D_DRIVER_TYPE_WARP, device);
	}

	winrt::check_hresult(hr);
	return device;
}

winrt::com_ptr<ID2D1DeviceContext> CreateRenderTarget(
	winrt::com_ptr<ID2D1Factory1> const& factory,
	winrt::com_ptr<ID3D11Device> const& device)
{
	WINRT_ASSERT(factory);
	WINRT_ASSERT(device);

	winrt::com_ptr<IDXGIDevice> const dxdevice{ device.as<IDXGIDevice>() };

	winrt::com_ptr<ID2D1Device> d2device;
	winrt::check_hresult(factory->CreateDevice(dxdevice.get(), d2device.put()));

	winrt::com_ptr<ID2D1DeviceContext> target;
	winrt::check_hresult(d2device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, target.put()));
	return target;
}

winrt::com_ptr<IDXGIFactory2> GetDxgiFactory(winrt::com_ptr<ID3D11Device> const& device)
{
	WINRT_ASSERT(device);

	winrt::com_ptr<IDXGIDevice> const dxdevice{ device.as<IDXGIDevice>() };

	winrt::com_ptr<IDXGIAdapter> adapter;
	winrt::check_hresult(dxdevice->GetAdapter(adapter.put()));

	winrt::com_ptr<IDXGIFactory2> factory;
	winrt::check_hresult(adapter->GetParent(__uuidof(factory), factory.put_void()));
	return factory;
}

void CreateDeviceSwapChainBitmap(
	winrt::com_ptr<IDXGISwapChain1> const& swapchain,
	winrt::com_ptr<ID2D1DeviceContext> const& target)
{
	WINRT_ASSERT(swapchain);
	WINRT_ASSERT(target);

	winrt::com_ptr<IDXGISurface> surface;
	winrt::check_hresult(swapchain->GetBuffer(0, __uuidof(surface), surface.put_void()));

	D2D1_BITMAP_PROPERTIES1 const props{ D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)) };

	winrt::com_ptr<ID2D1Bitmap1> bitmap;

	winrt::check_hresult(target->CreateBitmapFromDxgiSurface(surface.get(),
		props,
		bitmap.put()));

	target->SetTarget(bitmap.get());
}

winrt::com_ptr<IDXGISwapChain1> CreateSwapChainForCoreWindow(winrt::com_ptr<ID3D11Device> const& device)
{
	WINRT_ASSERT(device);

	winrt::com_ptr<IDXGIFactory2> const factory{ GetDxgiFactory(device) };

	DXGI_SWAP_CHAIN_DESC1 props{};
	props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	props.SampleDesc.Count = 1;
	props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	props.BufferCount = 2;
	props.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	winrt::com_ptr<IDXGISwapChain1> swapChain;

	winrt::check_hresult(factory->CreateSwapChainForCoreWindow(
		device.get(),
		winrt::get_unknown(winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread()),
		&props,
		nullptr, // all or nothing
		swapChain.put()));

	return swapChain;
}