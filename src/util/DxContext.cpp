#include "DxContext.h"
#include "client/Latite.h"
#include "Util.h"

D2D1_RECT_F DXContext::getRect(RectF const& rc)  {
	return D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom);
}

void DXContext::fillRectangle(RectF const& rect, d2d::Color const& color)  {
	this->brush->SetColor(color.get());
	ctx->FillRectangle(getRect(rect), brush);
}

void DXContext::drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness)  {
	this->brush->SetColor(color.get());
	ctx->DrawRectangle(getRect(rect), brush, lineThickness);
}

void DXContext::fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius)  {
	this->brush->SetColor(color.get());
	auto rounded = D2D1::RoundedRect(getRect(rect), radius, radius);
	ctx->FillRoundedRectangle(rounded, brush);
}

void DXContext::drawRoundedRectangle(RectF irect, d2d::Color const& color, float radius, float lineThickness, OutlinePosition outPos)  {
	RectF rect = irect;
	this->brush->SetColor(color.get());
	switch (outPos) {
	case OutlinePosition::Inside:
		rect.left += (lineThickness / 2.f);
		rect.right -= (lineThickness / 2.f);
		rect.top += (lineThickness / 2.f);
		rect.bottom -= (lineThickness / 2.f);
		break;
	case OutlinePosition::Outside:
		rect.left -= lineThickness / 2.f;
		rect.right += lineThickness / 2.f;
		rect.top -= lineThickness / 2.f;
		rect.bottom += lineThickness / 2.f;
		radius += lineThickness / 2.f;
		break;
	default:
		break;
	}
	auto rc = getRect(rect);
	auto rounded = D2D1::RoundedRect(rc, radius, radius);

	ctx->DrawRoundedRectangle(rounded, brush, lineThickness);
}

void DXContext::drawGaussianBlur(float intensity)  {
	ID2D1Effect* gaussianBlurEffect = Latite::getRenderer().getBlurEffect();
	
	// maybe we might not need to flush if we dont draw anything before clickgui?
	ctx->Flush();
	auto bitmap = Latite::getRenderer().copyCurrentBitmap();
	gaussianBlurEffect->SetInput(0, bitmap);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);
	auto sz = ctx->GetSize();
	auto rc = D2D1::RectF(0, 0, sz.width, sz.height);
	D2D1::Matrix3x2F oMat;
	//ctx->SetTransform(D2D1::Matrix3x2F::Scale(sz.width / rc.right, sz.height / rc.bottom));
	ctx->DrawImage(gaussianBlurEffect, D2D1::Point2F(0.f, 0.f), rc);
	bitmap->Release();
}

void DXContext::drawGaussianBlur(ID2D1Bitmap1* bmp, float intensity) {
	ID2D1Effect* gaussianBlurEffect = Latite::getRenderer().getBlurEffect();

	ctx->Flush();
	Latite::getRenderer().copyCurrentBitmap(bmp);
	gaussianBlurEffect->SetInput(0, bmp);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);
	auto sz = ctx->GetSize();
	auto rc = D2D1::RectF(0, 0, sz.width, sz.height);
	D2D1::Matrix3x2F oMat;
	//ctx->SetTransform(D2D1::Matrix3x2F::Scale(sz.width / rc.right, sz.height / rc.bottom));
	ctx->DrawImage(gaussianBlurEffect, D2D1::Point2F(0.f, 0.f), rc);
	bmp->Release();
}

void DXContext::setFont(Renderer::FontSelection font)  {
	this->currentFormat = Latite::getRenderer().getTextFormat(font);
}

void DXContext::drawText(RectF const& rc, std::string const& text, d2d::Color const& color, float size, DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT verticalAlignment)  {
	brush->SetColor(color.get());
	auto ws = util::StrToWStr(text);
	IDWriteTextLayout* layout;
	if (SUCCEEDED(this->factory->CreateTextLayout(ws.c_str(), static_cast<uint32_t>(ws.size()),
		currentFormat, rc.getWidth(), rc.getHeight(),
		&layout))) {
		DWRITE_TEXT_RANGE range;
			range.startPosition = 0;
			layout->SetFontSize(size, range);
			layout->SetTextAlignment(alignment);
			layout->SetParagraphAlignment(verticalAlignment);
			this->ctx->DrawTextLayout({ rc.getPos().x, rc.getPos().y }, layout,
				brush);
			SafeRelease(&layout);
	}
}

DXContext::DXContext() : brush(Latite::getRenderer().getSolidBrush()), ctx(Latite::getRenderer().getDeviceContext()), factory(Latite::getRenderer().getDWriteFactory())  {
}
