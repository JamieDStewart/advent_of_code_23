//\==============================================================================
// Advent of code 2023 Day 9
// Question: https://adventofcode.com/2023/day/9
// Gist:  Use your OASIS tool to process environmental readings, get next item in sequence
// Input data: Series of values from oasis tool
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace day_09
{
	struct Reading
	{
		std::vector<int64_t> reading;
		std::vector<std::vector<int64_t>> differences;
	};

	std::vector<Reading> read_input_file( const std::string& input_file )
	{
		std::vector<Reading> readings = {};
		std::string directions;
		std::map<std::string, std::pair<std::string, std::string>> parent_children;
		//file is a series of lines made up of hand info and bit amount
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			//The first line is the directions
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//each line is a series of reasings simply add to a vector and pass into readings vector
					std::vector<int64_t> reading = {};
					std::stringstream ss( line );
					int64_t value = {};
					while( ss >> value )
					{
						reading.push_back( value );
					}
					//generate differences for each reading vector
					std::vector<std::vector<int64_t>> differences = {};
					std::vector<int64_t> difference = reading;
					int64_t accumulation_value = 0;
					do
					{
						//use std::adjacent_difference to get the differences and substitute these into the current vector
						std::adjacent_difference( difference.begin(), difference.end(), difference.begin() );
						difference.erase( difference.begin() );
						differences.push_back( difference );
						accumulation_value = std::accumulate( difference.begin(), difference.end(), 0LL );

					}
					while ( accumulation_value != 0 );

					readings.push_back( { reading, differences } );
				}

			}
			file_input.close();
		}
		return readings;
	}


	uint64_t part_01( std::vector<Reading>& readings )
	{
		//for each set of readings we need to get the next value in the sequence then accumulate these values
		std::vector<int64_t> next_values_in_sequence = {};
		for ( auto& [reading, differences] : readings )
		{
			//we've now got the differences down to zero, so add the final value onto all final values
			int64_t next_value_in_sequence = 0;
			for( const auto& diff : differences )
			{
				next_value_in_sequence += diff[diff.size() - 1];
			}
			next_values_in_sequence.push_back( next_value_in_sequence + reading[reading.size()-1]);
		}
		return std::accumulate(next_values_in_sequence.begin(), next_values_in_sequence.end(), 0LL );
	}

	uint64_t part_02( std::vector<Reading>& readings )
	{
		//for each set of readings we need to get the previous value in the sequence then accumulate these values
		std::vector<int64_t> next_values_in_sequence = {};
		for ( auto& [reading, differences] : readings )
		{
			//we've now got the differences down to zero, iterate over the differences in reverse and subtract them from the initial difference value
			int64_t next_value_in_sequence = 0;
			for ( auto iter = differences.rbegin(); iter != differences.rend(); ++iter )
			{
				next_value_in_sequence = (*iter)[0] - next_value_in_sequence;
			}
			next_values_in_sequence.push_back( reading[0] - next_value_in_sequence );
		}
		return std::accumulate( next_values_in_sequence.begin(), next_values_in_sequence.end(), 0LL );
	}


}

Result aoc::day_09()
{
	timer::start();
	std::vector<day_09::Reading> readings = day_09::read_input_file( "./input/day_09.txt" );

	const uint64_t part_1_answer = day_09::part_01( readings );
	const uint64_t part_2_answer = day_09::part_02( readings );

	timer::stop();
	return { std::string( " 9: OASIS Predictions" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}