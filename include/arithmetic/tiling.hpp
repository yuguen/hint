#ifndef TILING_HPP
#define TILING_HPP

#include <type_traits>

using namespace std;

namespace hint {
	template <typename T0, typename T1, typename T2, typename T3, typename T4>
	struct Tile;

	template <
	        unsigned int WX_,
	        unsigned int WY_,
	        unsigned int XOffset_,
	        unsigned int YOffset_,
	        bool IsSigned_>
	struct Tile<
	        integral_constant<unsigned int, WX_>,
	        integral_constant<unsigned int, WY_>,
	        integral_constant<unsigned int, XOffset_>,
	        integral_constant<unsigned int, YOffset_>,
	        integral_constant<bool, IsSigned_>>
	{
			static constexpr unsigned int WX = WX_;
			static constexpr unsigned int WY = WY_;
			static constexpr unsigned int XOffset = XOffset_;
			static constexpr unsigned int YOffset = YOffset_;
			static constexpr bool IsSigned = IsSigned_;
	};

	// Type holding a tile list
	template <class... TileList>
	struct TileSeq{};

	// Structures used to merge the tile sequences --------------------------------------
	template<class...>
	struct SeqConcat;

	template<class... TL1, class... TL2>
	struct SeqConcat<TileSeq<TL1...>, TileSeq<TL2...>>
	{
		using seq = TileSeq<TL1..., TL2...>;
	};

	template<class... TL1, class... TL2, class... TL3, class... TL4>
	struct SeqConcat<TileSeq<TL1...>, TileSeq<TL2...>, TileSeq<TL3...>, TileSeq<TL4...>>
	{
		using seq = TileSeq<TL1..., TL2..., TL3..., TL4...>;
	};

	//-----------------------------------------------------------------------------------

	// Factorise the dimension splitting computation ------------------------------------
	template<unsigned int MultDim, unsigned int TileDim, typename Enable = void>
	struct MultDimSplitter;

	template <
	        unsigned int MultDim,
	        unsigned int TileDim
	    >
	struct MultDimSplitter<MultDim, TileDim, typename enable_if<(MultDim >= TileDim)>::type>
	{
		    static constexpr unsigned int lowDim = (MultDim / TileDim) > 1 ?
			                                                (MultDim / TileDim) >> 1 :
			                                                                       1;
	};
	//--------------------------------------------------------------------------------------

	template <
	        typename WX,
	        typename WY,
	        typename XOffset,
	        typename YOffset,
	        typename IsSigned,
	        typename MaxTileWidth,
	        typename MaxTileHeight,
	        typename Enable = void
	>
	struct Tiling_;

	// Basic tile
	template <
	        typename WX,
	        typename WY,
	        typename XOffset,
	        typename YOffset,
	        typename MaxTileWidth,
	        typename MaxTileHeight,
	        bool value
	>
	struct Tiling_<
	        WX,
	        WY,
	        XOffset,
	        YOffset,
	        integral_constant<bool, value>,
	        MaxTileWidth,
	        MaxTileHeight,
	        typename enable_if<(WX::value <= MaxTileWidth::value) and (WY::value <= MaxTileHeight::value)>::type>
	{
		using tiles = TileSeq<Tile<WX, WY, XOffset, YOffset, integral_constant<bool, value> > >;
	};

	//Multi sub-columns and sub-rows
	template <
	        typename WX,
	        typename WY,
	        typename XOffset,
	        typename YOffset,
	        typename MaxTileWidth,
	        typename MaxTileHeight,
	        bool value
	>
	struct Tiling_<
	        WX,
	        WY,
	        XOffset,
	        YOffset,
	        integral_constant<bool, value>,
	        MaxTileWidth,
	        MaxTileHeight,
	        typename enable_if<(WX::value > MaxTileWidth::value) and (WY::value > MaxTileHeight::value)>::type>
	{
		static constexpr unsigned int max_tile_width = MaxTileWidth::value;
		static constexpr unsigned int max_tile_height = MaxTileHeight::value;
		static constexpr unsigned int wx = WX::value;
		static constexpr unsigned int wy = WY::value;
		static constexpr unsigned int x_offset = XOffset::value;
		static constexpr unsigned int y_offset = YOffset::value;

		static constexpr unsigned int east_dim = max_tile_width * MultDimSplitter<wx, max_tile_width>::lowDim;
		static constexpr unsigned int north_dim = max_tile_height * MultDimSplitter<wy, max_tile_height>::lowDim;

		using tiles = typename SeqConcat<
		    // North east
		    typename Tiling_<
		        integral_constant<unsigned int, east_dim>,
		        integral_constant<unsigned int, north_dim>,
		        XOffset,
		        YOffset,
		        false_type,
		        MaxTileWidth,
		        MaxTileHeight>::tiles,

		    // Northwest
		    typename Tiling_<
		        integral_constant<unsigned int, wx - east_dim>,
		        integral_constant<unsigned int, north_dim>,
		        integral_constant<unsigned int, x_offset + east_dim>,
		        YOffset,
		        integral_constant<bool, value>,
		        MaxTileWidth,
		        MaxTileHeight>::tiles,

		    // Southeast
		    typename Tiling_<
		        integral_constant<unsigned int, east_dim>,
		        integral_constant<unsigned int, wy - north_dim>,
		        XOffset,
		        integral_constant<unsigned int, y_offset + north_dim>,
		        integral_constant<bool, value>,
		        MaxTileWidth,
		        MaxTileHeight
		    >::tiles,

		    // Southwest
		    typename Tiling_<
		        integral_constant<unsigned int, wx - east_dim>,
		        integral_constant<unsigned int, wy - north_dim>,
		        integral_constant<unsigned int, x_offset + east_dim>,
		        integral_constant<unsigned int, y_offset + north_dim>,
		        integral_constant<bool, value>,
		        MaxTileWidth,
		        MaxTileHeight
		    >::tiles
		>::seq;
	};

	//Only one column but multiple rows
	template <
	        typename WX,
	        typename WY,
	        typename XOffset,
	        typename YOffset,
	        typename MaxTileWidth,
	        typename MaxTileHeight,
	        bool value
	>
	struct Tiling_<
	        WX,
	        WY,
	        XOffset,
	        YOffset,
	        integral_constant<bool, value>,
	        MaxTileWidth,
	        MaxTileHeight,
	        typename enable_if<(WX::value <= MaxTileWidth::value) and (WY::value > MaxTileHeight::value)>::type>
	{
		static constexpr unsigned int max_tile_height = MaxTileHeight::value;
		static constexpr unsigned int wy = WY::value;
		static constexpr unsigned int y_offset = YOffset::value;

		static constexpr unsigned int north_dim = max_tile_height * MultDimSplitter<wy, max_tile_height>::lowDim;

		using tiles = typename SeqConcat<
		    // North
		    typename Tiling_<
		        WX,
		        integral_constant<unsigned int, north_dim>,
		        XOffset,
		        YOffset,
		        integral_constant<bool, value>,
		        MaxTileWidth,
		        MaxTileHeight>::tiles,

		    // South
		    typename Tiling_<
		        WX,
		        integral_constant<unsigned int, wy - north_dim>,
		        XOffset,
		        integral_constant<unsigned int, y_offset + north_dim>,
		        integral_constant<bool, value>,
		        MaxTileWidth,
		        MaxTileHeight
		    >::tiles
		>::seq;
	};

	//Multi sub-columns, one row
	template <
	        typename WX,
	        typename WY,
	        typename XOffset,
	        typename YOffset,
	        typename MaxTileWidth,
	        typename MaxTileHeight,
	        bool value
	>
	struct Tiling_<
	        WX,
	        WY,
	        XOffset,
	        YOffset,
	        integral_constant<bool, value>,
	        MaxTileWidth,
	        MaxTileHeight,
	        typename enable_if<(WX::value > MaxTileWidth::value) and (WY::value <= MaxTileHeight::value)>::type>
	{
		static constexpr unsigned int max_tile_width = MaxTileWidth::value;
		static constexpr unsigned int wx = WX::value;
		static constexpr unsigned int x_offset = XOffset::value;

		static constexpr unsigned int east_dim = max_tile_width * MultDimSplitter<wx, max_tile_width>::lowDim;

		using tiles = typename SeqConcat<
		    // east
		    typename Tiling_<
		        integral_constant<unsigned int, east_dim>,
		        WY,
		        XOffset,
		        YOffset,
		        false_type,
		        MaxTileWidth,
		        MaxTileHeight>::tiles,

		    // West
		    typename Tiling_<
		        integral_constant<unsigned int, wx - east_dim>,
		        WY,
		        integral_constant<unsigned int, x_offset + east_dim>,
		        YOffset,
		        integral_constant<bool, value>,
		        MaxTileWidth,
		        MaxTileHeight>::tiles
		>::seq;
	};
}
#endif // TILING_HPP
