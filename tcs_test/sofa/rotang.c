/*!
 *  TEST UTILITY
 *
 *  SOFA Transform of ICRS -> observed Zenith Distance and Azimuth
 *  Full World topographic latitudes and longitudes in 1 degree steps
 *
 *  Excludes atmospheric effects
 *
 *  Author: ASP
 *  Date:   20171020
 */

#include <stdio.h>
#include <stdlib.h>
#include <sofa.h>

#define NDP 8 //Number of decimal points in a conversion

int main ( int argc, char *argv[] ) {

    int lat_d;
    int lon_d;

//  Julian date for test 
    int    year       = 2017;
    int    month      = 11;
    int    day        = 2;
    int    hour       = 20;
    int    min        = 0;
    double sec        = 0.0;
    int    obs_len    = 1000; // Length of observation

//  Target object details in ICRS units
    double icrs_ra     = 4.4167028; // RA [decimal]
    double icrs_ra_pm  =  0.0; // RA proper motion [mas/year] 
    double icrs_dec    = 39.49927220; // DEC [decimal]
    double icrs_dec_pm =  0.0; // DEC proper motion [mas/year] 

// From IERS bulletins
    double dut1      = 0.6;
//  double leap_secs =  37; // Hard coded in SOFA
    double cip_x     = 0.0; // Celestial Intermediate Pole CIP [radians]
    double cip_y     = 0.0; 

//  Target catalog object (ICRS)
    char   cat_ra_sgn;  // RA co-ordinate +/-HH:MM:SS.sss
    int    cat_ra_h;
    int    cat_ra_m; 
    double cat_ra_s;
    double cat_ra;      // Right ascension [radians]
    double cat_ra_pm = 0;   // RA proper motion [radians/year] 

    char   cat_dec_sgn; // DEC co-ordinate +/-DDD:MM:DD.sss 
    int    cat_dec_d;
    int    cat_dec_m;
    double cat_dec_s;
    double cat_dec;      // Declination
    double cat_dec_pm = 0;   // DEC proper motion [radians/year] 

    double cat_par = 0;  // parallax  [arcsec]
    double cat_vel = 0;  // recession velocity [km/s]

//  Observer details +/- DD:MM:SS.sss
    char   obs_lat_sgn=  '+';
    int    obs_lat_d  =   28; // Lat. [degrees]
    int    obs_lat_m  =   45; // Lat. [minutes]
    double obs_lat_s  =   44; // Lat. [seconds]
    double obs_lat;           // Lat. [radians]

    char   obs_lon_sgn=  '+';
    int    obs_lon_d  =   17; // Long. [degrees]
    int    obs_lon_m  =   52; // Long. [minutes]
    double obs_lon_s  =   45; // Long. [seconds]
    double obs_lon;           // Long. [radians] 

    double obs_alt    =   2363; // Altitude [metres]
    double obs_temp   =   20.0; // Temperature [Celcius]
    double obs_pres   = 1000.0; // Pressure [mbar]
    double obs_rh     =    0.5; // Relative humidity [0.0-1.0]
    double obs_wl     =   0.55; // Wavelength [um]

    double utc1, utc2; // UTC 
    double tai1, tai2; // TAI atomic time 
    double tt1,  tt2;  // TT  terrestrial time

    double az;  // Azimuth [radians, N=0, E=+90, S=+180, W=+270]
    double zd;  // Zenith distance [radians]
    double hob; // Hour angle [radians] 
    double dob; // DEC [radians]
    double rob; // RA  [radians] 
    double eo;  // Equation of origins (ERA - GST) 

    char ra_sign;
    char az_sign;
    int  zd_dmsf[4];
    int  az_dmsf[4];

//  Convert ICRS decimal RA and DEC to radians 
    cat_ra  = icrs_ra  * D2PI / 24;
    cat_dec = icrs_dec * DAS2R;
//    cat_ra_pm  = atan2( icrs_ra_pm * DAS2R, cos( cat_dec ));
//    cat_dec_pm = icrs_dec_pm * DAS2R;

//  Convert observing latitude & longitude to radians
//            sign         deg        min        sec        radians  
    iauAf2a ( obs_lat_sgn, obs_lat_d, obs_lat_m, obs_lat_s, &obs_lat );
    iauAf2a ( obs_lon_sgn, obs_lon_d, obs_lon_m, obs_lon_s, &obs_lon );


//  UTC, TAI and TT date.  NB leap second handling is hardcoded */
    if ( iauDtf2d ( "UTC", year, month, day, hour, min, sec, &utc1, &utc2 ) ) return -1;
    if ( iauUtctai( utc1, utc2, &tai1, &tai2 ) ) return -1;
    if ( iauTaitt ( tai1, tai2, &tt1,  &tt2  ) ) return -1;

//  Convert catalogue ICRS RA,Dec to radians 
//    if ( iauTf2a ( cat_ra_sgn,  cat_ra_h,  cat_ra_m,  cat_ra_s,  &cat_ra  ) ) return -1;
//    if ( iauAf2a ( cat_dec_sgn, cat_dec_d, cat_dec_m, cat_dec_s, &cat_dec ) ) return -1;

//  Parallax (arcsec) and recession speed (km/s). 
//  px = 164.99e-3;
//  rv = 0.0;

    if ( iauAtco13 ( cat_ra, cat_dec, cat_ra_pm, cat_dec_pm, cat_par, cat_vel, 
                             utc1, utc2, dut1,
                             obs_lon, obs_lat, obs_alt, cip_x, cip_y, obs_pres, obs_temp, obs_rh, obs_wl,
                             &az, &zd, &hob, &dob, &rob, &eo ) ) return -1;

//            printf ( "%i\t%i\t%.9f\t%.9f\n", lat_d, lon_d, zd * DR2D, az * DR2D );
            printf ( "%.15f\t%.15f\n", zd * DR2D, az * DR2D );
        }

    }
    return 0;
}
