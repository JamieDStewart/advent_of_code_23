//\==============================================================================
// Advent of code 2023 Day 1
// Question: https://adventofcode.com/2023/day/1
// Gist:  Elves Tebucheting you into sky, calibration values all muddled up in input
// Input data: calibration values mixed with letters
//\==============================================================================


#include "days.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace day_01
{
	void read_input_from_file( std::vector<std::string>& input_vector )
	{
		//open the input file for reading
		std::ifstream file_input;
		file_input.open( "./data/day_01_input.txt" );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					input_vector.push_back( line );
				}

			}
			//close the input file
			file_input.close();
		}
	}

	uint32_t part_1( std::vector<std::string>& input )
	{
		//keep track of the accumulated total for the calibration values
		uint32_t calibration_value_sum = 0;
		uint32_t calibration_value = 0;
		const std::string digits = "0123456789";

		for ( const auto& line : input )
		{
			//find the first integer value in the string
			const size_t first = line.find_first_of( digits );
			const size_t last = line.find_last_of( digits );
			if ( first != std::string::npos )
			{
				const std::string calibration_value_string = std::string( line, first, 1 ) + std::string( line, last, 1 );
				std::stringstream ss( calibration_value_string );
				ss >> calibration_value;
				calibration_value_sum += calibration_value;
			}
		}

		return calibration_value_sum;
	}

	uint32_t part_2( std::vector<std::string>& input )
	{
		struct value_pos
		{
			std::string value;
			size_t pos;
		};

		//keep track of the accumulated total for the calibration values
		uint32_t calibration_value_sum = 0;
		uint32_t calibration_value = 0;
		const std::string digits = "0123456789";

		const std::vector<std::string> search_strings = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
		const std::vector<std::string> search_values = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

		for ( const auto& line : input )
		{
			//find the positions of the first integer value in the string
			const size_t first = line.find_first_of( digits );
			const size_t last = line.find_last_of( digits );
			//search the string and find occurrences of the number strings
			std::vector<value_pos> locations;
			for ( int i = 0; i < search_strings.size(); ++i )
			{
				size_t offset = 0;
				size_t location = 0;
				//perform this as a while loop so that strings containing multiple sub stings of number are all accounted for eg. two43two
				while ( location != std::string::npos )
				{
					location = line.find( search_strings[i], location + offset );
					if ( location != std::string::npos )
					{
						offset = 1;
						locations.push_back( { search_values[i], location } );
					}
				}
			}
			//sort locations based on position in string
			std::sort( locations.begin(), locations.end(), []( const value_pos a, const value_pos b )
					   {
						   return (a.pos < b.pos);
					   } );

			std::string first_digit = (!locations.empty() && locations[0].pos < first) ? locations[0].value : std::string( line, first, 1 );
			std::string last_digit = (last == std::string::npos || (!locations.empty() && locations[locations.size() - 1].pos > last)) ? locations[locations.size() - 1].value : std::string( line, last, 1 );

			std::stringstream ss( first_digit + last_digit );
			ss >> calibration_value;
			calibration_value_sum += calibration_value;
		}

		return calibration_value_sum;
	}
}

void aoc::day_01()
{
	//vector to store the total quantity of calories that an elf is carrying
	std::vector< std::string > input_values;
	//read file input
	day_01::read_input_from_file( input_values );

	const uint32_t part_1_calibration_values_sum = day_01::part_1( input_values );
	std::cout << "Part 1: Calibration Values Sum = " << part_1_calibration_values_sum << std::endl;
	const uint32_t part_2_calibration_values_sum = day_01::part_2( input_values );
	std::cout << "Part 2: Calibration Values Sum = " << part_2_calibration_values_sum << std::endl;
}
