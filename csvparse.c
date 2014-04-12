/*
 Name        : csvparse.c
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

 Description : Read the input values to use for calculations from a CSV file.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "csvparse.h"

// Maximum number of characters to read from each line of the CSV file.
const int g_line_size = 250;
// Character separator used by the CSV file.
const char g_field_sep = ',';
// Character used to indicate the decimal numbers.
const int g_decimal_char = '.';
// Number of fields to search.
const int g_number_of_fields = 3;

int valid_characters(char *line)
{
	int valid = 1;
	char *line_pos = NULL;

	for ( line_pos = line; valid && *line_pos != '\n'; line_pos++ ) {
		// Discard header and lines with some character no numeric.
		if ( isalpha(*line_pos) )
			valid = 0;
	}

	return valid;
}

MeasureNode_t *parse_csv_line(char *line)
{
	int i = 0;
	int len = 0;
	double value = 0.0;
	int field_count = 0;
	int field_first_pos = 0;
	MeasureNode_t *new_measure = malloc(sizeof(MeasureNode_t));

	new_measure->next = NULL;

	// Parse the fields from the line received.
	len = strlen(line);
	for ( i = 0 ; i < len ; i++ )
	{
		// Look for separators to identify the fields.
		if ( line[i] == g_field_sep || line[i] == '\n' )
		{
			// The separator is changed by 0 (end of string character),
			// thus the characters for this field can be accessed as a
			// string from the annotated start position of the field.
			line[i] = '\0';

			// Convert to double.
			value = atof(line + field_first_pos);

			switch ( field_count ) {
				case 0:
					new_measure->data.time = value;
					break;
				case 1:
					new_measure->data.magnitude = value;
					break;
				case 2:
					new_measure->data.velocity = value;
					break;
			}

			field_count++;

			field_first_pos = i + 1;
		}
	}

	// Check the number of fields read.
	if ( field_count != g_number_of_fields ) {
		free(new_measure);

		new_measure = NULL;
	}

	return new_measure;
}

MeasureNode_t *read_csv_file(char *cvs_file_name)
{
	FILE *csv_file = NULL;
	char line[g_line_size];
	MeasureNode_t *measures_list = NULL;
	MeasureNode_t *last_measure = NULL;
	MeasureNode_t *new_measure = NULL;

	csv_file = fopen(cvs_file_name, "r");

	while( !feof(csv_file) ) {
		if ( fgets(line, g_line_size, csv_file)  != NULL ) {
			if ( valid_characters(line) ) {
				new_measure = parse_csv_line(line);

				if ( new_measure != NULL ) {
					if ( measures_list == NULL )
						measures_list = new_measure;
					else
						last_measure->next = new_measure;

					last_measure = new_measure;
				}
			}
		}
	}

	fclose(csv_file);

	return measures_list;
}
