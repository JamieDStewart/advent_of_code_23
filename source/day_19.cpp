//\==============================================================================
// Advent of code 2023 Day 19
// Question: https://adventofcode.com/2023/day/19
// Gist: Part sorting state machine
// Input data: A list of workflows followed by part ratings {xmas}
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
#include <vector>

namespace day_19
{
	struct Part
	{
		int32_t value[4];
	};

	int32_t get_part_index( const char c )
	{
		switch( c )
		{
		case 'x': return 0;
		case 'm': return 1;
		case 'a': return 2;
		default: return 3;
		}
	}

	bool less_than( const int32_t a, const int32_t b )
	{
		return a < b;
	}

	bool more_than( const int32_t a, const int32_t b )
	{
		return a > b;
	}

	struct Workflow
	{
		std::vector<bool(*)(const int32_t, const int32_t)> operators;
		std::vector<std::string> op_results;
		std::vector<int32_t> part_index;
		std::vector<int32_t> value;
	};

	struct Workflows_Parts
	{
		std::map<std::string, Workflow> workflows;
		std::vector<Part> parts;
	};



	std::string process_workflow_string( const std::string& line, Workflow& workflow )
	{
		const size_t brace_pos = line.find( '{' );
		std::string key = line.substr( 0, brace_pos );
		//now get workflow
		
		std::string wf_str = line.substr( brace_pos+1, line.length()- (brace_pos + 2) );
		// read to first ':' to get operator
		size_t cp = 0;
		size_t op_end_pos = wf_str.find( ':');
		while( op_end_pos != std::string::npos )
		{
			std::string op = wf_str.substr( cp, op_end_pos - cp );
			int32_t part_index = get_part_index( op[0] );
			bool (*operation)(const int32_t a, const int32_t b);
			operation = (op[1] == '<') ? less_than : more_than;
			int32_t value = {};
			std::stringstream( op.substr( 2 ) ) >> value;
			// now read in next portion of string to next comma
			size_t comma_loc = wf_str.find( ',', op_end_pos );
			std::string result = wf_str.substr( op_end_pos + 1, comma_loc - (op_end_pos+1) );
			workflow.operators.push_back( operation );
			workflow.op_results.push_back( result );
			workflow.part_index.push_back( part_index );
			workflow.value.push_back( value );
			cp = comma_loc + 1;
			op_end_pos = wf_str.find( ':', cp );			
		}
		workflow.op_results.push_back( wf_str.substr( cp ) );
		return key;
	}

	Part extract_part_data( std::string& part_line )
	{
		//replace all non numeric chars with spaces
		std::ranges::for_each( part_line.begin(), part_line.end(), []( char& c )
							   {
								   if ( std::string( "{xmas=,}" ).find( c ) != std::string::npos )
								   {
									   c = ' ';
								   }
							   } );
		std::stringstream ss( part_line );
		Part p = { 0,0,0,0 };
		for( int i = 0; i < 4; ++i )
		{
			ss >> p.value[i];
		}
		return p;
	}

	Workflows_Parts read_input_file( const std::string& input_file )
	{
		//set up containers for workflows and parts
		std::map<std::string, Workflow> workflows;
		std::vector<Part> parts;

		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			bool processing_parts = false;
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					if( !processing_parts )
					{
						//read in workflow
						Workflow workflow = {};
						std::string key = process_workflow_string( line, workflow );
						workflows[key] = workflow;
					}
					else
					{
						Part part = extract_part_data( line );
						parts.push_back( part );
					}
				}
				else
				{
					processing_parts = true;
				}
			}
			file_input.close();
		}
		return {workflows, parts};
	}

	std::string process_workflow( const Part& part, const Workflow& workflow )
	{
		for ( auto i = 0; i < workflow.operators.size(); ++i )
		{
			const auto part_index = workflow.part_index[i];
			if ( workflow.operators[i]( part.value[part_index], workflow.value[i] ) )
			{
				return workflow.op_results[i];
			}
		}
		//final result
		const auto result_index = workflow.op_results.size() - 1;
		return workflow.op_results[result_index];
	}

	
	uint64_t part_01( Workflows_Parts& instructions )
	{
		//process each part starting at workflow location 'in'
		//parts that are accepted get added to the accepted list, rejected to the rejected list
		auto& [workflows, parts] = instructions;
		std::vector<Part> accepted = {};
		std::vector<Part> rejected = {};

		for( const auto& part : parts )
		{
			std::string workflow_id = "in";
			while ( workflow_id != "A" && workflow_id != "R" )
			{
				workflow_id = process_workflow( part, workflows[workflow_id] );
			}
			if ( workflow_id == "A" )
			{
				accepted.push_back( part );
			}
			else
			{
				rejected.push_back( part );
			}

		}
		return  std::accumulate( accepted.begin(), accepted.end(), 0, []( int64_t total, Part& p )
												 {
													 return total + p.value[0] + p.value[1] + p.value[2] + p.value[3];
												 } );
	}

	struct part_range
	{
		int32_t lower[4];
		int32_t upper[4];
	};

	struct work_node
	{
		std::string workflow_id;
		part_range part_values;
	};

	std::vector<work_node> process_workflow_potential(  Workflow& workflow, part_range pr )
	{
		std::vector<work_node> work_nodes = {};
		for ( auto i = 0; i < workflow.operators.size(); ++i )
		{
			const auto p_i = workflow.part_index[i];
			const auto p_v = workflow.value[i];
			
			//work out where to split the range or reject if value is outside range bounds
			if( pr.lower[p_i] < p_v && pr.upper[p_i] < p_v ) //both values lower than limit send through range unmodified
			{
				//test with upper value
				if ( workflow.operators[i]( pr.upper[p_i], p_v ) )
				{
					//passed move part range onto next operator
					work_nodes.push_back( { workflow.op_results[i], pr } );
				}
			}
			else if ( pr.lower[p_i] > p_v && pr.upper[p_i] > p_v ) //both values lower than limit send through range unmodified
			{
				//test with lower value
				if ( workflow.operators[i]( pr.lower[p_i], p_v ) )
				{
					//passed move part range onto next operator
					work_nodes.push_back( { workflow.op_results[i], pr } );
				}
			}
			else
			{
				//value falls within range split range
				//test with lower range if success then split and output lower half of range else split and output upper 
				part_range mod_pr = pr;
				if( const bool use_lower_portion_or_range = workflow.operators[i]( pr.lower[p_i], p_v ); use_lower_portion_or_range )
				{
					mod_pr.upper[p_i] = p_v-1;
					pr.lower[p_i] = p_v-1;
					work_nodes.push_back( { workflow.op_results[i], mod_pr } );
				}
				else
				{
					mod_pr.lower[p_i] = p_v;
					pr.upper[p_i] = p_v;
					work_nodes.push_back( { workflow.op_results[i], mod_pr } );
				}
			}
			//work_nodes.push_back( { workflow.op_results[i], pr } );
			
		}
		//final result
		const auto result_index = workflow.op_results.size() - 1;
		work_nodes.push_back( { workflow.op_results[result_index], pr } );
		return work_nodes;
	}

	uint64_t possibilities( part_range p )
	{
		return static_cast<uint64_t>(p.upper[0] - p.lower[0]) * static_cast<uint64_t>(p.upper[1] - p.lower[1]) *
			static_cast<uint64_t>(p.upper[2] - p.lower[2]) * static_cast<uint64_t>(p.upper[3] - p.lower[3]);
	}

	uint64_t part_02( Workflows_Parts& instructions )
	{
		//work out how many potential accepted solutions there are so at each workflow there are i values that succeed or fail
		auto& [workflows, parts] = instructions;
		std::vector<part_range> accepted = {};
		std::vector<part_range> rejected = {};

		//start with the 'in' workflow and call process that will return a list of workflows and their part ranges
		std::vector<work_node> nodes = { { "in", {{0,0,0,0},{4000,4000,4000,4000}}} };
		while( !nodes.empty() )
		{
			std::string wf_id = nodes[0].workflow_id;
			auto pr = nodes[0].part_values;
			uint64_t pr_sum = possibilities( pr );
			if( wf_id == "A" )
			{
				accepted.push_back( pr );
			}
			else if( wf_id == "R" )
			{
				rejected.push_back( pr );
			}
			else
			{
				auto wf = workflows[wf_id];
				auto returned_nodes = process_workflow_potential( wf, pr );
				nodes.insert( nodes.end(), returned_nodes.begin(), returned_nodes.end() );
			
			}
			nodes.erase( nodes.begin() );
		}
		//work out how many potential successes there are
		uint64_t sum = 0;
		for( const auto& a : accepted )
		{
			const uint64_t ranges = possibilities(a);
			sum += ranges;
		}
		return sum;
	}


}

Result aoc::day_19()
{
	timer::start();
	day_19::Workflows_Parts instructions = day_19::read_input_file( "./input/day_19.txt" );

	const uint64_t part_1_answer = day_19::part_01( instructions );

	const uint64_t part_2_answer = day_19::part_02( instructions );

	timer::stop();
	return { std::string( "19: Sorting xmas Parts" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}