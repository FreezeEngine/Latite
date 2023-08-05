#pragma once
#include "util/DXUtil.h"
#include <vector>
#include <shared_mutex>

class Renderer final {
public:
	Renderer() = default;
	Renderer(Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	~Renderer();

	bool init(IDXGISwapChain* chain);
	bool hasInitialized() { return hasInit; };
	HRESULT reinit();
	std::shared_lock<std::shared_mutex> lock();
	void render();
private:
	std::wstring fontFamily = L"Segoe UI";
	void releaseAllResources();

	bool hasInit = false;
	bool firstInit = false;
	bool reqCommandQueue = false;

	IDXGISwapChain* gameSwapChain;
	IDXGISwapChain4* swapChain4;

	ComPtr<IDXGIDevice> dxgiDevice;

	ComPtr<ID3D12Device> gameDevice12;
	ComPtr<ID3D11Device> gameDevice11;
	ID3D11Device* d3dDevice;
	ID3D12CommandQueue* commandQueue;

	ComPtr<ID3D11DeviceContext> d3dCtx;
	ComPtr<ID3D11On12Device> d3d11On12Device;

	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device> d2dDevice;
	ComPtr<ID2D1DeviceContext> d2dCtx;
	ComPtr<IDWriteFactory> dWriteFactory;
	ComPtr<IWICImagingFactory2> wicFactory;

	ComPtr<ID2D1SolidColorBrush> solidBrush;
	ComPtr<ID2D1Effect> shadowEffect;
	ComPtr<ID2D1Effect> affineTransformEffect;
	ComPtr<ID2D1Effect> blurEffect;

	ComPtr<IDWriteTextFormat> font;
	ComPtr<IDWriteTextFormat> fontSemilight;
	ComPtr<IDWriteTextFormat> fontLight;

	std::vector<ID3D12Resource*> d3d12Targets = {  };
	std::vector<ID3D11Resource*> d3d11Targets = {};
	std::vector<ID2D1Bitmap1*> renderTargets = {};

	std::vector<ID2D1Bitmap1*> blurBuffers = {};

	std::shared_mutex mutex;
	int bufferCount = 3;
public:
	ID2D1Bitmap1* testBitmap;

	enum class FontSelection {
		Regular,
		Semilight,
		Light
	};

	void createDeviceIndependentResources();
	void createDeviceDependentResources();

	void releaseDeviceIndependentResources();
	void releaseDeviceResources();

	void setDevice11(ID3D11Device* dev) {
		gameDevice11 = dev;
	}

	void setDevice12(ID3D12Device* dev) {
		gameDevice12 = dev;
	}

	void setCommandQueue(ID3D12CommandQueue* queue) noexcept {
		commandQueue = queue;
	}

	void setSwapChain(IDXGISwapChain* chain) noexcept {
		gameSwapChain = chain;
	}

	[[nodiscard]] std::wstring getFontFamily() {
		return this->fontFamily;
	}

	void setFontFamily(std::wstring ws) {
		this->fontFamily = ws;
	}

	D2D1_SIZE_F getScreenSize() {
		return { (float)d2dCtx->GetPixelSize().width, (float)d2dCtx->GetPixelSize().height };
	}

	[[nodiscard]] IDWriteTextFormat* getTextFormat(FontSelection selection) {
		switch (selection) {
		case FontSelection::Regular:
			return font.Get();
		case FontSelection::Semilight:
			return fontSemilight.Get();
		case FontSelection::Light:
			return fontLight.Get();
		default:
			return nullptr;
		}
	}

	[[nodiscard]] ID2D1DeviceContext* getDeviceContext() {
		return d2dCtx.Get();
	}

	[[nodiscard]] ID2D1Device* getDevice() {
		return d2dDevice.Get();
	}

	[[nodiscard]] ID2D1SolidColorBrush* getSolidBrush() {
		return solidBrush.Get();
	}

	[[nodiscard]] ID2D1Factory3* getFactory() {
		return d2dFactory.Get();
	}

	[[nodiscard]] IDWriteFactory* getDWriteFactory() {
		return dWriteFactory.Get();
	}

	[[nodiscard]] ID2D1Bitmap1* copyCurrentBitmap() {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		ID2D1Bitmap1* newBitmap;

		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

		HRESULT hr = d2dCtx->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,pixelFormat), &newBitmap);
		if (SUCCEEDED(hr)) {
			newBitmap->CopyFromBitmap(nullptr, myBitmap, nullptr);
		}
		return newBitmap;
	}

	[[nodiscard]] ID2D1Bitmap1* copyCurrentBitmap(d2d::Rect const& rc) {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		ID2D1Bitmap1* newBitmap;

		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

		HRESULT hr = d2dCtx->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), &newBitmap);
		if (SUCCEEDED(hr)) {
			auto pt = D2D1::Point2U((UINT32)rc.left, (UINT32)rc.top);
			auto urc = D2D1::RectU((UINT32)rc.left, (UINT32)rc.top, (UINT32)rc.right, (UINT32)rc.bottom);
			newBitmap->CopyFromBitmap(&pt, myBitmap, &urc);
		}
		return newBitmap;
	}

	[[nodiscard]] void copyCurrentBitmap(ID2D1Bitmap1*& bmp) {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		bmp->CopyFromBitmap(nullptr, myBitmap, nullptr);
	}

	[[nodiscard]] ID2D1Effect*& getShadowEffect() {
		return *shadowEffect.GetAddressOf();
	}

	[[nodiscard]] ID2D1Effect*& getAffineTransformEffect() {
		return *affineTransformEffect.GetAddressOf();
	}

	[[nodiscard]] ID2D1Effect*& getBlurEffect() {
		return *blurEffect.GetAddressOf();
	}

	[[nodiscard]] ID2D1Bitmap1* getBitmap() {
		auto buf = swapChain4->GetCurrentBackBufferIndex();
		return this->renderTargets[buf];
	}

	[[nodiscard]] ID2D1Bitmap1* getBlurBitmap() {
		return blurBuffers[swapChain4->GetCurrentBackBufferIndex()];
	}

	[[nodiscard]] IWICImagingFactory2* getImagingFactory() {
		return this->wicFactory.Get();
	}

};