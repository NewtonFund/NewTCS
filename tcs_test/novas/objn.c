#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eph_manager.h" /* remove this line for use with solsys version 2 */
#include "novas.h"
#include "sofa.h"

#define JPLEPH  "/var/local/novas/DE405.bin"

//#define DEBUG 

int main ( int argc, char *argv[] )
{
/* NOVAS 3.1 Example Calculations */
    const short int year      = 2017;
    const short int month     = 1;
    const short int day       = 1;
    const double    hour      = 0.0;
    const double    leap_secs = 37;
    short int       accuracy  = 0;
    short int       de_num    = 0;
    short int       error     = 0;
   
//    const double ut1_utc      = -0.387845;
    const double ut1_utc      = 0.6;  // 2017 	
   
    double       latitude     = 45.0; // Default latitude
    double       longitude    =  0.0; // Default longitude
    double       declination  = 90.0; // Object declination 
    const double height       = 0.0;
    const double temperature  = 20.0;
    const double pressure     = 0.0;
      
    const double y_pole       = 0.0;
    const double x_pole       = 0.0;
   
    double jd_beg, jd_end, jd_utc, jd_tt, jd_ut1, jd_tdb, delta_t,
           ra, dec, dis, rat, dect, dist, zd, az, rar, decr, last, theta, 
           jd[2], pos[3], vel[3], pose[3], elon, elat, r, lon_rad, lat_rad, 
           sin_lon, cos_lon, sin_lat, cos_lat, vter[3], vcel[3];

/*  For conversions */ 
    char zd_sign;
    char az_sign;
    int  zd_dmsf[4];
    int  az_dmsf[4];
    int  ndp = 8;		/* Number of decimal points */

    int i = 1;

    on_surface geo_loc;
    observer   obs_loc;
    cat_entry star, dummy_star;
    sky_pos    t_place;

/*  Set accuracy from arguments */
    i = argc > 1 ? atoi( argv[1] ) : 1; 
    accuracy = argc > 2 ? (*argv[2] == '1') : 0;  

/*  Establish time arguments */
    jd_utc  = julian_date( year, month, day, hour );
    jd_tt   = jd_utc + ((leap_secs + 32.184L) / 86400.0L);
    jd_ut1  = jd_utc + (ut1_utc / 86400.0L);
    delta_t = leap_secs - ut1_utc;
    jd_tdb  = jd_tt;          /* Approximation good to 0.0017 seconds. */

#ifdef DEBUG
    printf( "DBG: Time\n" );
    printf( "DBG: UTC= %15.6f\n", jd_utc );
    printf( "DBG: TT = %15.6f\n", jd_tt  );
//    printf( "DBG: TAI= %15.6f\n", tai1 + tai2 );
//    printf( "DBG: 𝚫T = %15.6f\n", dut1        );
    return 0;
#endif

   
    for ( declination = 0.0; declination <= 90.0; declination += 1.0 ) { 

/* Make a test star */
        make_cat_entry (" TEST","FK6", 1, 12.0, declination, 0.0, 0.0, 0.0, 0.0, &star );
        if ((error = ephem_open(JPLEPH, &jd_beg, &jd_end, &de_num)) != 0) {
            if (error == 1)
                printf ("JPL ephemeris file not found.\n");
            else
                printf ("Error reading JPL ephemeris file header.\n");
            return (error);
        } else {
//        printf ("JPL ephemeris DE%d open:\n  Beg. JD = %10.2f\n  End  JD = %10.2f\n\n", de_num, jd_beg, jd_end);
        }

        make_on_surface( latitude, longitude, height, temperature, pressure, &geo_loc );
        make_observer_on_surface( latitude, longitude, height, temperature, pressure, &obs_loc );

        if ( error = app_star( jd_tt, &star, accuracy, &ra, &dec )) {
            printf ( "Error %d from app_star.\n", error);
            return (error);
        }

        if ( error = topo_star( jd_tt, delta_t, &star, &geo_loc, accuracy, &rat, &dect )) {
            printf ( "Error %d from topo_star.\n", error);
            return (error);
        }
        equ2hor ( jd_ut1, delta_t, accuracy, 0.0, 0.0, &geo_loc, rat, dect, 0, &zd, &az, &rar, &decr );
    
        iauA2af( ndp, zd * DD2R, &zd_sign, zd_dmsf ); 
        iauA2af( ndp, az * DD2R, &az_sign, az_dmsf ); 
        printf ("%f\t%.9f\t%.9f\n", declination, zd, az );
      
  
//        printf ("%f\t%c%3.3i\t%2.2i\t%2.2i.%*.*i\t", declination, zd_sign, zd_dmsf[0], zd_dmsf[1], zd_dmsf[2], ndp, ndp, zd_dmsf[3] );
//        printf (    "%c%3.3i\t%2.2i\t%2.2i.%*.*i\n",              az_sign, az_dmsf[0], az_dmsf[1], az_dmsf[2], ndp, ndp, az_dmsf[3] );
    }
    ephem_close();  /* remove this line for use with solsys version 2 */
    return 0;
}
