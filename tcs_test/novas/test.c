/*
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eph_manager.h" /* remove this line for use with solsys version 2 */
#include "novas.h"
#include "sofa.h"

#define JPLEPH  "/var/local/novas/DE405.bin"


#define DEBUG


int main ( int argc, char *argv[] )
{
/* NOVAS 3.1 Example Calculations */
    const short int year      = 2017;
    const short int month     = 1;
    const short int day       = 1;
//    const double    hour      = 0.0000627;
    const double    hour      = 0.0;
    const double    leap_secs = 33.0;
    short int       accuracy  = 0;
    short int       de_num    = 0;
    short int       error     = 0;
   
    const double ut1_utc      = -0.387845;
   
    const double latitude     = 90.0;
    const double longitude    =  0.0;
    const double height       =  0.0;
    const double temperature  = 20.0;
    const double pressure     =  0.0;
      
    const double y_pole       =  0.0;
    const double x_pole       =  0.0;
   
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

    int i;

    on_surface geo_loc;
    observer   obs_loc;
    cat_entry star, dummy_star;
    sky_pos    t_place;

/*  Set looping and accuracy from arguments */
    i = argc > 1 ? atoi( argv[1] ) : 1; 
    accuracy = argc > 2 ? (*argv[2] == '1') : 0;  

/*  Establish time arguments */
    jd_utc  = julian_date( year, month, day, hour );
    jd_tt   = jd_utc + (leap_secs + 32.184) / 86400.0L;
    jd_ut1  = jd_utc + ut1_utc / 86400.0;
    delta_t = 32.184 + leap_secs - ut1_utc;
    jd_tdb  = jd_tt;          /* Approximation good to 0.0017 seconds. */
   
/* Make a test star */
    make_cat_entry (" TEST","FK6", 1, 12.0, 90.0, 0.0, 0.0, 0.0, 0.0, &star );
      
    if ((error = ephem_open (JPLEPH, &jd_beg,&jd_end,&de_num)) != 0) {
        if (error == 1)
            printf ("JPL ephemeris file not found.\n");
        else
            printf ("Error reading JPL ephemeris file header.\n");
        return (error);
    } else {
        printf ("JPL ephemeris DE%d open:\n  Beg. JD = %10.2f\n  End  JD = %10.2f\n\n", de_num, jd_beg, jd_end);
    }

/*   Write assumed longitude, latitude, height (ITRS = WGS-84).
*/
    make_on_surface( latitude, longitude, height, temperature, pressure, &geo_loc );
    make_observer_on_surface( latitude, longitude, height, temperature,pressure, &obs_loc );
    printf ( "Geodetic location:\n");
    printf ( "  Long.=%15.10f\n  Lat. =%15.10f\n  Hght.=%15.10f\n\n", geo_loc.longitude, geo_loc.latitude, geo_loc.height );

    printf ( "Time:\n");
    printf ( "  UTC=%15.6f\n", jd_utc);
    printf ( "  TT =%15.6f\n", jd_tt );
    printf ( "  UT1=%15.6f\n", jd_ut1);
    printf ( "  𝚫T =%15.6f\n", delta_t);
    puts("");
      
    for ( ; i-- ; ) {
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
    equ2hor ( jd_ut1, delta_t, accuracy, 0.0, 0.0, &geo_loc, rat, dect, 2, &zd, &az, &rar, &decr );
    }
    
//   printf ("FK6 1307 geocentric and topocentric positions:\n");
//   printf ("%15.10f        %15.10f\n", ra,  dec);
//   printf ("%15.10f        %15.10f\n", rat, dect);
    
    iauA2af( ndp, zd * DD2R, &zd_sign, zd_dmsf ); 
    iauA2af( ndp, az * DD2R, &az_sign, az_dmsf ); 


    printf ("FK6 zenith dist. and azimuth:\n");
    printf ("  ZD=%15.10f\n  AZ=%15.10f\n", zd,  az);
    printf ("  ZD=%c%3.3i:%2.2i:%2.2i.%*.*i\n", zd_sign, zd_dmsf[0], zd_dmsf[1], zd_dmsf[2], ndp, ndp, zd_dmsf[3] );
    printf ("  AZ=%c%3.3i:%2.2i:%2.2i.%*.*i\n", az_sign, az_dmsf[0], az_dmsf[1], az_dmsf[2], ndp, ndp, az_dmsf[3] );
    printf ("\n");

    ephem_close();  /* remove this line for use with solsys version 2 */
    return 0;
}
