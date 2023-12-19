//\==============================================================================
// Advent of code 2023 Day 18
// Question: https://adventofcode.com/2023/day/18
// Gist: Dig out a trench from coordinates and fill it in
// Input data: Coordinates to dig
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <ranges>
#include <set>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace day_18
{
	struct V2
	{
		int64_t x;
		int64_t y;
		bool operator ==( const V2& r ) const
		{
			return (x == r.x && y == r.y);
		}

		V2& operator +=( const V2& r )
		{
			x += r.x; y += r.y;
			return *this;
		}

		V2 operator *( const int64_t s ) const
		{
			return { x * s, y * s };
		}
	};

	struct Instruction
	{
		V2 direction;
		int64_t distance;
		std::string color;
	};

	V2 get_direction( const char c )
	{
		switch(c)
		{
		case 'U': case '3': return {0, 1};
		case 'D': case '1': return {0, -1};
		case 'L': case '2': return {-1, 0};
		default: return { 1, 0 };
		}
	}

	std::vector<Instruction> read_input_file( const std::string& input_file )
	{
		std::vector<Instruction> instructions = {};
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//read first character convert to direction
					const V2 direction = get_direction( line[0] );
					int64_t distance = {};
					std::string color = {};
					std::size_t brace_pos = line.find_first_of( '(' );
					std::stringstream( line.substr( 2, brace_pos ) ) >> distance;

					std::stringstream( line.substr(brace_pos+2, 6 ) ) >> color;
					instructions.push_back( { direction, distance, color } );
				}
			}
			file_input.close();
		}
		return instructions;
	}

	uint64_t shoelace( const std::vector<V2>& vertices )
	{
		int64_t left = {}; int64_t right = {};
		for( size_t i = 0; i < vertices.size()-1; ++i)
		{
			left  += vertices[i].x * vertices[i + 1].y;
			right += vertices[i].y * vertices[i + 1].x;
		}
		return abs(left - right) >> 1;
	}

	uint64_t part_01( const std::vector<Instruction>& instructions )
	{
		//work out the volume of a shape from it's vertices
		V2 pos = { 0,0 };
		std::vector<V2> vertices = {1, pos};
		for ( const auto& [direction, distance, color] : instructions )
		{
			pos += (direction * distance);
			vertices.push_back( pos );
		}
		//make sure start and end points are the same
		if( vertices[0] != vertices[vertices.size()-1] )
		{
			vertices.push_back( vertices[0] );
		}
		const uint64_t perimeter = std::accumulate( instructions.begin(), instructions.end(), 0, []( uint64_t total, const Instruction& i )
													{
													return	total += static_cast<uint64_t>(i.distance);
													} );
		
		return shoelace( vertices ) + perimeter / 2 + 1;
	}

	uint64_t part_02( const std::vector<Instruction>& instructions )
	{
		//instructions were wrong as elves are, well.... elves.
		//convert colour parameter of instructions to dig code first 5 values are distance to dig, last digit is direction
		std::vector<Instruction> decoded_instructions = {};
		for( const auto& [direction, distance, color] : instructions )
		{
			int64_t decoded_distance = {};
			std::stringstream( color.substr( 0, color.length() - 1 ) ) >> std::hex >> decoded_distance;
			const V2 decoded_dir = get_direction( color[color.length() - 1] );
			decoded_instructions.push_back( { decoded_dir, decoded_distance, color } );
		}
		//work out the volume of a shape from it's vertices
		V2 pos = { 0,0 };
		std::vector<V2> vertices = { 1, pos };
		for ( const auto& [direction, distance, color] : decoded_instructions )
		{
			pos += (direction * distance);
			vertices.push_back( pos );
		}
		//make sure start and end points are the same
		if ( vertices[0] != vertices[vertices.size() - 1] )
		{
			vertices.push_back( vertices[0] );
		}
		const uint64_t perimeter = std::accumulate( decoded_instructions.begin(), decoded_instructions.end(), 0, []( uint64_t total, const Instruction& i )
													{
														return	total += static_cast<uint64_t>(i.distance);
													} );

		return shoelace( vertices ) + perimeter / 2 + 1;
	}


}

Result aoc::day_18()
{
	timer::start();
	const std::vector<day_18::Instruction> instructions = day_18::read_input_file( "./input/day_18.txt" );

	const uint64_t part_1_answer = day_18::part_01( instructions );

	const uint64_t part_2_answer = day_18::part_02( instructions );

	timer::stop();
	return { std::string( "18: Diggin' a Lava Pit!" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}