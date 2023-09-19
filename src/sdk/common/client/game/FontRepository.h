#pragma once
#include "sdk/Util.h"

namespace SDK {
	class FontRepository {
	public:
		MVCLASS_FIELD(std::vector<std::shared_ptr<class Font>>, fontList, 0x20, 0x28, 0x20);
	
		class Font* getSmoothFont() {
			return fontList[7].get();
		}
	};
}