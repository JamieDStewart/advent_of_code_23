//\==============================================================================
// Advent of code 2023 Day 20
// Question: https://adventofcode.com/2023/day/20
// Gist: Part Electrical pulses, press a button and a circuit sends out pulses
// Input data: a circuit with a list of nodes and node types
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
#include <string>
#include <sstream>
#include <vector>

namespace day_20
{
	struct Module
	{
		std::string name;
		std::vector<std::string> destination_modules;
		std::vector<std::pair<std::string, uint32_t>> connected_input_modules;
		uint32_t state;
		char type;
	};

	std::map<std::string, Module> read_input_file( const std::string& input_file )
	{
		std::map<std::string, Module> configuration;
		
		std::ifstream file_input;
		file_input.open( input_file );
		if ( file_input.is_open() )
		{
			bool processing_parts = false;
			for ( std::string line; std::getline( file_input, line ); )
			{
				if ( line.length() > 0 )
				{
					//get the name and type of the module
					const auto module_name_end = line.find_first_of( ' ' );
					std::string name = line.substr( 0, module_name_end );
					const char type = name[0];
					if( type == '&' || type == '%' )
					{
						//strip off type from name
						name.erase( 0, 1 );
					}
					//get the destination modules from the line
					const auto destination_location = line.find_first_of( '>' );
					std::string destination_modules = line.substr( destination_location+1 );
					//replace the ',' with space to make using stringstream more simple
					std::ranges::replace( destination_modules.begin(), destination_modules.end(), ',', ' ' );
					std::stringstream ss( destination_modules );
					std::string destination_name = {};
					std::vector<std::string> destinations;
					while( ss >> destination_name )
					{
						destinations.push_back( destination_name );
					}
					configuration[name] = { name, destinations, {}, 0, type };
				}				
			}
			file_input.close();
		}
		// need to set up all input modules on conjunction modules
		std::vector<Module*> conjunction_modules = {};
		for( auto& module : std::views::values(configuration) )
		{
			if( module.type == '&' )
			{
				conjunction_modules.push_back( &module );
			}
		}
		//for each conjunction module go through all modules and find those that reference it as an destination
		for( auto conjunction : conjunction_modules )
		{
			std::string id = conjunction->name;
			for ( auto& module : std::views::values( configuration ) )
			{
				for( auto& destination : module.destination_modules )
				{
					if( destination == id )
					{
						conjunction->connected_input_modules.emplace_back( std::make_pair( module.name, 0 ) );
					}
				}
			}
		}
		return configuration;
	}

	std::vector<std::pair<Module*, std::pair<uint32_t, std::string>>> process_signal( Module* module, const uint32_t pulse, const std::string& sender, std::map<std::string, Module>& configuration )
	{
		std::vector<std::pair<Module*, std::pair<uint32_t, std::string>>> output = {};
		uint32_t out_pulse = 0;
		
		if( module->type == '%' )
		{
			if( pulse != 0 )
			{
				return output;
			}
			
			out_pulse = (module->state == 0) ? 1 : 0;
			module->state = (module->state == 0) ? 1 : 0;
			
		}
		else if( module->type == '&' )
		{
			uint32_t pulses_received = 0;
			for( auto& input_value : module->connected_input_modules )
			{
				if ( input_value.first == sender )
				{
					input_value.second = pulse;
				}
				pulses_received += input_value.second;
			}
			//if all high send low pulse, else send high pulse
			out_pulse = (pulses_received == module->connected_input_modules.size()) ? 0 : 1;
		}

		for ( const auto& id : module->destination_modules )
		{
			Module* destination = &configuration[id];
			output.emplace_back( std::make_pair( destination, std::make_pair( out_pulse, module->name ) ) );
		}

		return output;
	}

	uint64_t state_value( std::map<std::string, Module>& configuration )
	{
		int64_t state = 0;
		for ( auto& module : std::views::values( configuration ) )
		{
			if( module.type == '%' )
			{
				state += module.state;
			}
			if( module.type == '&' )
			{
				for ( const auto input_value : std::views::values( module.connected_input_modules) )
				{
					state += input_value;
				}
			}
		}
		return state;
	}


	uint64_t part_01( std::map<std::string, Module>& configuration, uint64_t button_presses )
	{
		//the initial state of the configuration all flip_flops are off 0 and all conjunctions have low/off state
		//so adding up all states and conjunctions should equal 0 when back to equilibrium.
		uint64_t low_pulses = 0;
		uint64_t high_pulses = 0;
		uint64_t loop_counter = 0;
		const uint64_t initial_state = state_value( configuration );
		do
		{
			std::vector<std::pair<Module*, std::pair<uint32_t, std::string>>> active_modules = { { &configuration["broadcaster"], {0, "button"}} };
			while ( !active_modules.empty() )
			{
				//perform operation on first active module
				Module* active_module = active_modules[0].first;
				uint32_t pulse_recieved = active_modules[0].second.first;
				std::string sender = active_modules[0].second.second;

				//increment low or high pulses received accordingly
				if ( pulse_recieved == 0 ) { ++low_pulses; }
				else { ++high_pulses; }

				std::vector<std::pair<Module*, std::pair<uint32_t, std::string>>> destinations = process_signal( active_module, pulse_recieved, sender, configuration );
				//add the destinations to the active modules
				active_modules.insert( active_modules.end(), destinations.begin(), destinations.end() );
				active_modules.erase( active_modules.begin() );
			}
			++loop_counter;
		}while ( state_value( configuration ) != initial_state && loop_counter != button_presses);

		return (low_pulses * (button_presses/ loop_counter)) * (high_pulses * (button_presses/ loop_counter));
	}

	void reset_state( std::map<std::string, Module>& configuration )
	{
		for ( auto& module : std::views::values( configuration ) )
		{
			if ( module.type == '%' )
			{
				module.state = 0;
			}
			if ( module.type == '&' )
			{
				for ( auto& input :  module.connected_input_modules )
				{
					input.second = 0;
				}
			}
		}
	}

	uint64_t part_02( std::map<std::string, Module>& configuration )
	{
		reset_state( configuration );
		//the initial state of the configuration all flip_flops are off 0 and all conjunctions have low/off state
		//how many button presses need to happen to send a low pulse value to rx;
		uint64_t low_pulses = 0;
		uint64_t high_pulses = 0;
		uint64_t button_presses = 1;
		//rx is attached to conjunction dr, dr is attached to conjunctions mp, qt, qb, and ng
		//to send a low signal dr must have 4 high pulses set for these attached objects
		//conjunctions need to find lowest common multiple to find this one
		std::map<std::string, uint64_t> dr_inputs = { {"mp", 0}, {"qt",0}, {"qb",0}, {"ng",0}};
		bool should_continue = true;
		while(  should_continue )
		{
			std::vector<std::pair<Module*, std::pair<uint32_t, std::string>>> active_modules = { { &configuration["broadcaster"], {0, "button"}} };
			while ( !active_modules.empty() )
			{
				//perform operation on first active module
				Module* active_module = active_modules[0].first;
				const uint32_t pulse_received = active_modules[0].second.first;
				std::string sender = active_modules[0].second.second;

				if( active_module->name == "dr" && pulse_received == 1 )
				{					
					if ( dr_inputs[sender] == 0 )
					{
						should_continue = false;
						dr_inputs[sender] = button_presses;
						for ( const auto input : std::views::values( dr_inputs ) )
						{
							if ( input == 0 )
							{
								should_continue = true;
								break;
							}
						}
					}					
					if ( !should_continue ) { break; }
				}
				//increment low or high pulses received accordingly
				if ( pulse_received == 0 ) { ++low_pulses; }
				else { ++high_pulses; }

				std::vector<std::pair<Module*, std::pair<uint32_t, std::string>>> destinations = process_signal( active_module, pulse_received, sender, configuration );
				//add the destinations to the active modules
				active_modules.insert( active_modules.end(), destinations.begin(), destinations.end() );
				active_modules.erase( active_modules.begin() );
			}
			++button_presses;
		}

		//all input values for calling rx have been received find LCM
		uint64_t lcm = 1;
		for ( auto input : std::views::values( dr_inputs ) )
		{
			lcm *= input;
		}
		
		return lcm;
	}


}

Result aoc::day_20()
{
	timer::start();
	std::map<std::string, day_20::Module> configuration = day_20::read_input_file( "./input/day_20.txt" );

	const uint64_t part_1_answer = day_20::part_01( configuration, 1000LLU );

	const uint64_t part_2_answer = day_20::part_02( configuration );

	timer::stop();
	return { std::string( "20: Circuit Pulses" ), part_1_answer, part_2_answer, timer::get_elapsed_seconds() };

}