#ifndef TILED_PRODUCT_HPP
#define TILED_PRODUCT_HPP

#include <type_traits>

#include "arithmetic/tiling.hpp"

using namespace std;

namespace hint {

	template <
			unsigned int WX,
			unsigned int WY,
			unsigned int MaxTileWidth,
			unsigned int MaxTileHeight,
			bool IsSigned
		>
	using Tiling = Tiling_<
		integral_constant<unsigned int, WX>,
		integral_constant<unsigned int, WY>,
		integral_constant<unsigned int, 0>,
		integral_constant<unsigned int, 0>,
		integral_constant<bool, IsSigned>,
		integral_constant<unsigned int, MaxTileWidth>,
		integral_constant<unsigned int, MaxTileHeight>
	>;
}

#endif // TILED_PRODUCT_HPP
