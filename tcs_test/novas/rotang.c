/*!
 *  TEST UTILITY
 *
 *  NOVAS Transform of ICRS -> observed Zenith Distance and Azimuth
 *  Full World topographic latitudes and longitudes in 1 degree steps
 *
 *  Excludes atmospheric effects
 *
 *  Author: ASP
 *  Date:   20171020
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eph_manager.h" /* remove this line for use with solsys version 2 */
#include "novas.h"
#include "sofa.h"


// Location of JPL epehemeris file
#define JPLEPH  "/var/local/novas/DE405.bin"

/* Compute parallactic angle given latitude, object dec and alt.
 * all angles in rads.
 * N.B. always return >= 0, caller must determine sign and degenerate cases at
 *   pole or zenith.
 */
double parallacticLDA (double lt, double dec, double alt)
{
	double ca = sin(lt);
	double cb = sin(dec);
	double sb = cos(dec);
	double cc = sin(alt);
	double sc = cos(alt);
	double cpa;

	/* given three sides find an angle */
	if (sb==0 || sc==0)
	    return (0);
	cpa = (ca - cb*cc)/(sb*sc);
	if (cpa < -1) cpa = -1;
	if (cpa >  1) cpa =  1;
	return (acos (cpa));
}

int main ( int argc, char *argv[] )
{
//  
    short  accuracy  = 0;
    short  error     = 0;

//  Julian date for test 
    short  year      = 2017;
    short  month     = 11;
    short  day       = 2;
    double hour      = 20.0;
    short int cal_year;
    short int cal_month;
    short int cal_day;
    double    cal_hour;

//  From IERS bulletins
    double dut1      = 0.6;
    double leap_secs = 37;
    double cip_x     = 0.0; // Polar motion
    double cip_y     = 0.0;

//  Target catalog object (ICRS)    
    double cat_ra    = 4.4167028;
    double cat_dec   = 39.4992722; 

//  Observer details WGS84 model
    double obs_lat   =  28.760; // Roque de los Muchachos 
    double obs_lon   =  342.1184; 
    double obs_alt   =  2326; // Altitude    [metres]
    double obs_temp  =  20.0; // Temperature [Celcius]
    double obs_pres  = 1000.0; // Pressure    [mbar]

//  Ephemeris data
    short  eph_num;
    double eph_beg;
    double eph_end;

//  Time variables
    double utc;     // Observing JD converted to decimal UTC
    double utc_end;
    double utc_beg;
    double ut1;     // UTC1   
    double tt;      // Terrestrial time
    double tdb;     // Barycentrici Dynamical Time 
    double delta_t;  
    double obs_len = 33000; // Observation length in seconds  

//  Results
    double ra,  rar,  rat;
    double dec, decr, dect;
    double az;
    double zd;

//  Observation structures 
    on_surface sur_loc;   // Surface location 
    cat_entry  cat_star;  // Catalogue entry

    if ( ( error = ephem_open(JPLEPH, &eph_beg, &eph_end, &eph_num ))) {
        if (error == 1) {
            printf( "ERR: JPL ephemeris file %s not found.\n", JPLEPH );
            return (error);
        } else {
            printf( "ERR: Error reading ephemeris file %s header.\n", JPLEPH );
            return (error);
        }
    } else {
//        printf ( "INF: JPL ephemeris DE%d open:\n  Beg. JD = %10.2f\n  End  JD = %10.2f\n\n", eph_num, eph_beg, eph_end );
    }

    make_on_surface( obs_lat, obs_lon, obs_alt, obs_temp, obs_pres, &sur_loc );

/*  Make a test catalog star */
    make_cat_entry (" TEST","FK6", 1, cat_ra, cat_dec, 0.0, 0.0, 0.0, 0.0, &cat_star );

//  Evaluate Time variables 
    utc     = julian_date( year, month, day, hour ); 
    utc_beg = utc; 
    utc_end = utc + obs_len/86400.0L;  

    for ( ; utc < utc_end; utc = utc + 10.0/86400.0L ) { 
      tt      = utc + ((leap_secs + 32.184L) / 86400.0L);
      ut1     = utc + ( dut1 / 86400.0L);
      delta_t = leap_secs - dut1;
      tdb     = tt;  // Approximation good to 0.0017 seconds

      if ( error = app_star( tt, &cat_star, accuracy, &ra, &dec )) {
          printf ( "ERR: Error %d from app_star.\n", error );
          return (error);
      }

      if ( error = topo_star( tt, delta_t, &cat_star, &sur_loc, accuracy, &rat, &dect )) {
          printf ( "ERR: Error %d from topo_star.\n", error);
          return (error);
      }

//    Convert to observer coordinates 
      equ2hor( ut1, delta_t, accuracy, cip_x, cip_y, &sur_loc, rat, dect, 0, &zd, &az, &rar, &decr );

//    Print output
      printf( "%f\t%f\t%.15f\t%.15f\t%.15f\t%.15f\n", utc-utc_beg, utc, 90-zd, fmod(az,360.0), decr, DR2D*parallacticLDA( DD2R*obs_lat, DD2R*decr, DPI/2 - DD2R*zd ) );
    }

//  Tidy-up
    ephem_close(); 
    return 0;
}



