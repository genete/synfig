/* === S Y N F I G ========================================================= */
/*!	\file synfig/renderer_software.h
**	\brief Template Header
**
**	$Id$
**
**	\legal
**	......... ... 2014 Ivan Mahonin
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

#ifndef __SYNFIG_RENDERER_SOFTWARE_H
#define __SYNFIG_RENDERER_SOFTWARE_H

/* === H E A D E R S ======================================================= */

#include "renderer.h"
#include "surface.h"

/* === M A C R O S ========================================================= */

/* === T Y P E D E F S ===================================================== */

/* === C L A S S E S & S T R U C T S ======================================= */

namespace synfig
{

class RendererSoftware;

template<>
class Renderer::TypesTemplate<RendererSoftware, Renderer::PrimitiveTypeSurface>:
	public Renderer::TypesTemplateBase<synfig::Surface> { };


class RendererSoftware: public Renderer {
private:
	static RendererId id;
public:
	typedef RendererSoftware RendererType;
	typedef Renderer::TypesBase<RendererType> Types;

	static RendererId get_id();
	static void initialize();
	static void deinitialize();

	RendererSoftware();
	virtual Result render_surface(const Params &params, const Primitive<PrimitiveTypeSurface> &primitive);
	virtual Result render_polygon(const Params &params, const Primitive<PrimitiveTypePolygon> &primitive);
	virtual Result render_colored_polygon(const Params &params, const Primitive<PrimitiveTypeColoredPolygon> &primitive);
	virtual Result render_mesh(const Params &params, const Primitive<PrimitiveTypeMesh> &primitive);
};

}; /* end namespace synfig */

/* -- E N D ----------------------------------------------------------------- */

#endif
