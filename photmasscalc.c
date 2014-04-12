/*
 ============================================================================
 Name        : photmasscalc.c
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

 Description : Perform the calculation of the photometric mass of a
 meteoroid from its light curve whose values are received in terms of
 time, magnitude and velocity.
 ============================================================================
 */

#include <stdio.h>
#include <math.h>
#include "photmasscalc.h"

const double VELOCITY_CONVERSION_FACTOR = 10000.0;

/*
 * Calculation of tau. From page 6529 of:
 * Fireball End Heights: A Diagnostic for the Structure of Meteoric Material.
 * Ceplecha, Z., McCrosky, R. E.
 * Journal of Geophysical Research
 *
 * tau follows a formula like:
 * log tau =  a + b * log v
 * and rewritten tau is:
 * tau =  a * exp(10, b) * v
 *
 * v is object velocity in km/s.
 */
double get_tau(double velocity)
{
	// tau is initiated to 1.0 to avoid a division by zero in case
	// the velocity is not in the values taken into account.
	double tau = 1.0;

	if ( velocity <= 9.3 )
		tau = pow(10.0, -12.75);
	else if ( velocity <= 12.5 )
		tau = pow(10.0, - 15.60) * pow(velocity, 2.92);
	else if ( velocity <= 17.0 )
		tau = pow(10.0, - 13.24) * pow(velocity, 0.77);
	else if ( velocity <= 27.0 )
		tau = pow(10.0, - 12.50) * pow(velocity, 0.17);
	else if ( velocity <= 72.0 )
		tau = pow(10.0, - 13.69 ) * velocity;

	return tau;
}

/*
 * Calculation of the luminous intensity from the magnitude.
 * Derived from: M = - 2.5 * log10 ( I )
 */
double get_luminous_intensity(double magnitude)
{
	double exp = magnitude / ( -2.5 );

	return pow(10.0, exp);
}

/*
 * Calculation of differential of time as
 * the difference between the last and first time
 * divided by the number of measures.
 */
double get_dt(MeasureNode_t *measures_list)
{
	MeasureNode_t *next_node = NULL;
	double first_time = 0.0;
	double last_time = 0.0;
	int num_of_measures = 0;
	double dt = 1.0;

	if ( measures_list != NULL )
		first_time = measures_list->data.time;

	while ( measures_list != NULL ) {
		next_node = measures_list->next;

		last_time = measures_list->data.time;

		measures_list = next_node;

		num_of_measures++;
	}

	if ( num_of_measures > 0 )
		dt = ( last_time - first_time ) / (double) num_of_measures;

	return dt;
}

/*
 * Calculation of photometric mass as the integral defined as (2) in:
 * Ceplecha, Z., 1966.
 */
double calculate_phot_mass(MeasureNode_t *measures_list)
{
	double phot_mass = 0.0;
	MeasureNode_t *next_node = measures_list;
	double tau = 0.0, I = 0.0, v = 0.0;

	double dt = get_dt(measures_list);

	while ( next_node != NULL ) {

		tau = get_tau(next_node->data.velocity);
		I = get_luminous_intensity(next_node->data.magnitude);
		v = next_node->data.velocity * VELOCITY_CONVERSION_FACTOR;

		phot_mass += ( 2 / tau ) * ( ( I / pow(v, 3.0)) * dt );

		printf("tau = %e, I = %e, v = %e, phot_mass = %e\n", tau, I, v, phot_mass);

		next_node = next_node->next;
	}

	return phot_mass;
}
