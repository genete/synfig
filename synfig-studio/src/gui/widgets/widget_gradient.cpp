/* === S Y N F I G ========================================================= */
/*!	\file widget_gradient.cpp
**	\brief Template File
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007 Chris Moore
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

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "widgets/widget_gradient.h"
#include "app.h"
#include <gtkmm/menu.h>
#include <synfig/exception.h>
#include <synfig/surface.h>
#include <ETL/misc>

#include "general.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;
using namespace studio;

/* === M A C R O S ========================================================= */

#define ARROW_NEGATIVE_THRESHOLD 0.4

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

void
studio::render_gradient_to_window(const Glib::RefPtr<Gdk::Drawable>& window,const Gdk::Rectangle& ca,const synfig::Gradient &gradient)
{
	//TODO: those must be custom colors
	synfig::Color dark(0.65, 0.65, 0.65);
	synfig::Color light(0.88, 0.88, 0.88);
	
	Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
	int w=ca.get_width();
	int h=ca.get_height();
	Cairo::RefPtr<Cairo::Surface> grad=Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, w, h);
	Cairo::RefPtr<Cairo::Context> subcr=Cairo::Context::create(grad);
	render_gradient_to_cairo(subcr, Gdk::Rectangle(0, 0, w, h), gradient);
	synfig::gamma_filter(grad->cobj(), App::gamma);
	subcr->set_operator(Cairo::OPERATOR_DEST_OVER);
	render_checkerboard_to_cairo(subcr, Gdk::Rectangle(0, 0, w, h), dark, light);
	cr->set_source(grad, ca.get_x(), ca.get_y());
	cr->paint();
}

void
studio::render_checkerboard_to_cairo(const Cairo::RefPtr<Cairo::Context>& cr, const Gdk::Rectangle& ca, const synfig::Color& dark, const synfig::Color& light)
{
	double	height = ca.get_height();
	double	width = ca.get_width();

	double r1(dark.get_r());
	double g1(dark.get_g());
	double b1(dark.get_b());
	double r2(light.get_r());
	double g2(light.get_g());
	double b2(light.get_b());
	Gdk::Color gdk_c;
	Cairo::RefPtr<Cairo::ImageSurface> image=Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, height, height);
	Cairo::RefPtr<Cairo::Context> subcr = Cairo::Context::create(image);
	subcr->save();
	subcr->set_source_rgb(r1, g1, b1);
	subcr->paint();
	subcr->restore();
	subcr->save();
	subcr->set_source_rgb(r2, g2, b2);
	subcr->rectangle(0.0, 0.0, height/2.0, height/2.0);
	subcr->clip();
	subcr->paint();
	subcr->restore();
	subcr->save();
	subcr->set_source_rgb(r2, g2, b2);
	subcr->rectangle(height/2.0, height/2.0, height/2.0, height/2.0);
	subcr->clip();
	subcr->paint();
	subcr->restore();

	Cairo::RefPtr<Cairo::SurfacePattern> pattern = Cairo::SurfacePattern::create (image);
	pattern->set_extend(Cairo::EXTEND_REPEAT);
	cr->save();
	cr->rectangle(ca.get_x(), ca.get_y(), width-2, height);
	cr->translate(ca.get_x(), ca.get_y());
	cr->set_source(pattern);
	cr->fill();
	cr->restore();
}

void
studio::render_gradient_to_cairo(const Cairo::RefPtr<Cairo::Context>& cr,const Gdk::Rectangle& ca,const synfig::Gradient &gradient)
{
	double	height = ca.get_height();
	double	width = ca.get_width();

	Cairo::RefPtr<Cairo::LinearGradient> gpattern = Cairo::LinearGradient::create(ca.get_x(), ca.get_y(), ca.get_x()+width, ca.get_y());
	double a, r, g, b;
	Gradient::CPoint cp;
	Gradient::const_iterator iter;
	for(iter=gradient.begin();iter!=gradient.end(); iter++)
	{
		cp=*iter;
		a=cp.color.get_a();
		r=cp.color.get_r();
		g=cp.color.get_g();
		b=cp.color.get_b();
		gpattern->add_color_stop_rgba(cp.pos, r, g, b, a);
	}

	cr->save();
	cr->rectangle(ca.get_x(), ca.get_y(), ca.get_width()-2, ca.get_height());
	cr->set_source(gpattern);
	cr->fill();
	cr->restore();

	cr->save();
	cr->set_line_width(1.0);
	cr->set_source_rgb(1.0, 1.0, 1.0);
	cr->rectangle(ca.get_x()+1.5, ca.get_y()+1.5, width-3, height-3);
	cr->stroke();
	cr->restore();
	cr->save();
	cr->set_line_width(1.0);
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->rectangle(ca.get_x()+0.5, ca.get_y()+0.5, width-1, height-1);
	cr->stroke();
	cr->restore();
}

/* === M E T H O D S ======================================================= */

Widget_Gradient::Widget_Gradient():
	editable_(false)
{
	set_size_request(-1,64);
	signal_expose_event().connect(sigc::mem_fun(*this, &studio::Widget_Gradient::redraw));
	add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	add_events(Gdk::BUTTON1_MOTION_MASK);

}

Widget_Gradient::~Widget_Gradient()
{
}

#define CONTROL_HEIGHT		16
bool
Widget_Gradient::redraw(GdkEventExpose */*bleh*/)
{
	//!Check if the window we want draw is ready
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(!window) return false;

	const int h(get_height());
	const int w(get_width());

	Gdk::Rectangle area(0,0,w,h);
	if(!editable_)
	{
		render_gradient_to_window(window,area,gradient_);
		return true;
	}

	render_gradient_to_window(window,Gdk::Rectangle(0,0,w,h),gradient_);

	Gradient::iterator iter,selected_iter;
	bool show_selected(false);
	for(iter=gradient_.begin();iter!=gradient_.end();iter++)
	{
		if(*iter!=selected_cpoint)
		get_style()->paint_arrow(
			window,
		  (iter->color.get_y()<ARROW_NEGATIVE_THRESHOLD)?Gtk::STATE_SELECTED:Gtk::STATE_ACTIVE, //use light arrow on dark color, and dark arrow on light color , todo detect from style which is darkest from SELECTED or ACTIVE, here SELECTED is lighter.
			Gtk::SHADOW_OUT,
			area,
			*this,
			" ",
			Gtk::ARROW_UP,
			1,
			int(iter->pos*w)-CONTROL_HEIGHT/2+1,
			h-CONTROL_HEIGHT,
			CONTROL_HEIGHT,
			CONTROL_HEIGHT
		);
		else
		{
			selected_iter=iter;
			show_selected=true;
		}
	}

	// we do this so that we can be sure that
	// the selected marker is shown on top
  // show 2 arrows for selected, to compensate for lack of contrast in some color schemes, such as ubuntu default theme
  // Gtk::STATE_SELECTED was used, but resulted in a barely visible arrow in some color scheme, hence double arrow with contrasting color

	if(show_selected)
	{
		get_style()->paint_arrow(
			window,
			(selected_iter->color.get_y()<ARROW_NEGATIVE_THRESHOLD)?Gtk::STATE_SELECTED:Gtk::STATE_ACTIVE, //use light arrow on dark color, and dark arrow on light color , todo detect from style which is darkest from SELECTED or ACTIVE
			Gtk::SHADOW_OUT,
			area,
			*this,
			" ",
			Gtk::ARROW_UP,
			1,
			round_to_int(selected_iter->pos*w)-CONTROL_HEIGHT/2+1,
			h-CONTROL_HEIGHT,
			CONTROL_HEIGHT,
			CONTROL_HEIGHT
		); // paint_arrow(window, state_type, shadow_type, area, widget, detail, arrow_type, fill, x, y, width, height)
		get_style()->paint_arrow(
			window,
						(selected_iter->color.get_y()<ARROW_NEGATIVE_THRESHOLD)?Gtk::STATE_SELECTED:Gtk::STATE_ACTIVE, //use light arrow on dark color, and dark arrow on light color , todo detect from style which is darkest from SELECTED or ACTIVE
			Gtk::SHADOW_OUT,
			area,
			*this,
			" ",
			Gtk::ARROW_UP,
			1,
			round_to_int(selected_iter->pos*w)-CONTROL_HEIGHT/2+1,
			h-CONTROL_HEIGHT*1.3,
			CONTROL_HEIGHT,
			CONTROL_HEIGHT
		);
	}

	return true;
}

void
Widget_Gradient::insert_cpoint(float x)
{
	Gradient::CPoint new_cpoint;
	new_cpoint.pos=x;
	new_cpoint.color=gradient_(x);
	gradient_.push_back(new_cpoint);
	gradient_.sort();
	gradient_.sort();
	set_selected_cpoint(new_cpoint);
	queue_draw();
}

void
Widget_Gradient::remove_cpoint(float x)
{
	gradient_.erase(gradient_.proximity(x));
	signal_value_changed_();
	queue_draw();
}

void
Widget_Gradient::popup_menu(float x)
{
	Gtk::Menu* menu(manage(new Gtk::Menu()));
	menu->signal_hide().connect(sigc::bind(sigc::ptr_fun(&delete_widget), menu));

	menu->items().clear();

	menu->items().push_back(
		Gtk::Menu_Helpers::MenuElem(
			_("Insert Color Stop"),
			sigc::bind(
				sigc::mem_fun(*this,&studio::Widget_Gradient::insert_cpoint),
				x
			)
		)
	);

	if(!gradient_.empty())
	{
		menu->items().push_back(
			Gtk::Menu_Helpers::MenuElem(
				_("Remove Color Stop"),
				sigc::bind(
					sigc::mem_fun(*this,&studio::Widget_Gradient::remove_cpoint),
					x
				)
			)
		);
	}

	menu->popup(0,0);
}

void
Widget_Gradient::set_value(const synfig::Gradient& x)
{
	gradient_=x;
	if(gradient_.size())
		set_selected_cpoint(*gradient_.proximity(0.0f));
	queue_draw();
}

void
Widget_Gradient::set_selected_cpoint(const synfig::Gradient::CPoint &x)
{
	selected_cpoint=x;
	signal_cpoint_selected_(selected_cpoint);
	queue_draw();
}

void
Widget_Gradient::update_cpoint(const synfig::Gradient::CPoint &x)
{
	try
	{
		Gradient::iterator iter(gradient_.find(x));
		iter->pos=x.pos;
		iter->color=x.color;
		gradient_.sort();
		queue_draw();
	}
	catch(synfig::Exception::NotFound)
	{
		// Yotta...
	}
}

bool
Widget_Gradient::on_event(GdkEvent *event)
{
	//if(editable_)
	{
		const int x(static_cast<int>(event->button.x));
		const int y(static_cast<int>(event->button.y));

		float pos((float)x/(float)get_width());
		if(pos<0.0f)pos=0.0f;
		if(pos>1.0f)pos=1.0f;

		switch(event->type)
		{
		case GDK_MOTION_NOTIFY:
			if(editable_ && y>get_height()-CONTROL_HEIGHT)
			{
				if(!gradient_.size()) return true;
				Gradient::iterator iter(gradient_.find(selected_cpoint));
				//! Use SHIFT to stack two CPoints together.
				if(event->button.state&GDK_SHIFT_MASK)
				{
					float begin(-100000000),end(100000000);
					Gradient::iterator before(iter),after(iter);
					after++;
					if(iter!=gradient_.begin())
					{
						before--;
						begin=before->pos;
					}
					if(after!=gradient_.end())
					{
						end=after->pos;
					}

					if(pos>end)
						pos=end;
					if(pos<begin)
						pos=begin;

					iter->pos=pos;
				}
				else
				{
					iter->pos=pos;
					gradient_.sort();
				}

//				signal_value_changed_();
				changed_=true;
				queue_draw();
				return true;
			}
			break;
		case GDK_BUTTON_PRESS:
			changed_=false;
			if(event->button.button==1)
			{
				if(editable_ && y>get_height()-CONTROL_HEIGHT)
				{
					set_selected_cpoint(*gradient_.proximity(pos));
					queue_draw();
					return true;
				}
				else
				{
					signal_clicked_();
					return true;
				}
			}
			else if(editable_ && event->button.button==3)
			{
				popup_menu(pos);
				return true;
			}
			break;
		case GDK_BUTTON_RELEASE:
			if(editable_ && event->button.button==1 && y>get_height()-CONTROL_HEIGHT)
			{
				set_selected_cpoint(*gradient_.proximity(pos));
				if(changed_)signal_value_changed_();
				return true;
			}
		default:
			break;
		}
	}

	return false;
}
