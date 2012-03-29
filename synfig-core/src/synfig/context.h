/* === S Y N F I G ========================================================= */
/*!	\file context.h
**	\brief Iterator for the layers behind the current Layer.
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === S T A R T =========================================================== */

#ifndef __SYNFIG_CONTEXT_H
#define __SYNFIG_CONTEXT_H

/* === H E A D E R S ======================================================= */

#include "canvasbase.h"
#include "rect.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfig {

class Vector;
typedef Vector Point;
class Color;
class Surface;
class RendDesc;
class ProgressCallback;
class Layer;
class Time;
class Rect;

/*!	\class Context
**	\brief Context is a class to warp the iterator for a double queue of layers
* (that is the CanvasBase).
**	\see Layer, Canvas, CanvasBase */
class Context : public CanvasBase::const_iterator
{
public:
	Context() { }

	//! Constructor based on other CanvasBase iterator
	Context(const CanvasBase::const_iterator &x):CanvasBase::const_iterator(x) { }

	//!Assignation operator
	Context operator=(const CanvasBase::const_iterator &x)
	{ return CanvasBase::const_iterator::operator=(x); }

	//!	Returns the color of the context at the Point \pos.
	//! It is the blended color of the context
	Color get_color(const Point &pos)const;

	//!	With a given \quality and a given render description it puts the context
	//! blend result into the painting \surface */
	bool accelerated_render(Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb) const;

	//! Sets the context to the Time \time. It is done recursively.
   	void set_time(Time time)const;

	//!	Sets the context to the Time \time. It is done recursively. Vector \pos is not used
   	void set_time(Time time,const Vector &pos)const;

	//! Returns the bounding rectangle of all the context.
	//! It is the union of all the layers's bounding rectangle.
	Rect get_full_bounding_rect()const;

	//! Returns the first context's layer's handle that intesects the given \point */
	etl::handle<Layer> hit_check(const Point &point)const;

	//! Sets dirty (dirty_time_= Time::end()) to all Outline type layers
	void set_dirty_outlines();

}; // END of class Context

}; // END of namespace synfig

/* === E N D =============================================================== */

#endif
