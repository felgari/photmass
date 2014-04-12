/*
 ============================================================================
 Name        : photmass.c
 Author      : Felipe Gallego
 Version     : 1.0
 Copyright   : Copyright (c) 2013 Felipe Gallego. All rights reserved.
 This is free software: you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.

 Description : Calculate the photometric mass of a meteoroid from its light
 curve. The values of the light curve are as time, magnitude and velocity
 from a CSV. These calculations are based in the following papers:

 - Dynamic and photometric mass of meteors.
 Ceplecha, Z.
Bulletin of the Astronomical Institute of Czechoslovakia, vol. 17, p.347

 - Fireball End Heights: A Diagnostic for the Structure of Meteoric Material.
Ceplecha, Z., McCrosky, R. E.
Journal of Geophysical Research
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include "photmasstypes.h"
#include "csvparse.h"
#include "photmasscalc.h"

// Maximum size of the name of the file.
const int g_file_name_max_size = 200;

/*
 * Print to standard output a help message showing the arguments to use
 * when invoking this program.
 *
 * Parameters:
 * - prog_name: Actual name of the program that has been invoked.
 */
void print_help(char *prog_name) {
	printf("Usage: %s -i input_file_name -o output_file_name\n", prog_name);
}

/*
 * Parse program arguments to get the information needed to read the measures
 * to use in calculations.
 */
int parse_program_arguments(int argc, char **argv, char *input_file, char *output_file) {
	int c;
	int exit_status = EXIT_SUCCESS;

	opterr = 0;

	if ( argc > 1 ) {
		while ( ( c = getopt(argc, argv, "i:o:") ) != -1 ) {
			switch (c) {
				case 'i':
					strncpy(input_file, optarg, g_file_name_max_size);
					break;
				case 'o':
					strncpy(output_file, optarg, g_file_name_max_size);
					break;
				case '?':
					if ( optopt == 'i' || optopt == 'o' )
						fprintf(stderr, "Option -%c requires an argument.\n", optopt);
					else if ( isprint(optopt) )
						fprintf(stderr, "Unknown option `-%c'.\n", optopt);
					else
						fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);

					// No break statement needed, as next statement is 'default'.
				default:
					print_help(argv[0]);

					exit_status = EXIT_FAILURE;
					break;
			}
		}
	}
	else {
		print_help(argv[0]);

		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}

/*
 * Print the list of measures.
 */
void show_measures(MeasureNode_t *measures_list)
{
	MeasureNode_t *next_node = measures_list;

	while ( next_node != NULL ) {
		printf("Measure-> Time: %f Magnitude: %f Speed: %f\n",
				next_node->data.time, next_node->data.magnitude, next_node->data.velocity);

		next_node = next_node->next;
	}
}

/*
 * Free memory allocated for the list of measures.
 */
void free_list(MeasureNode_t *measures_list)
{
	MeasureNode_t *next_node = measures_list;

	while ( measures_list != NULL ) {
		next_node = measures_list->next;

		free(measures_list);

		measures_list = next_node;
	}
}

/*
 * Main function to calculate the photometric mass.
 * Perform the following tasks:
 * - Process program arguments to get the source of measures.
 * - Read the measures.
 * - Calculate the photometric mass form measures.
 */
int main(int argc, char **argv) {
	int exit_status = EXIT_SUCCESS;
	// Size plus one for the end of string character.
	char input_file[g_file_name_max_size + 1];
	char output_file[g_file_name_max_size + 1];
	MeasureNode_t *measures_list = NULL;
	double phot_mass = 0.0;

	printf("Starting %s ...\n", argv[0]);

	// Process program arguments.
	if ( parse_program_arguments(argc, argv, input_file, output_file) == 0 ) {

		// Read csv file.
		printf("Opening file %s\n", input_file);
		measures_list = read_csv_file(input_file);

		// Print the measures read from the input file.
		// show_measures(measures_list);

		// Calculation of the photometric mass.
		phot_mass = calculate_phot_mass(measures_list);

		printf("The photometric mass calculated is: %f grams\n", phot_mass);
	}
	else {
		exit_status = EXIT_FAILURE;
	}

	free_list(measures_list);

	printf("Finishing %s ...\n", argv[0]);

	return exit_status;
}
