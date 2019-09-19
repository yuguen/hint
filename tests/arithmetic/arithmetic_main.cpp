#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ArithmeticTests

#include <iostream>
#include <utility>

#include <boost/test/unit_test.hpp>

#include "arithmetic/tiled_product.hpp"

using namespace std;


template <typename Tile>
inline void print_tile()
{
	cout << "Tile of size (" << Tile::WX << ", " << Tile::WY << ") at position (" <<
			Tile::XOffset << ", " << Tile::YOffset <<") -- ";
	if (not Tile::IsSigned)
		cout << "not ";
	cout << "signed." << endl;
}

inline void print_seq() {};

template<typename H, typename... T>
inline void print_seq(H &&, T&&...)
{
	print_tile<H>();
	print_seq(T{}...);
}

template<class Head, class... Tail>
void print_tile_seq(hint::TileSeq<Head, Tail...>)
{
	print_seq(Head{}, Tail{}... );
}


BOOST_AUTO_TEST_CASE(tiling_test)
{
	typename hint::Tiling<16,16,4,4,false>::tiles t;
	print_tile_seq(t);
};
