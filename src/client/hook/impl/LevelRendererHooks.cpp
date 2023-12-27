#include "pch.h"
#include "LevelRendererHooks.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLevelEvent.h"
#include "client/render/Renderer.h"
#include "client/Latite.h"

namespace {
	std::shared_ptr<Hook> renderLevelHook;
}

void* LevelRendererHooks::LevelRenderer_renderLevel(SDK::LevelRenderer* lvl, SDK::ScreenContext* scn, void* unk) {
	SDK::ScreenContext::instance3d = scn;
	auto res = renderLevelHook->oFunc<decltype(&LevelRenderer_renderLevel)>()(lvl, scn, unk);
	{
		RenderLevelEvent ev{ lvl, scn };
		Eventing::get().dispatch(ev);
	}
	return res;
}

LevelRendererHooks::LevelRendererHooks() {
	renderLevelHook = addHook(Signatures::LevelRenderer_renderLevel.result, LevelRenderer_renderLevel,
		"LevelRenderer::renderLevel");
}
