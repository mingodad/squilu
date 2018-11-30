#include "squirrel.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

struct lat_long_st
{
    double latitude, longitude;
};

inline double toDeg(double lat)
{
    return lat * 180.0 / M_PI;
}

inline double toRad(double x)
{
    return x * M_PI / 180.0;
}

//
// Convert Ordnance Survey grid reference easting/northing coordinate to (OSGB36) latitude/longitude
//
// @param {OsGridRef} easting/northing to be converted to latitude/longitude
// @return {LatLon} latitude/longitude (in OSGB36) of supplied grid reference
//
static lat_long_st osGridToLatLong(double easting, double northing)
{
    double E = easting;
    double N = northing;

    double a = 6377563.396;
    double b = 6356256.910;              // Airy 1830 major & minor semi-axes
    double F0 = 0.9996012717;                             // NatGrid scale factor on central meridian
    double lat0 = 49.0*M_PI/180.0;
    double lon0 = -2.0*M_PI/180.0;  // NatGrid true origin
    double N0 = -100000.0;
    double E0 = 400000.0;                     // northing & easting of true origin, metres
    double e2 = 1.0 - (b*b)/(a*a);                          // eccentricity squared
    double n = (a-b)/(a+b);
    double n2 = n*n;
    double n3 = n*n*n;

    double lat=lat0;
    double M=0.0;

    double ma_p1 = (1.0 + n + (5.0/4.0)*n2 + (5.0/4.0)*n3);
    double mb_p1 = (3.0*n + 3.0*n2 + (21.0/8.0)*n3);
    double mc_p1 = ((15.0/8.0)*n2 + (15.0/8.0)*n3);
    double md_p1 = (35.0/24.0)*n3;
    double n_less_n0 = N-N0;
    double bf0 = b * F0;
    double af0 = a * F0;

    do
    {
        lat = (n_less_n0-M)/af0 + lat;
        double lat_less_lat0 =  lat-lat0;
        double lat_plus_lat0 =  lat+lat0;
        double Ma =  ma_p1 * lat_less_lat0;
        double Mb =  mb_p1 * sin(lat_less_lat0) * cos(lat_plus_lat0);
        double Mc =  mc_p1 * sin(2*lat_less_lat0) * cos(2*lat_plus_lat0);
        double Md =  md_p1 * sin(3.0*lat_less_lat0) * cos(3*lat_plus_lat0);
        M = bf0 * (Ma - Mb + Mc - Md);                // meridional arc

    }
    while (n_less_n0-M >= 0.00001);    // ie until < 0.01mm

    double cosLat = cos(lat);
    double sinLat = sin(lat);
    double e2_sinLat2 = 1.0-e2*sinLat*sinLat;
    double nu = a*F0/sqrt(e2_sinLat2);              // transverse radius of curvature
    double rho = a*F0*(1.0-e2)/pow(e2_sinLat2, 1.5);  // meridional radius of curvature
    double eta2 = nu/rho-1.0;

    double tanLat = tan(lat);
    double tan2lat = tanLat*tanLat;
    double tan4lat = tan2lat*tan2lat;
    double tan6lat = tan4lat*tan2lat;
    double secLat = 1.0/cosLat;
    double nu3 = nu*nu*nu;
    double nu5 = nu3*nu*nu;
    double nu7 = nu5*nu*nu;
    double VII = tanLat/(2.0*rho*nu);
    double VIII = tanLat/(24.0*rho*nu3)*(5+3.0*tan2lat+eta2-9.0*tan2lat*eta2);
    double IX = tanLat/(720.0*rho*nu5)*(61+90.0*tan2lat+45.0*tan4lat);
    double X = secLat/nu;
    double XI = secLat/(6.0*nu3)*(nu/rho+2.0*tan2lat);
    double XII = secLat/(120.0*nu5)*(5+28.0*tan2lat+24.0*tan4lat);
    double XIIA = secLat/(5040.0*nu7)*(61+662.0*tan2lat+1320.0*tan4lat+720.0*tan6lat);

    double dE = (E-E0);
    double dE2 = dE*dE;
    double dE3 = dE2*dE;
    double dE4 = dE2*dE2;
    double dE5 = dE3*dE2;
    double dE6 = dE4*dE2;
    double dE7 = dE5*dE2;

    lat = lat - VII*dE2 + VIII*dE4 - IX*dE6;
    double lon = lon0 + X*dE - XI*dE3 + XII*dE5 - XIIA*dE7;

    return {toDeg(lat), toDeg(lon)};
}

static SQRESULT sq_osGridToLatLong(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP();
	SQ_GET_FLOAT(v, 2, easting);
	SQ_GET_FLOAT(v, 3, northing);

	lat_long_st ll = osGridToLatLong(easting, northing);
	sq_newarray(v, 2);
	sq_pushfloat(v, ll.latitude);
	sq_arrayappend(v, -1);
	sq_pushfloat(v, ll.longitude);
	sq_arrayappend(v, -1);
    return 1;
}

#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
typedef struct {
    HSQOBJECT func_to_call;
    HSQOBJECT param;
} gc_scope_alert_st;

static const SQChar *gc_scope_alert_TAG = _SC("gc_scope_alert");

static SQRESULT gc_scope_alert_releasehook(SQUserPointer p, SQInteger size, HSQUIRRELVM v)
{
	gc_scope_alert_st *self = ((gc_scope_alert_st *)p);
//printf("%p %p\n", p, v);
	if(self){
	    if(v){
            SQInteger top = sq_gettop(v);
            sq_reservestack(v, 20);
            sq_pushobject(v, self->func_to_call);
            sq_pushroottable(v);
            sq_pushobject(v, self->param);
            sq_call(v, 2, SQFalse, SQTrue);
            sq_release(v, &self->func_to_call);
            sq_release(v, &self->param);
            sq_settop(v, top);
	    }
	    //else
	    sq_free(self, sizeof(gc_scope_alert_st));
	}
	return 0;
}

static SQRESULT gc_scope_alert_constructor(HSQUIRRELVM v)
{
    SQInteger rc;
    gc_scope_alert_st proto;
    sq_resetobject(&proto.func_to_call);
    sq_resetobject(&proto.param);
    if((rc = sq_getstackobj(v, 2, &proto.func_to_call)) < 0) return rc;
    if(sq_gettop(v) > 2){
         if((rc = sq_getstackobj(v, 3, &proto.param)) < 0) return rc;
    }
    gc_scope_alert_st *self = (gc_scope_alert_st*)sq_malloc(sizeof(gc_scope_alert_st));
    if(!self) return sq_throwerror(v, "Failed to create %s", gc_scope_alert_TAG);
    sq_addref(v, &proto.func_to_call);
    sq_addref(v, &proto.param);
    *self = proto;
    sq_setinstanceup(v, 1, self);
    sq_setreleasehook(v,1, gc_scope_alert_releasehook);
    return 1;
}

#define _DECL_FUNC(name,nparams,tycheck) {_SC(#name),  gc_scope_alert_##name,nparams,tycheck}
static SQRegFunction gc_scope_alert_methods[] =
{
	_DECL_FUNC(constructor,  -2, _SC("xc.")),
	{0,0}
};
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS

static SQRESULT spectralnorm_A(HSQUIRRELVM v)
{
	SQ_FUNC_VARS_NO_TOP();
	SQ_GET_INTEGER(v, 2, i);
	SQ_GET_INTEGER(v, 3, j);
	SQInteger ij = j + i++;
	sq_pushfloat(v, 1.0/(ij * (ij+1)/2.0+i));
    return 1;
}


#ifdef __cplusplus
extern "C" {
#endif

    SQRESULT sqext_register_dad_utils(HSQUIRRELVM v)
    {
        sq_pushstring(v,_SC("dad_utils"),-1);
        sq_newtable(v);

        sq_insertfunc(v, _SC("osGridToLatLong"), sq_osGridToLatLong, 3, _SC(".ff"), true);


    	sq_insertfunc(v, _SC("spectralnorm_A"), spectralnorm_A, 3, _SC(".ii"), true);

#ifdef SQ_WITH_DELAYED_RELEASE_HOOKS
        sq_pushstring(v,gc_scope_alert_TAG,-1);
        sq_newclass(v,SQFalse);
        sq_settypetag(v,-1,(void*)gc_scope_alert_TAG);
        sq_insert_reg_funcs(v, gc_scope_alert_methods);
        sq_rawset(v,-3);
#endif // SQ_WITH_DELAYED_RELEASE_HOOKS
        sq_rawset(v,-3);//insert dad_utils
        return 1;
    }

#ifdef __cplusplus
}
#endif
