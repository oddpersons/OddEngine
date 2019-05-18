#include "pch.h"
#include "App.h"
#include "..\OddEngine\dllmain.h"

using namespace winrt;

using namespace Windows;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;


constexpr D2D1_COLOR_F color_white{ 1.0f,  1.0f,  1.0f,  1.0f };
constexpr D2D1_COLOR_F color_orange{ 0.92f,  0.38f,  0.208f,  1.0f };


struct App : implements<App, IFrameworkViewSource, IFrameworkView>
{
	winrt::com_ptr<ID2D1Factory1> m_factory;
	winrt::com_ptr<ID2D1DeviceContext> m_target;
	winrt::com_ptr<IDXGISwapChain1> m_swapChain;
	winrt::com_ptr<ID2D1SolidColorBrush> m_brush;
	winrt::com_ptr<IDWriteFactory> m_writeFactory;
	winrt::com_ptr<IDWriteTextFormat> m_systemTextFormat;
	double LastFPS = 0;
	float m_dpi{};

	std::unique_ptr<SpriteConfiguration> SpriteLib;

	IFrameworkView CreateView()
	{
		return *this;
	}

	void Initialize(CoreApplicationView const&)
	{
		testclass x;
		x.x = 5;

		
	}

	void Load(hstring const&)
	{
		CoreWindow const window{ CoreWindow::GetForCurrentThread() };

		window.SizeChanged([&](auto && ...)
			{
				if (m_target)
				{
					ResizeSwapChainBitmap();
					Render();
				}
			});

		DisplayInformation const display{ DisplayInformation::GetForCurrentView() };
		m_dpi = display.LogicalDpi();

		display.DpiChanged([&](DisplayInformation const& display, IInspectable const&)
			{
				if (m_target)
				{
					m_dpi = display.LogicalDpi();
					m_target->SetDpi(m_dpi, m_dpi);
					CreateDeviceSizeResources();
					Render();
				}
			});

		window.KeyDown([&](CoreWindow const& win, KeyEventArgs const& handler)
			{
				Render();
			});
		

		m_factory = CreateFactory();
		CreateDeviceIndependentResources();
	}

	void Uninitialize()
	{
	}

	void Run()
	{
		CoreWindow const window{ CoreWindow::GetForCurrentThread() };
		window.Activate();

		Render();
		CoreDispatcher const dispatcher{ window.Dispatcher() };
		dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
	}

	void SetWindow(CoreWindow const&) {}

	void Draw()
	{
		m_target->Clear(color_white);
		
		D2D1_SIZE_F const size{ m_target->GetSize() };
		D2D1_RECT_F const rect{ 100.0f, 100.0f, size.width - 100.0f, size.height - 100.0f };
		D2D1_RECT_F const textRect{200, 200, 400, 400};
		m_target->DrawRectangle(rect, m_brush.get(), 100.0f);

		ID2D1Bitmap* ptr = SpriteLib->NextSprite();
		m_target->DrawBitmap(
			ptr
		);
		
		char buffer[1024];
		WCHAR wbuffer[1024];
		swprintf(wbuffer, 1024, L"%.8f", LastFPS);


		std::wstring x = wbuffer;
		
	
		m_target->DrawText(
			wbuffer, 
			wcslen(wbuffer),
			m_systemTextFormat.get(),
			&textRect,
			m_brush.get()
		);

		(void)snprintf(buffer, sizeof(buffer), "Draw %.2f x %.2f @ %.2f\n", size.width, size.height, m_dpi);
		OutputDebugStringA(buffer);
		
	}

	void Render()
	{
		if (!m_target)
		{
			winrt::com_ptr<ID3D11Device> const device{ CreateDevice() };
			m_target = CreateRenderTarget(m_factory, device);
			m_swapChain = CreateSwapChainForCoreWindow(device);

			CreateDeviceSwapChainBitmap(m_swapChain, m_target);

			m_target->SetDpi(m_dpi, m_dpi);
			
			CreateDeviceResources();
			CreateDeviceSizeResources();
		}

		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		m_target->BeginDraw();
		Draw();
		m_target->EndDraw();
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		LastFPS = 1 / time_span.count();
		HRESULT const hr{ m_swapChain->Present(1, 0) };

		if (S_OK != hr && DXGI_STATUS_OCCLUDED != hr)
		{
			ReleaseDevice();
		}
	}

	void ReleaseDevice()
	{
		m_target = nullptr;
		m_swapChain = nullptr;

		ReleaseDeviceResources();
	}

	void ResizeSwapChainBitmap()
	{
		WINRT_ASSERT(m_target);
		WINRT_ASSERT(m_swapChain);

		m_target->SetTarget(nullptr);

		if (S_OK == m_swapChain->ResizeBuffers(0, // all buffers
			0, 0, // client area
			DXGI_FORMAT_UNKNOWN, // preserve format
			0)) // flags
		{
			CreateDeviceSwapChainBitmap(m_swapChain, m_target);
			CreateDeviceSizeResources();
		}
		else
		{
			ReleaseDevice();
		}
	}

	void CreateDeviceIndependentResources()
	{

		com_ptr<IWICImagingFactory> pFactory;
		

		winrt::hresult hr;

		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			//IID_PPV_ARGS(&pFactory)
			__uuidof(IWICImagingFactory),
			(void**)pFactory.put()
		);

		winrt::check_hresult(hr);

		std::wstring path{ L"{ \
			\"Path\" : \"Assets\\\\test.png\", \
			\"PerRow\" : 2, \
			\"Height\" : 50 \
			}"
		};
		SpriteLib = std::make_unique<SpriteConfiguration>(
			path,
			pFactory.get()
		);

		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown * *>(m_writeFactory.put())
		);
		winrt::check_hresult(hr);

		m_writeFactory->CreateTextFormat(
			L"Lucida Console",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			12.0f,
			L"en-us",
			m_systemTextFormat.put()
		);
	}

	void CreateDeviceResources()
	{
		winrt::check_hresult(m_target->CreateSolidColorBrush(
			color_orange,
			D2D1::BrushProperties(0.8f),
			m_brush.put()));

		SpriteLib->CreateDeviceDependentBitmap(m_target.get());
	}

	void CreateDeviceSizeResources()
	{
	}

	void ReleaseDeviceResources()
	{
		m_brush = nullptr;
	}
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	CoreApplication::Run(winrt::make<App>());
}