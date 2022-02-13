#include <iostream>
#include <vector>

#ifndef SFML_STATIC
#define SFML_STATIC
#endif
#ifdef _DEBUG
#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-audio-d.lib")
#pragma comment(lib, "sfml-network-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#else
#pragma comment(lib, "sfml-graphics-s.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "sfml-window-s.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "sfml-audio-s.lib")
#pragma comment(lib, "flac.lib")
#pragma comment(lib, "vorbisenc.lib")
#pragma comment(lib, "vorbisfile.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "openal32.lib")
#pragma comment(lib, "sfml-system-s.lib")
#pragma comment(lib, "winmm.lib")
#endif
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <windows.h>
#include "resource.h"

#define _CE_BEGIN namespace chess_engine \
	{
#define _CE_END }
#define _CE chess_engine::
#define _CE_API

#define GET_FIGURE_TYPE(FIGURE) (((unsigned char)0x0F) & ((unsigned char)FIGURE))
#define GET_FIGURE_COLOR(FIGURE) (((unsigned char)0xF0) & ((unsigned char)FIGURE))

_CE_BEGIN
constexpr unsigned char field_cost[8][8] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 2, 2, 1, 0, 0,
	0, 0, 1, 2, 2, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
};
_CE_END

_CE_BEGIN
enum class FigureType : unsigned char
{
	Empty = 0x01,
	Pawn = 0x02,
	Rook = 0x03,
	Horse = 0x04,
	Elephant = 0x05,
	Queen = 0x06,
	King = 0x07
};

enum class FigureColor : unsigned char
{
	Empty = 0x10,
	White = 0x20,
	Black = 0x30
};
_CE_END

_CE_BEGIN
constexpr unsigned char figure_cost[] = 
{
	0,
	0, //EMPTY
	1, //PAWN
	3, //ELEPHANT
	3, //HORSE
	5, //ROOK
	10, //QUEEN
	255 //KING
};
_CE_END

_CE_BEGIN
struct PartySettings
{
	size_t max_best_moves = 3;
	size_t max_depth = 2;
	unsigned char chess_board_figures[8][8] = 
		{
		{0x23, 0x24, 0x25, 0x27, 0x26, 0x25, 0x24, 0x23},
		{0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22},
		{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
		{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
		{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
		{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
		{0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32},
		{0x33, 0x34, 0x35, 0x37, 0x36, 0x35, 0x34, 0x33},
		};
};
_CE_END

_CE_BEGIN
template <typename _Value_type>
struct Vector
{
	Vector(const _Value_type x = _Value_type(), const _Value_type y = _Value_type()) :
		x(x), y(y)
	{}
	_Value_type x;
	_Value_type y;
};

using VectorUC = Vector<unsigned char>;
_CE_END

_CE_BEGIN
inline bool __fastcall figure_can_move(const VectorUC figure_position)
{
	if (figure_position.x < (unsigned char)8 && figure_position.y < (unsigned char)8)
		return true;
	return false;
}
inline bool __fastcall figure_can_move(const unsigned char x, const unsigned char y)
{
	if (x < (unsigned char)8 && y < (unsigned char)8)
		return true;
	return false;
}
_CE_END

_CE_BEGIN
template <typename _Lambda>
inline void __fastcall iterate_elephant(VectorUC figure_position, _Lambda lambda)
{
	unsigned char x = figure_position.x + 1;
	unsigned char y = figure_position.y + 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x++;
		y++;
	}

	x = figure_position.x - 1;
	y = figure_position.y - 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x--;
		y--;
	}
	x = figure_position.x - 1;
	y = figure_position.y + 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x--;
		y++;
	}
	x = figure_position.x + 1;
	y = figure_position.y - 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x++;
		y--;
	}
}
_CE_END

_CE_BEGIN
template <typename _Lambda>
inline void __fastcall iterate_horse(VectorUC figure_position, _Lambda lambda)
{
	unsigned char x = figure_position.x;
	unsigned char y = figure_position.y;

	if (figure_can_move(figure_position.x + 2, figure_position.y + 1))
	{
		lambda(figure_position.x + 2, figure_position.y + 1);
	}
	if (figure_can_move(figure_position.x + 2, figure_position.y - 1))
	{
		lambda(figure_position.x + 2, figure_position.y - 1);
	}
	if (figure_can_move(figure_position.x - 2, figure_position.y + 1))
	{
		lambda(figure_position.x - 2, figure_position.y + 1);
	}
	if (figure_can_move(figure_position.x - 2, figure_position.y - 1))
	{
		lambda(figure_position.x - 2, figure_position.y - 1);
	}

	if (figure_can_move(figure_position.x + 1, figure_position.y + 2))
	{
		lambda(figure_position.x + 1, figure_position.y + 2);
	}
	if (figure_can_move(figure_position.x + 1, figure_position.y - 2))
	{
		lambda(figure_position.x + 1, figure_position.y - 2);
	}
	if (figure_can_move(figure_position.x - 1, figure_position.y + 2))
	{
		lambda(figure_position.x - 1, figure_position.y + 2);
	}
	if (figure_can_move(figure_position.x - 1, figure_position.y - 2)) 
	{
		lambda(figure_position.x - 1, figure_position.y - 2);
	}
}
_CE_END

_CE_BEGIN
template <typename _Lambda>
inline void __fastcall iterate_pawn(VectorUC figure_position, const FigureColor figure_color, const unsigned char chess_board[8][8], _Lambda lambda)
{
	unsigned char x = figure_position.x;
	unsigned char y = figure_position.y;
	if (figure_color == FigureColor::White)
	{
		if (figure_position.y == 1)
		{
			if ((unsigned char)chess_board[figure_position.y + 2][figure_position.x] == 0x11)
			{
				lambda(figure_position.x, figure_position.y + 2);
			}
		}
		if ((unsigned char)chess_board[figure_position.y + 1][figure_position.x] == 0x11)
		{
			lambda(figure_position.x, figure_position.y + 1);
		}
		if ((unsigned char)chess_board[figure_position.y + 1][figure_position.x + 1] != 0x11)
		{
			lambda(figure_position.x + 1, figure_position.y + 1);
		}
		if ((unsigned char)chess_board[figure_position.y + 1][figure_position.x - 1] != 0x11)
		{
			lambda(figure_position.x - 1, figure_position.y + 1);
		}
	}
	else if (figure_color == FigureColor::Black)
	{
		if (figure_position.y == 6)
		{
			if ((unsigned char)chess_board[figure_position.y - 2][figure_position.x] == 0x11)
			{
				lambda(figure_position.x, figure_position.y - 2);
			}
		}
		if ((unsigned char)chess_board[figure_position.y - 1][figure_position.x] == 0x11)
		{
			lambda(figure_position.x, figure_position.y - 1);
		}
		if ((unsigned char)chess_board[figure_position.y - 1][figure_position.x + 1] != 0x11)
		{
			lambda(figure_position.x + 1, figure_position.y - 1);
		}
		if ((unsigned char)chess_board[figure_position.y - 1][figure_position.x - 1] != 0x11)
		{
			lambda(figure_position.x - 1, figure_position.y - 1);
		}
	}
}
_CE_END

_CE_BEGIN
template <typename _Lambda>
inline void __fastcall iterate_rook(VectorUC figure_position, _Lambda lambda)
{
	unsigned char x = figure_position.x + 1;
	unsigned char y = figure_position.y;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x++;
	}
	x = figure_position.x - 1;
	y = figure_position.y;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x--;
	}
	x = figure_position.x;
	y = figure_position.y + 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		y++;
	}
	x = figure_position.x;
	y = figure_position.y - 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		y--;
	}
}
_CE_END

_CE_BEGIN
template <typename _Lambda>
inline void __fastcall iterate_queen(VectorUC figure_position, _Lambda lambda)
{
	unsigned char x = figure_position.x + 1;
	unsigned char y = figure_position.y;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x++;
	}
	x = figure_position.x - 1;
	y = figure_position.y;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x--;
	}
	x = figure_position.x;
	y = figure_position.y + 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		y++;
	}
	x = figure_position.x;
	y = figure_position.y - 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		y--;
	}
	x = figure_position.x + 1;
	y = figure_position.y + 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x++;
		y++;
	}

	x = figure_position.x - 1;
	y = figure_position.y - 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x--;
		y--;
	}
	x = figure_position.x - 1;
	y = figure_position.y + 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y)) 
		{
			break;
		}
		x--;
		y++;
	}
	x = figure_position.x + 1;
	y = figure_position.y - 1;
	while (x < 8 && y < 8)
	{
		if (!lambda(x, y))
		{
			break;
		}
		x++;
		y--;
	}
}
_CE_END

_CE_BEGIN
template <typename _Lambda>
inline void __fastcall iterate_king(VectorUC figure_position, _Lambda lambda)
{
	if (figure_can_move(figure_position.x + 1, figure_position.y)) 
	{
		lambda(figure_position.x + 1, figure_position.y);
	}
	if (figure_can_move(figure_position.x - 1, figure_position.y))
	{
		lambda(figure_position.x - 1, figure_position.y);
	}
	if (figure_can_move(figure_position.x, figure_position.y + 1))
	{
		lambda(figure_position.x, figure_position.y + 1);
	}
	if (figure_can_move(figure_position.x, figure_position.y - 1))
	{
		lambda(figure_position.x, figure_position.y - 1);
	}
	if (figure_can_move(figure_position.x + 1, figure_position.y + 1))
	{
		lambda(figure_position.x + 1, figure_position.y + 1);
	}
	if (figure_can_move(figure_position.x - 1, figure_position.y - 1))
	{
		lambda(figure_position.x - 1, figure_position.y - 1);
	}
	if (figure_can_move(figure_position.x + 1, figure_position.y + 1))
	{
		lambda(figure_position.x + 1, figure_position.y + 1);
	}
	if (figure_can_move(figure_position.x - 1, figure_position.y - 1))
	{
		lambda(figure_position.x - 1, figure_position.y - 1);
	}
}
_CE_END

_CE_BEGIN
struct MoveTree
{
	MoveTree(
		const FigureColor figure_color,
		VectorUC figure_position, 
		size_t depth, 
		unsigned char chess_board_figures[8][8],
		bool not_first_color_tree = false
	) :
		figure_color(figure_color)
	{
		if (depth > 0)
		{
			if (not_first_color_tree)
			{
				this->next_move_tree = new MoveTree(
					figure_color == FigureColor::White ? FigureColor::Black : FigureColor::White, figure_position,
					depth - 1, chess_board_figures, false);
			}
			else
			{
				this->next_move_tree = new MoveTree(
					figure_color == FigureColor::White ? FigureColor::Black : FigureColor::White, figure_position,
					depth, chess_board_figures, true);
			}
		}
	}
	const FigureColor figure_color;
	MoveTree* next_move_tree;
};
_CE_END

_CE_API void get_possible_moves(
	_CE VectorUC* moves_data, 
	unsigned char* moves_count, 
	_CE VectorUC* moves_attack_data,
	unsigned char* moves_attack_count,
	_CE VectorUC figure_position, 
	_CE FigureType figure_type, 
	_CE FigureColor figure_color, 
	const unsigned char chess_board[8][8]
)
{
	unsigned char move_idx = 0;
	unsigned char move_attack_idx = 0;

	static auto get_possible_moves_lambd = [&](unsigned char x, unsigned char y)
	{
		if (chess_board[y][x] != 0x11)
		{
			if (GET_FIGURE_COLOR(chess_board[y][x]) != (unsigned char)figure_color)
			{
				moves_attack_data[move_attack_idx] = _CE VectorUC(x, y);
				move_attack_idx++;
			}
			return false;
		}
		moves_data[move_idx] = _CE VectorUC(x, y);
		move_idx++;
		return true;
	};

	switch (figure_type)
	{
	case _CE FigureType::Pawn:
		iterate_pawn(figure_position, figure_color, chess_board, get_possible_moves_lambd);
		break;
	case _CE FigureType::Elephant:
		iterate_elephant(figure_position, get_possible_moves_lambd);
		break;
	case _CE FigureType::Horse:
		iterate_horse(figure_position, get_possible_moves_lambd);
		break;
	case _CE FigureType::Rook:
		iterate_rook(figure_position, get_possible_moves_lambd);
		break;
	case _CE FigureType::Queen:
		iterate_queen(figure_position, get_possible_moves_lambd);
		break;
	case _CE FigureType::King:
		iterate_king(figure_position, get_possible_moves_lambd);
		break;
	}

	*moves_count = move_idx;
	*moves_attack_count = move_attack_idx;
}

_CE_API bool is_real_move(_CE VectorUC* moves_data, 
	const unsigned char moves_count, _CE VectorUC move_to)
{
	for (unsigned char i = 0; i < moves_count; i++)
	{
		if (moves_data[i].x == move_to.x && moves_data[i].y == move_to.y)
		{
			return true;
		}
	}
	return false;
}

_CE_API void get_best_moves(
	unsigned char max_best_moves_count,
	unsigned char* best_moves_count,
	unsigned char* max_cost,
	unsigned char* best_y,
	unsigned char* best_x,
	unsigned char* curr_y,
	unsigned char* curr_x,
	_CE FigureColor figure_color,
	const unsigned char chess_board[8][8]
	)
{
	for (unsigned char y = 0; y < 8; y++)
	{
		for (unsigned char x = 0; x < 8; x++)
		{
			if (GET_FIGURE_COLOR(chess_board[y][x]) == (unsigned char)figure_color)
			{
				static auto iterate_lambda = [&](unsigned char xx, unsigned char yy)
				{
					if (chess_board[yy][xx] != 0x11)
					{
						if (GET_FIGURE_COLOR(chess_board[yy][xx]) == (unsigned char)figure_color)
						{
							return false;
						}
						const unsigned char cost =
							_CE figure_cost[GET_FIGURE_TYPE(chess_board[yy][xx])] + _CE field_cost[yy][xx];
						for (unsigned char i = 0; i < max_best_moves_count; i++)
						{
							if (cost > max_cost[i])
							{
								max_cost[i] = cost;
								best_y[i] = yy;
								best_x[i] = xx;
								curr_x[i] = x;
								curr_y[i] = y;
							}
						}
						return false;
					}
					const unsigned char cost = _CE field_cost[yy][xx];
					for (unsigned char i = 0; i < max_best_moves_count; i++)
					{
						if (cost > max_cost[i])
						{
							max_cost[i] = cost;
							best_y[i] = yy;
							best_x[i] = xx;
							curr_x[i] = x;
							curr_y[i] = y;
						}
					}
					return true;
				};
				switch ((_CE FigureType)GET_FIGURE_TYPE(chess_board[y][x]))
				{
				case _CE FigureType::Pawn:
					iterate_pawn(
						_CE VectorUC(x, y),
						(_CE FigureColor)GET_FIGURE_COLOR(chess_board[y][x]),
						chess_board,
						iterate_lambda
					);
					break;
				case _CE FigureType::Elephant:
					iterate_elephant(_CE VectorUC(x, y), iterate_lambda);
					break;
				case _CE FigureType::Horse:
					iterate_horse(_CE VectorUC(x, y), iterate_lambda);
					break;
				case _CE FigureType::Rook:
					iterate_rook(_CE VectorUC(x, y), iterate_lambda);
					break;
				case _CE FigureType::Queen:
					iterate_queen(_CE VectorUC(x, y), iterate_lambda);
					break;
				case _CE FigureType::King:
					iterate_king(_CE VectorUC(x, y), iterate_lambda);
					break;
				}
			}
		}
	}
}

_CE_API void get_material_advantage(unsigned short* white, unsigned short* black, unsigned char chess_board[8][8])
{
	for (unsigned char y = 0; y < 8; y++)
	{
		for (unsigned char x = 0; x < 8; x++)
		{
			if (chess_board[y][x] != 0x11)
			{
				if (GET_FIGURE_COLOR(chess_board[y][x]) == (unsigned char)_CE FigureColor::White)
				{
					*white += _CE figure_cost[GET_FIGURE_TYPE(chess_board[y][x])] + _CE field_cost[y][x];
				}
				else
				{
					*black += _CE figure_cost[GET_FIGURE_TYPE(chess_board[y][x])] + _CE field_cost[y][x];
				}
			}
		}
	}
}

using namespace chess_engine;

class BufferFileEmulator
{
public:
	void* buffer = nullptr;
	DWORD length = 0;
	void __stdcall load(int lpName, LPCWSTR lpType)
	{
		const HRSRC src = FindResource(nullptr, MAKEINTRESOURCE(lpName), lpType);
		if (src != nullptr) {
			this->length = ::SizeofResource(nullptr, src);
			const HGLOBAL myResourceData = LoadResource(nullptr, src);

			if (myResourceData != nullptr)
			{
				this->buffer = LockResource(myResourceData);
			}
		}
	}
};

class ChessFigures
{
public:
	ChessFigures()
	{
		BufferFileEmulator buffer;
		buffer.load(IDB_PNG2, L"PNG");
		texture_.loadFromMemory(buffer.buffer, buffer.length);
		for (unsigned char y = 0; y < 2; y++)
		{
			for (unsigned char x = 0; x < 6; x++)
			{
				if (y == 0)
				{
					rectangle_shapes_[x].setTexture(&texture_);
					rectangle_shapes_[x].setTextureRect(sf::IntRect(sf::Vector2i(x * 200, y * 325), sf::Vector2i(200, 325)));
					rectangle_shapes_[x].setSize(sf::Vector2f(800.f / 12.f, 800.f / 8.f));
				}
				else
				{
					rectangle_shapes_[11 - x].setTexture(&texture_);
					rectangle_shapes_[11 - x].setTextureRect(sf::IntRect(sf::Vector2i(x * 200, y * 325), sf::Vector2i(200, 325)));
					rectangle_shapes_[11 - x].setSize(sf::Vector2f(800.f / 12.f, 800.f / 8.f));
				}
			}
		}
	}
	sf::RectangleShape& get_figure_rectangle_shape(
		const unsigned char figure, const unsigned char x, const unsigned char y) const
	{
		if (GET_FIGURE_TYPE(figure) == (unsigned char)FigureType::Empty || 
			GET_FIGURE_COLOR(figure) == (unsigned char)FigureColor::Empty)
			return rectangle_shapes_[12];
		if (GET_FIGURE_COLOR(figure) == (unsigned char)FigureColor::White)
		{
			rectangle_shapes_[GET_FIGURE_TYPE(figure) - 2].setPosition(800.f / 8.f * x + 15, 800.f / 8.f * y);
			return rectangle_shapes_[GET_FIGURE_TYPE(figure) - 2];
		}
		rectangle_shapes_[6 + GET_FIGURE_TYPE(figure) - 2].setPosition(800.f / 8.f * x + 15, 800.f / 8.f * y);
		return rectangle_shapes_[6 + GET_FIGURE_TYPE(figure) - 2];
	}
private:
	sf::Texture texture_;
	mutable sf::RectangleShape rectangle_shapes_[13];
};

class ChessBoard : public sf::Drawable
{
public:
	ChessBoard(const unsigned int x, const unsigned int y, PartySettings& party_settings) : 
		party_settings_(party_settings),
		possible_moves_data_(new VectorUC[32]),
		possible_moves_count_(new unsigned char(0)),
		possible_moves_attack_data_(new VectorUC[32]),
		possible_moves_attack_count_(new unsigned char(0))
	{
		BufferFileEmulator buffer;
		buffer.load(IDB_PNG1, L"PNG");
		texture_.loadFromMemory(buffer.buffer, buffer.length);
		rectangle_shape_.setTexture(&texture_);
		rectangle_shape_.setSize(sf::Vector2f(x, y));
		for (unsigned char y = 0; y < 8; y++)
		{
			for (unsigned char x = 0; x < 8; x++)
			{
				blure_rectangle_shapes_[y * 8 + x].setSize(sf::Vector2f(800.f / 8.f, 800.f / 8.f));
				blure_rectangle_shapes_[y * 8 + x].setFillColor(sf::Color(0, 0, 0, 0));
				blure_rectangle_shapes_[y * 8 + x].setPosition(x * (800.f / 8.f), y * (800.f / 8.f));
			}
		}
	}
	void resize(const unsigned int x, const unsigned int y)
	{
		sf::Vector2f targetSize(800, 800);
		sf::Vector2f targetSize2(800.f / 8.f, 800.f / 8.f);
		rectangle_shape_.setScale(
			targetSize.x / rectangle_shape_.getLocalBounds().width,
			targetSize.y / rectangle_shape_.getLocalBounds().height);
		for (unsigned char y = 0; y < 8; y++)
		{
			for (unsigned char x = 0; x < 8; x++)
			{
				blure_rectangle_shapes_[y * 8 + x].setScale(
					targetSize2.x / blure_rectangle_shapes_[y * 8 + x].getLocalBounds().width,
					targetSize2.y / blure_rectangle_shapes_[y * 8 + x].getLocalBounds().height);
			}
		}
	}
	void on_click_left(const unsigned char x, const unsigned char y)
	{
		if (GET_FIGURE_COLOR(party_settings_.chess_board_figures[y][x]) == (unsigned char)FigureColor::Black)
		{
			if (x >= 8 || y >= 8)
			{
				return;
			}
			if (blure_left_rectangle_shape_ != nullptr)
			{
				blure_left_rectangle_shape_->setFillColor(sf::Color(0, 0, 0, 0));
			}
			blure_rectangle_shapes_[y * 8 + x].setFillColor(sf::Color(0, 255, 0, 100));
			blure_left_rectangle_shape_ = &blure_rectangle_shapes_[y * 8 + x];
			left_click_position_ = sf::Vector2u(x, y);
			for (unsigned char i = 0; i < *possible_moves_count_; i++)
			{
				blure_rectangle_shapes_[possible_moves_data_[i].y * 8 + 
					possible_moves_data_[i].x].setFillColor(sf::Color(0, 0, 0, 0));
			}
			for (unsigned char i = 0; i < *possible_moves_attack_count_; i++)
			{
				blure_rectangle_shapes_[possible_moves_attack_data_[i].y * 8 +
					possible_moves_attack_data_[i].x].setFillColor(sf::Color(0, 0, 0, 0));
			}
			get_possible_moves(
				possible_moves_data_,
				possible_moves_count_,
				possible_moves_attack_data_,
				possible_moves_attack_count_,
				VectorUC(x, y),
				(FigureType)GET_FIGURE_TYPE(party_settings_.chess_board_figures[y][x]),
				(FigureColor)GET_FIGURE_COLOR(party_settings_.chess_board_figures[y][x]),
				party_settings_.chess_board_figures
			);
			for (unsigned char i = 0; i < *possible_moves_count_; i++)
			{
				blure_rectangle_shapes_[possible_moves_data_[i].y * 8 + 
					possible_moves_data_[i].x].setFillColor(sf::Color(0, 0, 255, 150));
			}
			for (unsigned char i = 0; i < *possible_moves_attack_count_; i++)
			{
				blure_rectangle_shapes_[possible_moves_attack_data_[i].y * 8 + 
					possible_moves_attack_data_[i].x].setFillColor(sf::Color(255, 0, 0, 150));
			}
		}
	}
	void on_click_right(const unsigned char x, const unsigned char y)
	{
		if (x >= 8 || y >= 8)
		{
			return;
		}
		if (blure_right_rectangle_shape_ != nullptr)
		{
			blure_right_rectangle_shape_->setFillColor(sf::Color(0, 0, 0, 0));
		}
		blure_rectangle_shapes_[y * 8 + x].setFillColor(sf::Color(255, 255, 0, 100));
		blure_right_rectangle_shape_ = &blure_rectangle_shapes_[y * 8 + x];
		right_click_position_ = sf::Vector2u(x, y);
		if (blure_left_rectangle_shape_ != nullptr)
		{
			if (is_real_move(possible_moves_data_, *possible_moves_count_, VectorUC(x, y)) || 
				is_real_move(possible_moves_attack_data_, *possible_moves_attack_count_, VectorUC(x, y)))
			{
				unsigned char& figure = party_settings_.chess_board_figures[left_click_position_.y][left_click_position_.x];
				unsigned char& empty = party_settings_.chess_board_figures[right_click_position_.y][right_click_position_.x];
				empty = figure;
				figure = 0x11;
				move_white_by_computer();
			}
		}
		blure_left_rectangle_shape_ = nullptr;
		blure_right_rectangle_shape_ = nullptr; 
		for (unsigned char y = 0; y < 8; y++)
		{
			for (unsigned char x = 0; x < 8; x++)
			{
				blure_rectangle_shapes_[y * 8 + x].setFillColor(sf::Color(0, 0, 0, 0));
			}
		}
	}
	void move_white_by_computer()
	{
		unsigned char max_best_moves_count = 3;
		unsigned char best_moves_count = 0;
		unsigned char max_cost[3] = { 0, 0, 0 };
		unsigned char best_y[3] = { 0, 0, 0 };
		unsigned char best_x[3] = { 0, 0, 0 };
		unsigned char curr_y[3] = { 0, 0, 0 };
		unsigned char curr_x[3] = { 0, 0, 0 };
		FigureColor curr_color = FigureColor::White;

		get_best_moves(max_best_moves_count, &best_moves_count, max_cost, best_y, best_x, curr_y, curr_x, curr_color, party_settings_.chess_board_figures);

		unsigned char chess_board_copy[8][8];

		signed short max = -0x0FFF;
		unsigned char nbest_y = 255;
		unsigned char nbest_x = 255;
		unsigned char ncurr_y = 255;
		unsigned char ncurr_x = 255;

		for (int i = 0; i < 3; i++)
		{
			memcpy(&chess_board_copy[0][0], &party_settings_.chess_board_figures[0][0], 8 * 8);
			{
				unsigned char& figure = chess_board_copy[curr_y[i]][curr_x[i]];
				unsigned char& empty = chess_board_copy[best_y[i]][best_x[i]];
				empty = figure;
				figure = 0x11;
			}

			unsigned char max_best_moves_count2 = 1;
			unsigned char best_moves_count2 = 0;
			unsigned char max_cost2 = 0;
			unsigned char best_y2 = 255;
			unsigned char best_x2 = 255;
			unsigned char curr_y2 = 255;
			unsigned char curr_x2 = 255;
			FigureColor curr_color2 = FigureColor::Black;

			unsigned short score_white1 = 0;
			unsigned short score_black1 = 0;
			get_material_advantage(&score_white1, &score_black1, chess_board_copy);

			get_best_moves(max_best_moves_count2, &best_moves_count2, &max_cost2, &best_y2, &best_x2, &curr_y2, &curr_x2, curr_color2, chess_board_copy);
			{
				unsigned char& figure = chess_board_copy[curr_y2][curr_x2];
				unsigned char& empty = chess_board_copy[best_y2][best_x2];
				empty = figure;
				figure = 0x11;
			}

			unsigned short score_white2 = 0;
			unsigned short score_black2 = 0;
			get_material_advantage(&score_white2, &score_black2, chess_board_copy);

			std::cout << "black's cost 1 = " << score_black1 << " white's cost 1 = " << score_white1 << std::endl;
			std::cout << "black's cost 2 = " << score_black2 << " white's cost 2 = " << score_white2 << std::endl;
			if (score_white2 > max)
			{
				max = score_white2;
				nbest_y = best_y[i];
				nbest_x = best_x[i];
				ncurr_y = curr_y[i];
				ncurr_x = curr_x[i];
			}
		}
		unsigned char& figure = party_settings_.chess_board_figures[ncurr_y][ncurr_x];
		unsigned char& empty = party_settings_.chess_board_figures[nbest_y][nbest_x];
		empty = figure;
		figure = 0x11;
	}

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(rectangle_shape_);
		for (unsigned char y = 0; y < 8; y++)
		{
			for (unsigned char x = 0; x < 8; x++)
			{
				target.draw(blure_rectangle_shapes_[y * 8 + x]);
				target.draw(chess_figures_.get_figure_rectangle_shape(party_settings_.chess_board_figures[y][x], x, y));
			}
		}
	}
	sf::Texture texture_;
	sf::RectangleShape rectangle_shape_;
	sf::RectangleShape blure_rectangle_shapes_[64];
	sf::RectangleShape* blure_left_rectangle_shape_;
	sf::Vector2u left_click_position_;
	sf::RectangleShape* blure_right_rectangle_shape_;
	sf::Vector2u right_click_position_;
	ChessFigures chess_figures_;
	PartySettings& party_settings_;
	VectorUC* possible_moves_data_;
	unsigned char* possible_moves_count_;
	VectorUC* possible_moves_attack_data_;
	unsigned char* possible_moves_attack_count_;
};

int main()
{

	sf::ContextSettings sfml_window_context_settings;
	sfml_window_context_settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(800, 800), "ChessQueenEngine!", 7U, sfml_window_context_settings);
	window.setFramerateLimit(15);
	WINDOWINFO wiInfo;
	GetWindowInfo(window.getSystemHandle(), &wiInfo);
	PartySettings party_settings;
	ChessBoard sfml_chess_board(wiInfo.rcClient.right - wiInfo.rcClient.left,
		wiInfo.rcClient.bottom - wiInfo.rcClient.top, party_settings);

	const MoveTree* move_tree = new MoveTree(FigureColor::White, { 0, 0 },
		1, party_settings.chess_board_figures);
	
	BufferFileEmulator buffer;
	buffer.load(IDR_OGG1, L"ogg");
	sf::Sound sound;
	sf::SoundBuffer sound_buffer;
	sound_buffer.loadFromMemory(buffer.buffer, buffer.length);
	sound.setBuffer(sound_buffer);
	sound.setLoop(true);
	sound.play();
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::Resized)
			{
				GetWindowInfo(window.getSystemHandle(), &wiInfo);
				sfml_chess_board.resize(wiInfo.rcClient.right - wiInfo.rcClient.left, 
					wiInfo.rcClient.bottom - wiInfo.rcClient.top);
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				sfml_chess_board.on_click_left(sf::Mouse::getPosition(window).x / (window.getSize().x / 8),
					sf::Mouse::getPosition(window).y / (window.getSize().y / 8));
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				sfml_chess_board.on_click_right(sf::Mouse::getPosition(window).x / 100, 
					sf::Mouse::getPosition(window).y / 100);
			}
        }

        window.clear();
        window.draw(sfml_chess_board);
        window.display();
    }

	return 0;

}
