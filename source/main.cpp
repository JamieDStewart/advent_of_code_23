

#include <iostream>
#include <sstream>
#include <vector>

#include "days.h"
#include "result.h"


std::vector< Result( * )() > questions = { aoc::day_01, aoc::day_02, aoc::day_03, aoc::day_04, aoc::day_05, aoc::day_06,
											aoc::day_07, aoc::day_08, aoc::day_09, aoc::day_10, aoc::day_11, aoc::day_12,
											aoc::day_13, aoc::day_14, aoc::day_15, aoc::day_16, aoc::day_17, aoc::day_18,
aoc::day_19};

void print_break_to_console()
{
	for ( int i = 0; i < 7; ++i )
	{
		std::cout << "==========";
	}
	std::cout << std::endl;
}

void print_x_spaces_console(int x)
{
	for( int i = 0; i < x; ++i)
	{
		std::cout << ' ';
	}
}

void print_separator()
{
	std::cout << "||";
}

void print_heading_to_console()
{
	print_separator();
	print_x_spaces_console( 23 );
	std::cout << "Advent of Code 2023";
	print_x_spaces_console( 24 );
	print_separator();
	std::cout << std::endl;
	print_break_to_console();
	print_separator();
	std::cout << "Day";
	print_x_spaces_console( 20 );
	print_separator();
	std::cout << "Part 1";
	print_x_spaces_console( 6 );
	print_separator();
	std::cout << "Part 2";
	print_x_spaces_console( 9 );
	print_separator();
	std::cout << "Time";
	print_x_spaces_console( 6 );
	print_separator();
	std::cout << std::endl;
	print_break_to_console();	
}

int get_spaces( uint64_t value )
{
	//get length of result digits using string stream ops
	std::string res_str;
	std::stringstream ss;
	ss << value;
	ss >> res_str;
	return static_cast<int>(res_str.size());
}

int get_spaces( double value )
{
	//get length of result digits using string stream ops
	std::string res_str;
	std::stringstream ss;
	ss << value;
	ss >> res_str;
	return static_cast<int>(res_str.size());
}

void print_result(const Result& result )
{
	print_separator();
	std::cout << result.name;
	print_x_spaces_console( 23 - static_cast<int>(result.name.size()) );
	print_separator();
	std::cout << result.part_1;
	print_x_spaces_console( 12 - get_spaces(result.part_1) );
	print_separator();
	std::cout << result.part_2;
	print_x_spaces_console( 15 - get_spaces( result.part_2 ) );
	print_separator();
	std::cout << result.execution_time;
	print_x_spaces_console( 10 - get_spaces( result.execution_time ) );
	print_separator();
}

int main( int argc, char* argv[] )
{
	//Create a vector to store the results for each day's solutions
	std::vector<Result> results = {};

	for( const auto& question : questions )
	{
		results.push_back( question() );
	}

	//display results to console
	print_break_to_console();
	//print headings
	print_heading_to_console();
	for( const auto& result : results )
	{
		print_result( result );
		std::cout << std::endl;
	}
	print_break_to_console();


	
	return 0;
}
