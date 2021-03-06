#include "api_param.h"
#include "api_class.h"

#include "../simcity.h"
#include "../simfab.h"
#include "../bauer/warenbauer.h"
#include "../dataobj/schedule.h"
#include "../dataobj/loadsave.h"
#include "../dataobj/scenario.h"
#include "../player/simplay.h"
#include "../utils/plainstring.h"
#include "api/api_command.h" // script_api::my_tool_t


template<typename T> T clamp(T v, T l, T u) { return v < l ? l : (v > u ? u :v); }

namespace script_api {

	karte_ptr_t welt;

// rotation handling
	void rotate90() { coordinate_transform_t::rotate90(); }
	void new_world() { coordinate_transform_t::new_world(); }

	uint8 coordinate_transform_t::rotation = 4;

	void coordinate_transform_t::initialize()
	{
		if (rotation == 4) {
			rotation = welt->get_settings().get_rotation();
		}
	}

	void coordinate_transform_t::rdwr(loadsave_t *file)
	{
		file->rdwr_byte(rotation);
	}

	void coordinate_transform_t::koord_w2sq(koord &k)
	{
		// do not transform koord::invalid
		if (k.x == -1  &&  k.y == -1) {
			return;
		}
		switch( rotation ) {
			// 0: do nothing
			case 1: k = koord(k.y, welt->get_size().y-1 - k.x); break;
			case 2: k = koord(welt->get_size().x-1 - k.x, welt->get_size().y-1 - k.y); break;
			case 3: k = koord(welt->get_size().x-1 - k.y, k.x); break;
			default: break;
		}
	}

	void coordinate_transform_t::koord_sq2w(koord &k)
	{
		// just rotate back
		rotation = 4 - rotation;
		koord_w2sq(k);
		// restore original rotation
		rotation = 4 - rotation;
	}

	void coordinate_transform_t::ribi_w2sq(ribi_t::ribi &r)
	{
		if (rotation) {
			r = ( ( (r << 4) | r) >> rotation) & 15;
		}
	}

	void coordinate_transform_t::ribi_sq2w(ribi_t::ribi &r)
	{
		if (rotation) {
			r = ( ( (r << 4) | r) << rotation) >> 4 & 15;
		}
	}

// void parameter
	SQInteger param<void_t>::push(HSQUIRRELVM, void_t const&)
	{
		return 0;
	}

	void_t param<void_t>::get(HSQUIRRELVM, SQInteger)
	{
		return void_t();
	}

// integer arguments
	uint8 param<uint8>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return clamp<uint8>(i, 0, 255);
	}
	SQInteger param<uint8>::push(HSQUIRRELVM vm, uint8 const& v)
	{
		sq_pushinteger(vm, v);
		return 1;
	}

	sint8 param<sint8>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return clamp<sint8>(i, -128, 127);
	}
	SQInteger param<sint8>::push(HSQUIRRELVM vm, sint8 const& v)
	{
		sq_pushinteger(vm, v);
		return 1;
	}

	uint16 param<uint16>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return clamp<uint16>(i, 0, 0xffff);
	}
	SQInteger param<uint16>::push(HSQUIRRELVM vm, uint16 const& v)
	{
		sq_pushinteger(vm, v);
		return 1;
	}

	sint16 param<sint16>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return clamp<sint16>(i, -32768, 0x7fff);
	}
	SQInteger param<sint16>::push(HSQUIRRELVM vm, sint16 const& v)
	{
		sq_pushinteger(vm, v);
		return 1;
	}

	uint32 param<uint32>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return i>=0 ? i : 0;
	}
	SQInteger param<uint32>::push(HSQUIRRELVM vm, uint32 const& v)
	{
		sq_pushinteger(vm, v);
		return 1;
	}

	sint32 param<sint32>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return i;
	}
	SQInteger param<sint32>::push(HSQUIRRELVM vm, sint32 const& v)
	{
		sq_pushinteger(vm, v);
		return 1;
	}

	uint64 param<uint64>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return i>=0 ? i : 0;
	}
	SQInteger param<uint64>::push(HSQUIRRELVM vm, uint64 const& v)
	{
		sq_pushinteger(vm, (SQInteger)v);
		return 1;
	}

	sint64 param<sint64>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQInteger i = 0;
		sq_getinteger(vm, index, &i);
		return i;
	}
	SQInteger param<sint64>::push(HSQUIRRELVM vm, sint64 const& v)
	{
		sq_pushinteger(vm, (SQInteger)v);
		return 1;
	}


	waytype_t param<waytype_t>::get(HSQUIRRELVM vm, SQInteger index)
	{
		return (waytype_t)(param<sint16>::get(vm, index));
	}
	SQInteger param<waytype_t>::push(HSQUIRRELVM vm, waytype_t const& v)
	{
		return param<sint16>::push(vm, v);
	}


	weg_t::system_type param<weg_t::system_type>::get(HSQUIRRELVM vm, SQInteger index)
	{
		return (weg_t::system_type)(param<uint8>::get(vm, index));
	}
	SQInteger param<weg_t::system_type>::push(HSQUIRRELVM vm, weg_t::system_type const& v)
	{
		return param<uint8>::push(vm, v);
	}


	obj_t::typ param<obj_t::typ>::get(HSQUIRRELVM vm, SQInteger index)
	{
		return (obj_t::typ)(param<uint8>::get(vm, index));
	}
	SQInteger param<obj_t::typ>::push(HSQUIRRELVM vm, obj_t::typ const& v)
	{
		return param<uint8>::push(vm, v);
	}


	SQInteger param<climate>::push(HSQUIRRELVM vm, climate const& v)
	{
		return param<uint8>::push(vm, v);
	}
// floats
	double param<double>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQFloat d = 0.0;
		sq_getfloat(vm, index, &d);
		return d;
	}
	SQInteger param<double>::push(HSQUIRRELVM vm, double  const& v)
	{
		sq_pushfloat(vm, (SQFloat)v);
		return 1;
	}

// strings
	const char* param<const char*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		const char* str = NULL;
		if (!SQ_SUCCEEDED(sq_getstring(vm, index, &str))) {
			sq_raise_error(vm, "Supplied string parameter is null");
			return NULL;
		}
		return str;
	}
	SQInteger param<const char*>::push(HSQUIRRELVM vm, const char* const& v)
	{
		if (v) {
			sq_pushstring(vm, v, -1);
		}
		else {
			sq_pushnull(vm);
		}
		return 1;
	}

	plainstring param<plainstring>::get(HSQUIRRELVM vm, SQInteger index)
	{
		if (sq_gettype(vm, index) == OT_NULL) {
			return NULL;
		}
		plainstring ret;
		const char* str = NULL;
		if (!SQ_SUCCEEDED(sq_getstring(vm, index, &str))) {
			// try tostring
			if (SQ_SUCCEEDED(sq_tostring(vm, index))) {
				sq_getstring(vm, -1, &str);
				ret = str;
				sq_pop(vm, 1);
			}
		}
		else {
			ret = str;
		}
		return ret;
	}

	SQInteger param<plainstring>::push(HSQUIRRELVM vm, plainstring const& v)
	{
		return param<const char*>::push(vm, v.c_str());
	}

// bool
	bool param<bool>::get(HSQUIRRELVM vm, SQInteger index)
	{
		SQBool b = false;
		sq_tobool(vm, index, &b);
		return b;
	}
	SQInteger param<bool>::push(HSQUIRRELVM vm, bool const& v)
	{
		sq_pushbool(vm, v);
		return 1;
	}

// coordinates
	koord param<koord>::get(HSQUIRRELVM vm, SQInteger index)
	{
		sint16 x=-1, y=-1;
		get_slot(vm, "x", x, index);
		get_slot(vm, "y", y, index);
		koord k(x,y);
		if (k.x != -1  &&  k.y != -1) {
			// transform coordinates
			coordinate_transform_t::koord_sq2w(k);
		}
		else {
			k = koord::invalid;
		}
		return k;
	}

	SQInteger param<koord>::push(HSQUIRRELVM vm, koord const& v)
	{
		koord k(v);
		// transform coordinates
		coordinate_transform_t::koord_w2sq(k);
		return push_instance(vm, "coord", k.x, k.y);
	}

	koord3d param<koord3d>::get(HSQUIRRELVM vm, SQInteger index)
	{
		sint8 z = -1;
		if (!SQ_SUCCEEDED(get_slot(vm, "z", z, index))) {
			return koord3d::invalid;
		}
		koord k = param<koord>::get(vm, index);
		return koord3d(k, z);
	}


	SQInteger param<koord3d>::push(HSQUIRRELVM vm, koord3d const& v)
	{
		koord k(v.get_2d());
		// transform coordinates
		coordinate_transform_t::koord_w2sq(k);
		return push_instance(vm, "coord3d", k.x, k.y, v.z);
	}

// pointers to classes

	convoi_t* param<convoi_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		convoihandle_t cnv = param<convoihandle_t>::get(vm, index);
		if (!cnv.is_bound()) {
			sq_raise_error(vm, "Invalid convoi id %d", cnv.get_id());
		}
		return cnv.get_rep();
	}

	fabrik_t* param<fabrik_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		koord pos = param<koord>::get(vm, index);
		fabrik_t* fab = fabrik_t::get_fab(pos);
		if (fab==NULL) {
			sq_raise_error(vm, "no factory at position (%s)", pos.get_str());
		}
		return fab;
	}

	SQInteger param<fabrik_t*>::push(HSQUIRRELVM vm, fabrik_t* const& fab)
	{
		if (fab == NULL) {
			sq_pushnull(vm); return 1;
		}
		koord pos(fab->get_pos().get_2d());
		coordinate_transform_t::koord_w2sq(pos);
		return push_instance(vm, "factory_x", pos.x, pos.y);
	}

	const ware_production_t* param<const ware_production_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		fabrik_t* fab = param<fabrik_t*>::get(vm, index);
		if (fab == NULL) {
			return NULL;
		}
		// obtain index into wareproduction_t arrays
		SQInteger i = -1;
		if (SQ_SUCCEEDED(get_slot(vm, "index", i, index))) {
			if (i>=0) {
				if ( (uint32)i<fab->get_eingang().get_count()) {
					return &fab->get_eingang()[i];
				}
				else {
					i -= fab->get_eingang().get_count();
					if ( (uint32)i<fab->get_ausgang().get_count()) {
						return &fab->get_ausgang()[i];
					}
				}
			}
		}
		sq_raise_error(vm, "No production slot [%d] in factory at (%s)", i, fab->get_pos().get_str());
		return NULL;
	}

	const fabrik_lieferant_besch_t* param<const fabrik_lieferant_besch_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		fabrik_t* fab = param<fabrik_t*>::get(vm, index);
		if (fab == NULL) {
			return NULL;
		}
		// obtain index into wareproduction_t arrays
		SQInteger i = -1;
		if (SQ_SUCCEEDED(get_slot(vm, "index", i, index))) {
			if (i>=0  &&  (uint32)i<fab->get_eingang().get_count()) {
				const ware_production_t& in = fab->get_eingang()[i];
				const fabrik_lieferant_besch_t* besch = fab->get_besch()->get_lieferant(i);
				// sanity check
				if (besch  &&  besch->get_ware() == in.get_typ()) {
					return besch;
				}
			}
		}
		sq_raise_error(vm, "No input slot [%d] in factory at (%s)", i, fab->get_pos().get_str());
		return NULL;
	}

	const fabrik_produkt_besch_t* param<const fabrik_produkt_besch_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		fabrik_t* fab = param<fabrik_t*>::get(vm, index);
		if (fab == NULL) {
			return NULL;
		}
		// obtain index into wareproduction_t arrays
		SQInteger i = -1;
		if (SQ_SUCCEEDED(get_slot(vm, "index", i, index))) {
			i -= fab->get_eingang().get_count();
			if (i>=0  &&  (uint32)i<fab->get_ausgang().get_count()) {
				const ware_production_t& out = fab->get_ausgang()[i];
				const fabrik_produkt_besch_t* besch = fab->get_besch()->get_produkt(i);
				// sanity check
				if (besch  &&  besch->get_ware() == out.get_typ()) {
					return besch;
				}
			}
		}
		sq_raise_error(vm, "No output slot [%d] in factory at (%s)", i, fab->get_pos().get_str());
		return NULL;
	}

	player_t* param<player_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		uint8 plnr = 0;
		get_slot(vm, "nr", plnr, index);
		if(plnr < 15) {
			return welt->get_player(plnr);
		}
		else {
			sq_raise_error(vm, "Invalid player index %d", plnr);
			return NULL;
		}
	}


	SQInteger param<player_t*>::push(HSQUIRRELVM vm, player_t* const& player)
	{
		return push_instance(vm, "player_x", player ? player->get_player_nr() : 16);
	}


	player_t* get_my_player(HSQUIRRELVM vm)
	{
		sq_pushregistrytable(vm);
		uint8 player_nr = PLAYER_UNOWNED;
		player_t *pl = NULL;
		if (SQ_SUCCEEDED(get_slot<uint8>(vm, "my_player_nr", player_nr))  &&  player_nr < 15) {
			pl =  welt->get_player(player_nr);
		}
		sq_poptop(vm);
		return pl;
	}


	const haltestelle_t* param<const haltestelle_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		halthandle_t halt = param<halthandle_t>::get(vm, index);
		if (!halt.is_bound()) {
			sq_raise_error(vm, "Invalid halt id %d", halt.get_id());
		}
		return halt.get_rep();
	}


	planquadrat_t* param<planquadrat_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		koord pos = param<koord>::get(vm, index);
		planquadrat_t *plan = welt->access(pos);
		if (plan==NULL) {
			sq_raise_error(vm, "Coordinate out of range (%s)", pos.get_str());
		}
		return plan;
	}


	grund_t* param<grund_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		koord3d pos = param<koord3d>::get(vm, index);
		grund_t *gr = welt->lookup(pos);
		if (gr==NULL) {
			gr = welt->lookup_kartenboden(pos.get_2d());
		}
		if (gr==NULL) {
			sq_raise_error(vm, "Coordinate out of range (%s)", pos.get_str());
		}
		return gr;
	}


	SQInteger param<grund_t*>::push(HSQUIRRELVM vm, grund_t* const& v)
	{
		if (v) {
			koord k = v->get_pos().get_2d();
			// transform coordinates
			coordinate_transform_t::koord_w2sq(k);
			return push_instance(vm, "tile_x", k.x, k.y, v->get_pos().z);
		}
		else {
			sq_pushnull(vm); return 1;
		}
	}


	scenario_t* param<scenario_t*>::get(HSQUIRRELVM, SQInteger)
	{
		return welt->get_scenario();
	}


	SQInteger param<schedule_entry_t>::push(HSQUIRRELVM vm, schedule_entry_t const& v)
	{
		return push_instance(vm, "schedule_entry_x", v.pos, v.minimum_loading, v.waiting_time_shift);
	}


	SQInteger param<schedule_t*>::push(HSQUIRRELVM vm, schedule_t* const& v)
	{
		return param<const schedule_t*>::push(vm, v);
	}


	SQInteger param<const schedule_t*>::push(HSQUIRRELVM vm, const schedule_t* const& v)
	{
		if (v) {
			return push_instance(vm, "schedule_x", v->get_waytype(), v->entries);
		}
		else {
			sq_pushnull(vm); return 1;
		}
	}


	settings_t* param<settings_t*>::get(HSQUIRRELVM, SQInteger)
	{
		return &welt->get_settings();
	}


	simline_t* param<simline_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		linehandle_t line = param<linehandle_t>::get(vm, index);
		if (!line.is_bound()) {
			sq_raise_error(vm, "Invalid line id %d", line.get_id());
		}
		return line.get_rep();
	}


	stadt_t* param<stadt_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		koord pos = param<koord>::get(vm, index);
		return welt->suche_naechste_stadt(pos);
	}

	SQInteger param<stadt_t*>::push(HSQUIRRELVM vm, stadt_t* const& v)
	{
		if (v) {
			koord k = v->get_pos();
			// transform coordinates
			coordinate_transform_t::koord_w2sq(k);
			return push_instance(vm, "city_x", k.x, k.y);
		}
		else {
			sq_pushnull(vm); return 1;
		}
	}

	karte_t* param<karte_t*>::get(HSQUIRRELVM, SQInteger)
	{
		return welt;
	}

	SQInteger param<karte_t*>::push(HSQUIRRELVM vm, karte_t* const&)
	{
		sq_pushnull(vm); return 1;
	}

	tool_t* param<tool_t*>::get(HSQUIRRELVM vm, SQInteger index)
	{
		my_tool_t *mtool = get_attached_instance<my_tool_t>(vm, index, param<tool_t*>::tag());
		if (mtool) {
			return mtool->tool;
		}
		return NULL;
	}
};
