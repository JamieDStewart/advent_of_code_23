//\==============================================================================
// Advent of code 2023 Day 3
// Question: https://adventofcode.com/2023/day/3
// Gist:  in a grid add up all part numbers
// Input data: a schematic with numbers indicating parts and ids
//\==============================================================================


#include "days.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace day_03
{

	std::vector<std::string> read_input_file( const std::string& input_file )
	{
		std::vector<std::string> schematic = {};
		//open the input file for and parse into a vector of games
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					schematic.push_back( line );
				}
			}
			file_input.close();
		}
		return schematic;
	}

	struct Part
	{
		uint32_t id;
		uint32_t row;
		uint32_t start;
		uint32_t end;
	};

	std::vector<std::vector<Part>> extract_parts( const std::vector<std::string>& schematic )
	{
		std::vector<std::vector<Part>> parts = {};
		//iterate over all rows in the schematic extracting parts
		for ( uint32_t row = 0; row < schematic.size(); ++row )
		{
			std::vector<Part> row_parts = {};
			//process each row string
			const std::string& row_string = schematic[row];
			size_t start_pos = row_string.find_first_of( "0123456789" );
			while(  start_pos != std::string::npos )
			{
				uint32_t id;
				size_t end_pos = row_string.find_first_not_of( "0123456789", start_pos );
				if( end_pos == std::string::npos )
				{
					//found the end of the line so process from start to end as a part number
					std::stringstream( row_string.substr( start_pos ) ) >> id;
					end_pos = row_string.size();
				}
				else
				{
					std::stringstream( row_string.substr( start_pos, end_pos - start_pos ) ) >> id;
				}
				row_parts.push_back( {id, row, static_cast<uint32_t>(start_pos), static_cast<uint32_t>(end_pos-1)} );
				start_pos = row_string.find_first_of("0123456789", end_pos );
			}
			parts.push_back( row_parts );
		}
		return parts;
	}

	std::vector<uint32_t> get_parts_on_row_for_symbol_at_pos( const uint32_t pos, const int32_t row, const std::vector<std::vector<Part>>& parts )
	{
		std::vector<uint32_t> part_ids = {};
		if ( row < 0 || row >= parts.size() )
		{
			return part_ids;
		}
		//scan the parts on this row to see if any are adjacent to the position passed in
		const std::vector<Part> row_parts = parts[row];
		for ( const auto& part : row_parts )
		{
			
			const uint32_t dist_to_start = (part.start > pos) ? part.start - pos : pos - part.start;
			const uint32_t dist_to_end = (part.end > pos) ? part.end - pos : pos - part.end;
			if ( dist_to_start <= 1 || dist_to_end <= 1 )
			{
				//part is adjacent to symbol
				part_ids.push_back( part.id );
			}
		}
		return part_ids;
	}

	uint32_t part_1(const std::vector<std::string>& schematic, const std::vector<std::vector<Part>>& parts )
	{
		uint32_t sum_of_part_ids = 0;
		//iterate through line by line looking for symbols
		//once symbol found look in row above, this row and row below for part numbers adjacent to symbol
		for ( int32_t row = 0; row < schematic.size(); ++row )
		{
			//process each row string
			const std::string& row_string = schematic[row];
			//find the symbol character
			size_t pos = row_string.find_first_not_of( ".0123456789" );
			while ( pos != std::string::npos )
			{
				//process the symbol position to find any neighbouring parts
				//iterate over row above and below
				for ( int i = 0; i < 3; ++i )
				{
					std::vector<uint32_t> part_ids = get_parts_on_row_for_symbol_at_pos( static_cast<uint32_t>(pos), row - 1 + i, parts );
					sum_of_part_ids += std::accumulate(part_ids.begin(), part_ids.end(), 0);
				}
				//find next symbol
				pos = row_string.find_first_not_of( ".0123456789", pos+1 );
			}
		}
		return sum_of_part_ids;
	}

	uint32_t part_2( const std::vector<std::string>& schematic, const std::vector<std::vector<Part>>& parts )
	{
		uint32_t sum_of_product_of_part_ids = 0;
		//iterate through line by line looking for gear symbols -- part 2 is looking for gear ratio's
		//once symbol found look in row above, this row and row below for part numbers adjacent to symbol
		for ( int32_t row = 0; row < schematic.size(); ++row )
		{
			//process each row string
			const std::string& row_string = schematic[row];
			//find the gear symbol character
			size_t pos = row_string.find_first_of( '*' );
			while ( pos != std::string::npos )
			{
				std::vector<uint32_t> part_ids = {};
				//process the symbol position to find any neighbouring parts
				//iterate over row above and below
				for ( int i = 0; i < 3; ++i )
				{
					std::vector<uint32_t> row_part_ids = get_parts_on_row_for_symbol_at_pos( static_cast<uint32_t>(pos), row - 1 + i, parts );
					part_ids.insert( part_ids.end(), row_part_ids.begin(), row_part_ids.end() );
					
				}
				if ( part_ids.size() > 1 )
				{
					sum_of_product_of_part_ids += std::accumulate( part_ids.begin(), part_ids.end(), 1, std::multiplies<>() );
				}
				//find next symbol
				pos = row_string.find_first_of( '*', pos + 1 );
			}
		}
		return sum_of_product_of_part_ids;
	}

}

void aoc::day_03()
{
	const std::vector<std::string> schematic = day_03::read_input_file( "./data/day_03_input.txt" );
	const std::vector<std::vector<day_03::Part>> parts = day_03::extract_parts( schematic );

	std::cout << "Part 1: Sum of connected parts: " << day_03::part_1(schematic, parts) << std::endl;
	std::cout << "Part 2: Sum of Gear Ratio Products: " << day_03::part_2( schematic, parts ) << std::endl;

}