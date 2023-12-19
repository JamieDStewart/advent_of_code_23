//\==============================================================================
// Advent of code 2023 Day 16
// Question: https://adventofcode.com/2023/day/16
// Gist: A maze, trace a beam through the maze beam splits on '-' '|' reflects off '/' '\' loops possible
// Input data: a level grid of '/' '\' mirrors, '-' '|' splitters, and '.' empty space
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <fstream>
#include <numeric>
#include <queue>
#include <ranges>
#include <string>
#include <sstream>
#include <vector>

namespace day_16
{

	std::vector<std::string> read_input_file( const std::string& input_file )
	{
		std::vector<std::string> map;
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					map.push_back( line );
				}
			}
			file_input.close();
		}
		return map;
	}

	struct V2
	{
		int32_t x;
		int32_t y;

		bool operator ==( const V2& r ) const 
		{
			return (x == r.x && y == r.y);
		}

		V2& operator +=( const V2& r )
		{
			x += r.x; y += r.y;
			return *this;
		}

		V2 operator +( const V2& r ) const
		{
			return { x + r.x, y + r.y };
		}
	};
	//define cardinal directions
	constexpr V2 NORTH = { 0, -1 };
	constexpr V2 SOUTH = { 0, 1 };
	constexpr V2 EAST = { 1, 0 };
	constexpr V2 WEST = { -1, 0 };
	//define cardinal directions bit values
	constexpr uint8_t U8_NORTH = 0x1;
	constexpr uint8_t U8_EAST = 0x4;
	constexpr uint8_t U8_SOUTH = 0x2;
	constexpr uint8_t U8_WEST = 0x8;

	struct Light
	{
		V2 pos;
		V2 direction;
	};	

	V2 get_direction( const uint8_t dir )
	{
		switch(dir)
		{
		case U8_NORTH: return NORTH;
		case U8_SOUTH: return SOUTH;
		case U8_EAST: return EAST;
		default: return WEST;
		}
	}

	uint8_t get_direction_value( const V2& dir )
	{
		if ( dir == NORTH )
		{
			return U8_NORTH;
		}
		else if ( dir == SOUTH )
		{
			return U8_SOUTH;
		}
		else if ( dir == EAST )
		{
			return U8_EAST;
		}
		return U8_WEST;
	}

	V2 get_mirror_output_direction( const char mirror, const uint8_t in_dir )
	{
		if ( mirror == '/' )
		{
			switch ( in_dir )
			{
			case U8_WEST: return SOUTH;
			case U8_EAST: return NORTH;
			case U8_NORTH: return EAST;
			//case U8_SOUTH: return EAST;
			default: return WEST;
			}
		}
		//else if ( mirror == '\\' )
		{
			switch ( in_dir )
			{
			case U8_WEST: return NORTH;
			case U8_EAST: return SOUTH;
			case U8_NORTH: return WEST;
			//case U8_SOUTH: return EAST;
			default: return EAST;
			}
		}
	}

	uint8_t get_splitter_output_directions( const char splitter, const uint8_t in_dir )
	{
		if ( splitter == '-' )
		{
			switch ( in_dir )
			{
			case U8_NORTH: 
			case U8_SOUTH: return U8_WEST | (U8_EAST << 4);
			default: return 0;
			}
		}
		//if ( splitter == '|' )
		{
			switch ( in_dir )
			{
			case U8_EAST:
			case U8_WEST: return U8_NORTH | (U8_SOUTH << 4);
			default: return 0;
			}
		}
		
	}

	std::vector<std::vector<uint8_t>> navigate_maze( const Light& entry_light, const std::vector<std::string>& map )
	{
		const auto map_width = static_cast<int32_t>(map[0].length());
		const auto map_height = static_cast<int32_t>(map.size());
		std::vector<std::vector<uint8_t>> visited_squares( map.size(), std::vector<uint8_t>( map_width, 0 ) );

		std::queue<Light> lights;
		lights.push( entry_light );
		while ( !lights.empty() )
		{
			auto [pos, dir] = lights.front();
			lights.pop();
			//for each light navigate through the level until a splitter or mirror is hit
			bool light_continues = true;
			while ( light_continues )
			{
				if ( pos.x < 0 || pos.x >= map_width || pos.y < 0 || pos.y >= map_height )
				{
					break;
				}
				uint8_t& square = visited_squares[pos.y][pos.x];
				//has the light passed through this square in the current direction previously
				const uint8_t dir_value = get_direction_value( dir );
				if ( (square & dir_value) != 0 )
				{
					//square has been visited this way before, don't loop
					light_continues = false;
					continue;
				}
				square |= dir_value; //mark square as visited from this direction
				//look at the map and determine if this light should continue it's journey or end
				
				if ( const char map_tile = map[pos.y][pos.x]; map_tile == '.' )
				{
					pos += dir;
				}
				else if ( map_tile == '/' || map_tile == '\\' )
				{
					//change the direction of the light and move to the next location
					V2 new_dir = get_mirror_output_direction( map_tile, dir_value );
					lights.push( { pos + new_dir, new_dir } );
					light_continues = false;
				}
				else if ( map_tile == '|' || map_tile == '-' )
				{
					//light has hit a splitter
					const auto out_dirs = get_splitter_output_directions( map_tile, dir_value );
					if ( out_dirs == 0 )
					{
						//no split carry on in current direction
						pos += dir;
					}
					else
					{
						//beam was split upper 4 bits one direction lower 8 bits the other
						//first direction
						uint8_t dir = out_dirs & 0xF;
						V2 new_dir = get_direction( dir );
						lights.push( { pos + new_dir, new_dir } );
						//second direction
						dir = (out_dirs >> 4) & 0xF;
						new_dir = get_direction( dir );
						lights.push( { pos + new_dir, new_dir } );

						light_continues = false;
					}
				}
			}
		}
		return visited_squares;
	}

	uint64_t sum_visited_tiles( const std::vector<std::vector<uint8_t>>& tile_map )
	{
		//lights have traveled through the maze now to add up tiles visited
		uint64_t sum_visited_squares = {};
		for ( const auto& row : tile_map )
		{
			sum_visited_squares += std::accumulate( row.begin(), row.end(), 0, []( uint64_t total, uint8_t v )
													{
														return total + (v != 0);
													} );
		}
		return sum_visited_squares;
	}

	uint64_t part_01( const std::vector<std::string>& map )
	{
		//light starts in position 0,0 heading east
		const std::vector<std::vector<uint8_t>> visited_squares = navigate_maze( { {0,0}, {1,0} }, map );
		return sum_visited_tiles( visited_squares );
		
	}

	uint64_t part_02( const std::vector<std::string>& map )
	{
		//light can start in any edge location
		uint64_t max_sum = {};
		const auto map_width = static_cast<int32_t>(map[0].length());
		const auto map_height = static_cast<int32_t>(map.size());
		//for top row heading south & bottom row heading north
		for ( int32_t x = 0; x < map_width; ++x )
		{
			uint64_t sum = sum_visited_tiles( navigate_maze( { {x,0}, SOUTH }, map ) );
			if( sum > max_sum )
			{
				max_sum = sum;
			}
			sum = sum_visited_tiles( navigate_maze( { {x,map_height-1}, NORTH }, map ) );
			if ( sum > max_sum )
			{
				max_sum = sum;
			}
		}
		//for left column heading west & right column heading east
		for ( int32_t y = 0; y < map_height; ++y )
		{
			uint64_t sum = sum_visited_tiles( navigate_maze( { {0,y}, EAST }, map ) );
			if ( sum > max_sum )
			{
				max_sum = sum;
			}
			sum = sum_visited_tiles( navigate_maze( { {map_width-1,y}, WEST }, map ) );
			if ( sum > max_sum )
			{
				max_sum = sum;
			}
		}

		return max_sum;
	}


}

Result aoc::day_16()
{
	timer::start();
	const std::vector<std::string> map = day_16::read_input_file( "./input/day_16.txt" );

	const uint64_t part_1_answer = day_16::part_01( map );

	const uint64_t part_2_answer = day_16::part_02( map );

	timer::stop();
	return { std::string( "16: Light Maze" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}