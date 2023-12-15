//\==============================================================================
// Advent of code 2023 Day 15
// Question: https://adventofcode.com/2023/day/15
// Gist: String Hashing - use the provided hash function to has a big ass string
// Input data: a long string that needs hashing, delimiter is ',' for each substring to hash ignore '\n'
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <fstream>
#include <numeric>
#include <ranges>
#include <string>
#include <sstream>
#include <vector>

namespace day_15
{

	std::vector<std::string> read_input_file( const std::string& input_file )
	{
		std::vector<std::string> steps;
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					std::string set_string{};
					std::stringstream input_stream( line );
					//split the input string on the ';' character
					while ( std::getline( input_stream, set_string, ',' ) )
					{
						steps.push_back( set_string );
					}
				}
			}
			file_input.close();
		}

		return steps;
	}

	uint64_t Holiday_ASCII_String_Helper( const std::string& str )
	{
		uint64_t v = 0;
		for ( const auto& c : str )
		{
			v += static_cast<uint64_t>(c);
			v *= 17;
			v %= 256;			
		}
		return v;
	}
	
	uint64_t part_01( const std::vector<std::string>& steps )
	{
		std::vector<uint64_t> hashed_steps = {};
		for( const auto& step : steps )
		{
			hashed_steps.push_back( Holiday_ASCII_String_Helper( step ) );
		}
		return std::accumulate(hashed_steps.begin(), hashed_steps.end(), 0LLU );
	}

	

	uint64_t part_02( const std::vector<std::string>& steps )
	{
		struct Box
		{
			std::vector<std::pair< std::string, uint64_t>> lenses;
		};
		//there are 256 boxes
		Box boxes[256] = {};
		
		for ( const auto& step : steps )
		{
			//find the first instance of '=' or '-' in the step
			std::string lens_id = step.substr( 0, step.find_first_of("-=" ));
			const uint64_t box_id = Holiday_ASCII_String_Helper( lens_id );
			//if string contains a digit add it to the box if not remove the
			auto& lenses = boxes[box_id].lenses;
			auto lens = std::ranges::find( lenses, lens_id, &std::pair<std::string, uint64_t>::first );
			
			if( step[step.length()-1] == '-' )
			{
				if( lens != lenses.end() ) { lenses.erase( lens ); } // If the lens is in the box then remove it
			}
			else
			{
				if ( lens != lenses.end() ) { lens->second = step[step.length() - 1] - '0';	continue; } //lens already in box replace lens value
				boxes[box_id].lenses.emplace_back( std::make_pair( lens_id, step[step.length() - 1] - '0' ) );
			}
		}
		//accumulate the values in the boxes using the explained method
		uint64_t total = {};
		uint64_t box_index = 1;
		for( const auto& box : boxes )
		{
			uint64_t lens_index = 1;
			for( const auto& lens : box.lenses | std::views::values )
			{
				total += (box_index * lens_index * lens);
				++lens_index;
			}
			++box_index;
		}
		return total;
	}


}

Result aoc::day_15()
{
	timer::start();
	const std::vector<std::string> hash_steps = day_15::read_input_file( "./input/day_15.txt" );

	const uint64_t part_1_answer = day_15::part_01( hash_steps );

	const uint64_t part_2_answer = day_15::part_02( hash_steps );

	timer::stop();
	return { std::string( "15: Hash Function" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}