//\==============================================================================
// Advent of code 2023 Day 1
// Question: https://adventofcode.com/2023/day/1
// Gist:  Elves Tebucheting you into sky, calibration values all muddled up in input
// Input data: calibration values mixed with letters
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/*
 * Creating a namespace in this file to add in any functions used in solving the problem
 * I tend to use a mixture of C/C++ styling when solving Advent of code problems as it's
 * easy for me to work this way and keep each problem in a separate file without worrying
 * about any function name clashes between days.
 */
namespace day_01
{
	/**
	/* @brief a function to read input from file and store each input line as a string in a vector
	 * @param input_file the directory path and name of the file to read from
	*/
	std::vector<std::string> read_input_from_file( const std::string& input_file )
	{
		std::vector<std::string> input_vector = {};
		//open the input file for reading
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					input_vector.push_back( line );
				}

			}
			file_input.close();
		}
		return input_vector;
	}

	/**
	 * @brief function to solve part 1 of the problem for day 1.
	 *        Solution requires finding the first and last digit in the string
	 *        and adding these to a running total 
	 * @param input a vector of strings that contain numerical digits
	 * @return the sum total of all combined first & last digits in the input string vector
	 */
	uint32_t part_1( std::vector<std::string>& input )
	{
		//keep track of the accumulated total for the calibration values
		uint32_t calibration_value_sum = {};
		uint32_t calibration_value = {};
		const std::string digits = "0123456789";

		for ( const auto& line : input )
		{
			const size_t first = line.find_first_of( digits );		//find the first integer value in the string
			const size_t last = line.find_last_of( digits );			//find the last integer value in the string

			const std::string calibration_value_string = std::string( line, first, 1 ) + std::string( line, last, 1 );
			std::stringstream ss( calibration_value_string );
			ss >> calibration_value;
			calibration_value_sum += calibration_value;
			
		}
		return calibration_value_sum;
	}

	/**
	 * @brief Function to solve part 2 of the question
	 *        This time the input needs to be scanned for the written number values as well as the decimal values
	 * @param input vector of strings that contain written and decimal number values
	 * @return the sum total of all combined first and last numbers found in each input string from the input vector
	 */
	uint32_t part_2( std::vector<std::string>& input )
	{
		const std::vector<std::string> search_strings = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
		const std::vector<char> search_values = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };

		for ( auto& line : input )
		{
			uint32_t index = 0;
			for ( auto& number : search_strings )
			{
				size_t location = line.find( number );
				while( location != std::string::npos )
				{
					//replace the second character of this string with digit for that string
					line[location+1] = search_values[index];
					location = line.find( number, location+1 );
				}
				++index;
			}			
		}

		return part_1(input );
	}
}

Result aoc::day_01()
{
	timer::start();
	//vector to store the total quantity of calories that an elf is carrying
	std::vector< std::string > input_values = day_01::read_input_from_file( "./input/day_01.txt" );

	const uint32_t part_1_calibration_values_sum = day_01::part_1( input_values );
	const uint32_t part_2_calibration_values_sum = day_01::part_2( input_values );
	timer::stop();

	return { std::string(" 1: Trebuchet?!"), part_1_calibration_values_sum, part_2_calibration_values_sum, timer::get_elapsed_seconds()};
}
