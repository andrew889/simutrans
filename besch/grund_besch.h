/*
 *  Copyright (c) 1997 - 2002 by Volker Meyer & Hansj�rg Malthaner
 *
 * This file is part of the Simutrans project under the artistic licence.
 */

#ifndef __GRUND_BESCH_H
#define __GRUND_BESCH_H

#include "obj_besch_std_name.h"
#include "bildliste2d_besch.h"
#include "../simtypes.h"
#include "../dataobj/ribi.h"

/*
 *  Author:
 *      Volker Meyer
 *
 *  Description:
 *      Verschiedene Untergr�nde - viellcht bald weisse Berge?
 *
 *  Child nodes:
 *	0   Name
 *	1   Copyright
 *	2   Image-array
 */

class grund_t;
class karte_t;

class grund_besch_t : public obj_besch_std_name_t {
private:
	static karte_t *welt;

	static image_id image_offset;

public:
	static uint16 water_animation_stages;
	static sint16 water_depth_levels;

	// only these textures need external access
	static const grund_besch_t *shore;	// nicer shore graphics, optional
	static const grund_besch_t *fundament;
	static const grund_besch_t *slopes;
	static const grund_besch_t *fences;
	static const grund_besch_t *marker;
	static const grund_besch_t *borders;
	static const grund_besch_t *sea;	// different water depth
	static const grund_besch_t *ausserhalb;

	static char const* get_climate_name_from_bit(climate n);

	static bool double_grounds;

	static const uint8 slopetable[80];
	// returns the correct hang number for this slope
	static inline int get_double_hang(slope_t::type typ) {
		return slopetable[typ];
	}

	// returns the pointer to an image structure
	const image_t *get_image_ptr(uint16 typ, uint16 stage=0) const
	{
		image_array_t const* const imgarray   = get_child<image_array_t>(2);
		image_list_t   const* const liste = imgarray->get_list(typ);
		if(liste && liste->get_count() > 0) {
			image_t const* const image = imgarray->get_image(typ, stage);
			return image;
		}
		return NULL;
	}

	// image for all non-climate stuff like foundations ...
	image_id get_image(uint16 typ, uint16 stage=0) const
	{
		image_t const* const image = get_image_ptr(typ, stage);
		return image ? image->get_id() : IMG_EMPTY;
	}

	// image for all ground tiles
	static image_id get_ground_tile(grund_t *gr);

	static image_id get_water_tile(slope_t::type slope);
	static image_id get_climate_tile(climate cl, slope_t::type slope);
	static image_id get_snow_tile(slope_t::type slope);
	static image_id get_beach_tile(slope_t::type slope, uint8 corners);
	static image_id get_alpha_tile(slope_t::type slope);
	static image_id get_alpha_tile(slope_t::type slope, uint8 corners);

	static bool register_besch(const grund_besch_t *besch);

	static bool alles_geladen();

	/**
	 * Generates ground texture images, transition maps, etc.
	 */
	static void init_ground_textures(karte_t *welt);

	static image_id get_marker_image(slope_t::type slope_in, bool background)
	{
		uint8 slope = double_grounds ? slope_in : slopetable[slope_in];
		uint8 index = background ? (double_grounds ? (slope % 3) + 3 * ((uint8)(slope / 9)) + 27
		                                           : ((slope & 1) + ((slope >> 1) & 6) + 8))
		                         : (double_grounds ?  slope % 27
		                                           : (slope & 7 ));
		return marker->get_image(index);
	}

	static image_id get_border_image(slope_t::type slope_in)
	{
		uint8 slope = double_grounds ? slope_in : slopetable[slope_in];
		uint8 index = double_grounds ? (slope % 3) + 3 * ((uint8)(slope / 9)) : (slope & 1) + ((slope >> 1) & 6);
		return borders->get_image(index);
	}
};

#endif
