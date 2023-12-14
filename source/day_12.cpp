//\==============================================================================
// Advent of code 2023 Day 12
// Question: https://adventofcode.com/2023/day/12
// Gist:  Springs that work or don't, a #?. sequence permutation
// Input data: A series of springs and '.' work, '#' are broken, '?' who knows and sequences 1, 1, 3 etc
//\==============================================================================


#include "days.h"
#include "timer.h"
#include "result.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <sstream>
#include <vector>

namespace day_12
{
	struct Spring_Group
	{
		std::string springs;
		std::vector<uint32_t> groups;
	};

	std::vector<Spring_Group> read_input_file( const std::string& input_file )
	{
		std::vector<Spring_Group> spring_groups;
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//find the space to separate on
					size_t space = line.find_first_of( ' ' );
					std::string springs = line.substr(0, space);
					//get the grouping values
					std::string grouping = line.substr( space + 1, line.size() - space );
					std::replace( grouping.begin(), grouping.end(), ',', ' ' );
					std::vector<uint32_t> groups = {};
					std::stringstream ss( grouping );
					uint32_t value = {};
					while( ss >> value )
					{
						groups.push_back( value );
					}
					spring_groups.push_back( { springs, groups } );
				}
			}			
			file_input.close();
		}
		return spring_groups;
	}

	//forward declarations
	uint64_t process_pattern( std::string springs, const std::vector<uint32_t>& groups, std::map< uint64_t, uint64_t>&  cached_values );

	enum class Spring_types : uint8_t
	{
		good = '.',
		bad = '#',
		unknown = '?'
	};

	uint64_t process_spring( std::string springs, const std::vector<uint32_t>& groups, std::map< uint64_t, uint64_t>&  cached_values )
	{
		//consume the first character of the string and continue
		springs = springs.substr( 1 );
		return process_pattern( springs, groups, cached_values );
	}

	uint64_t process_unknown( const std::string& springs, const std::vector<uint32_t>& groups, std::map< uint64_t, uint64_t>&  cached_values )
	{
		//consume the first character of the string and continue
		return process_pattern( '.' + springs.substr( 1 ), groups, cached_values ) + 
			process_pattern( '#' + springs.substr( 1 ), groups, cached_values );
	}

	uint64_t process_broken_spring( std::string springs, std::vector<uint32_t> groups, std::map< uint64_t, uint64_t>&  cached_values )
	{
		//take the group number of contiguous bad springs and remove that many dead springs
		if ( groups.empty() ) { return 0; }
		const auto count = groups[0];
		//remove this group
		groups.erase( groups.begin() );
		uint64_t total = {};
		if ( springs.size() >= count )
		{
			total = std::count_if( springs.begin(), springs.begin() + count, []( const char& c )
								   {
									   return '#' == c || '?' == c;
								   } );
		}
		if( total < count )
		{
			//not enough dead springs to satisfy
			return 0;
		}
		else if( springs.length() == count )
		{
			//nothing left in this sequence of springs
			return process_pattern( "", groups, cached_values );
		}
		else if( springs[count] == '#' )
		{
			//dead spring follows sequence not allowed
			return 0;
		}
		else
		{
			return process_pattern( springs.substr( count + 1 ), groups, cached_values );
		}

	}


	uint64_t process_pattern( std::string springs, const std::vector<uint32_t>& groups, std::map< uint64_t, uint64_t>&  cached_values)
	{
		if ( springs.empty() )
		{
			//we have exhausted this string if we used up all the groupings then this is good
			return groups.empty() ? 1 : 0;
		}
		//make hash key
		std::stringstream ss = {};
		ss << springs << springs.length() << groups.size();
		const std::string key = ss.str();
		const uint64_t hash_key = std::hash<std::string>{}(key);
		if( const auto search = cached_values.find( hash_key ); search == cached_values.end() )
		{
			

			switch ( auto st = static_cast<Spring_types>(springs[0]); st )
			{
			case Spring_types::good:
				cached_values[hash_key] = process_spring( springs, groups, cached_values ); break;
			case Spring_types::bad:
				cached_values[hash_key] =  process_broken_spring( springs, groups, cached_values ); break;
			case Spring_types::unknown:
				cached_values[hash_key] =  process_unknown( springs, groups, cached_values ); break;
			}
		}
		return cached_values[hash_key];		
	}

	uint64_t part_01( const std::vector<Spring_Group>& spring_groups )
	{
		std::vector<uint64_t> possibilities = {};
		
		for( const auto& [spring, group] : spring_groups )
		{
			std::map< uint64_t, uint64_t> cached_values = {};
			possibilities.push_back( process_pattern(spring, group, cached_values ) );
		}

		return std::accumulate(possibilities.begin(), possibilities.end(), 0LLU );
	}
	
	uint64_t part_02( const std::vector<Spring_Group>& spring_groups )
	{
		//unwrap the springs group
		std::vector<Spring_Group> unwrapped = spring_groups;
		for ( auto& [spring, group] : unwrapped )
		{
			auto duplication = spring;
			auto dupe_group = group;
			for( int i = 0; i < 4; ++i )
			{
				spring.append( "?" );
				spring.append( duplication );
				group.insert( group.end(), dupe_group.begin(), dupe_group.end() );
			}
		}
		std::vector<uint64_t> possibilities = {};
		
		for ( const auto& [spring, group] : unwrapped )
		{
			std::map< uint64_t, uint64_t> cached_values = {};
			possibilities.push_back( process_pattern( spring, group, cached_values ) );
		}

		return std::accumulate( possibilities.begin(), possibilities.end(), 0LLU );
		return 0LLU;
	}


}

Result aoc::day_12()
{
	timer::start();
	const std::vector< day_12::Spring_Group> spring_groups = day_12::read_input_file( "./data/day_12_input.txt" );

	const uint64_t part_1_answer = day_12::part_01( spring_groups );

	const uint64_t part_2_answer = day_12::part_02( spring_groups );

	timer::stop();
	return { std::string( "12: Spring Permutations" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}