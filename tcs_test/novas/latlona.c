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

int main ( int argc, char *argv[] )
{
//  
    short  accuracy  = 0;
    short  error     = 0;

//  Julian date for test 
    short  year      = 2017;
    short  month     = 1;
    short  day       = 1;
    double hour      = 0.0;

//  From IERS bulletins
    double dut1      = 0.6;
    double leap_secs = 37;
    double cip_x     = 0.0; // Polar motion
    double cip_y     = 0.0;

//  Target catalog object (ICRS)    
    double cat_ra    = 12.0;
    double cat_dec   =  0.0; 

//  Observer details WGS84 model
    double obs_lat   =  45.0; 
    double obs_lon   =   0.0; 
    double obs_alt   =   0.0; // Altitude    [metres]
    double obs_temp  =  20.0; // Temperature [Celcius]
    double obs_pres  = 1000.0; // Pressure    [mbar]

//  Ephemeris data
    short  eph_num;
    double eph_beg;
    double eph_end;

//  Time variables
    double utc;     // Observing JD converted to decimal UTC
    double ut1;     // UTC1   
    double tt;      // Terrestrial time
    double tdb;     // Barycentrici Dynamical Time 
    double delta_t;  

//  Results
    double ra,  rar,  rat;
    double dec, decr, dect;
    double az;
    double zd;

//  Observation structures 
    on_surface sur_loc;   // Surface location 
    cat_entry  cat_star;  // Catalogue entry

//  Evaluate Time variables 
    utc     = julian_date( year, month, day, hour );
    tt      = utc + ((leap_secs + 32.184L) / 86400.0L);
    ut1     = utc + ( dut1 / 86400.0L);
    delta_t = leap_secs - dut1;
    tdb     = tt;  // Approximation good to 0.0017 seconds
   
/* Make a test catalog star */
    make_cat_entry (" TEST","FK6", 1, cat_ra, cat_dec, 0.0, 0.0, 0.0, 0.0, &cat_star );
    for ( obs_lat = -90.0; obs_lat <= 90.0; obs_lat += 1.0 ) { 
        for ( obs_lon = -180.0; obs_lon <= 180.0; obs_lon += 1.0 ) { 

            if ( ( error = ephem_open(JPLEPH, &eph_beg, &eph_end, &eph_num ))) {
                if (error == 1)
                    printf( "ERR: JPL ephemeris file %s not found.\n", JPLEPH );
                else
                    printf( "ERR: Error reading ephemeris file %s header.\n", JPLEPH );
                return (error);
            } else {
//            printf ( "INF: JPL ephemeris DE%d open:\n  Beg. JD = %10.2f\n  End  JD = %10.2f\n\n", eph_num, eph_beg, eph_end );
            }

            make_on_surface( obs_lat, obs_lon, obs_alt, obs_temp, obs_pres, &sur_loc );

            if ( error = app_star( tt, &cat_star, accuracy, &ra, &dec )) {
                printf ( "ERR: Error %d from app_star.\n", error );
                return (error);
            }

            if ( error = topo_star( tt, delta_t, &cat_star, &sur_loc, accuracy, &rat, &dect )) {
                printf ( "ERR: Error %d from topo_star.\n", error);
                return (error);
            }

//          Convert to observer coordinates 
            equ2hor( ut1, delta_t, accuracy, cip_x, cip_y, &sur_loc, rat, dect, 2, &zd, &az, &rar, &decr );

//          Print output
            printf( "%.15f\t%.15f\n", zd, az );
        }
        puts("");
    }
    ephem_close(); 
    return 0;
}
