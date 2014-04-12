/*
 * photmasstypes.h
 *
 *  Created on: 22/02/2013
 *      Author: felipe
 */

#ifndef PHOTMASSTYPES_H_
#define PHOTMASSTYPES_H_

typedef struct {
   double   time;
   double	magnitude;
   double	velocity;
} Measure_t;

typedef struct MeasureNode
{
	Measure_t data;
	struct MeasureNode *next;

} MeasureNode_t;

#endif /* PHOTMASSTYPES_H_ */
