/*
 * Wasser-Untergrund f�r Simutrans.
 * �berarbeitet Januar 2001
 * von Hj. Malthaner
 */

#include "wasser.h"

#include "../simworld.h"

#include "../besch/grund_besch.h"

#include "../dataobj/environment.h"



int wasser_t::stage = 0;
bool wasser_t::change_stage = false;



// for animated waves
void wasser_t::prepare_for_refresh()
{
	if(!welt->is_fast_forward()  &&  env_t::water_animation>0) {
		int new_stage = (welt->get_zeit_ms() / env_t::water_animation) % grund_besch_t::water_animation_stages;
		wasser_t::change_stage = (new_stage != stage);
		wasser_t::stage = new_stage;
	}
	else {
		wasser_t::change_stage = false;
	}
}


void wasser_t::calc_image_internal(const bool calc_only_snowline_change)
{
	if(  !calc_only_snowline_change  ) {
		koord pos2d( get_pos().get_2d() );
		sint16 height = welt->get_water_hgt( pos2d );
		set_hoehe( height );
		slope = slope_t::flat;

		sint16 zpos = min( welt->lookup_hgt( pos2d ), height ); // otherwise slope will fail ...

		if(  grund_t::underground_mode == grund_t::ugm_level  &&  grund_t::underground_level < zpos  ) {
			set_image(IMG_EMPTY);
		}
		else {
			set_image( min( height - zpos, grund_besch_t::water_depth_levels ) /*grund_besch_t::get_ground_tile(0,zpos)*/ );
		}

		// test tiles to north, south, east and west and add to ribi if water
		ribi = ribi_t::none;
		for(  uint8 i = 0;  i < 4;  i++  ) {
			grund_t *gr_neighbour = NULL;
			if(  get_neighbour( gr_neighbour, invalid_wt, ribi_t::nsew[i] )  &&  (gr_neighbour->ist_wasser()  ||  gr_neighbour->hat_weg( water_wt ))  ) {
				ribi |= ribi_t::nsew[i];
			}
		}

		// artifical walls from here on ...
		grund_t::calc_back_image( height, 0 );
	}
}


void wasser_t::rotate90()
{
	grund_t::rotate90();
	ribi = ribi_t::rotate90(ribi);
}
