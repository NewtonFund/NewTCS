/*  SOFA Transform of ICRS -> observed Zenith Distance and Azimuth
 *  Full World topographic latitudes and longitudes in 1 degree steps
 *
 *  Excludes atmospheric effects
 *
 *  Author: A.S.Piascik 
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
    int    month      = 1;
    int    day        = 1;
    int    hour       = 0;
    int    min        = 0;
    double sec        = 0.0;

//  Target object details in ICRS units
    double icrs_ra     = 12.0; // RA [decimal]
    double icrs_ra_pm  =  0.0; // RA proper motion [mas/year] 
    double icrs_dec    =    0; // DEC [decimal]
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
    char   obs_lat_sgn=  '-';
    int    obs_lat_d  =    0; // Lat. [degrees]
    int    obs_lat_m  =    0; // Lat. [minutes]
    double obs_lat_s  =    0; // Lat. [seconds]
    double obs_lat;           // Lat. [radians]

    char   obs_lon_sgn=  '-';
    int    obs_lon_d  =    0; // Long. [degrees]
    int    obs_lon_m  =    0; // Long. [minutes]
    double obs_lon_s  =    0; // Long. [seconds]
    double obs_lon;           // Long. [radians] 

    double obs_alt    =  0.0; // Altitude [metres]
    double obs_temp   = 20.0; // Temperature [Celcius]
    double obs_pres   =  0.0; // Pressure [mbar]
    double obs_rh     =  0.5; // Relative humidity [0.0-1.0]
    double obs_wl     = 1.00; // Wavelength [um]

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

//  UTC, TAI and TT date.  NB leap second handling is hardcoded */
    if ( iauDtf2d ( "UTC", year, month, day, hour, min, sec, &utc1, &utc2 ) ) return -1;
    if ( iauUtctai( utc1, utc2, &tai1, &tai2 ) ) return -1;
    if ( iauTaitt ( tai1, tai2, &tt1,  &tt2  ) ) return -1;

    for ( lat_d = -90; lat_d <= 90; lat_d += 1  ) {
//      Handle sign for southern hemisphere
        obs_lat_sgn = lat_d < 0 ? '-' : '+'; 
        obs_lat_d   = abs(lat_d);

//      Handle sign for westings
        for ( lon_d = -180; lon_d <= 180 ; lon_d += 1  ) {
            obs_lon_sgn =  lon_d < 0 ? '-' : '+';
            obs_lon_d   = abs(lon_d);

//          Convert observing latitude & longitude to radians
//                    sign         deg        min        sec        radians  
            iauAf2a ( obs_lat_sgn, obs_lat_d, obs_lat_m, obs_lat_s, &obs_lat );
            iauAf2a ( obs_lon_sgn, obs_lon_d, obs_lon_m, obs_lon_s, &obs_lon );

            if ( iauAtco13 ( cat_ra, cat_dec, cat_ra_pm, cat_dec_pm, cat_par, cat_vel, 
                             utc1, utc2, dut1,
                             obs_lon, obs_lat, obs_alt, cip_x, cip_y, obs_pres, obs_temp, obs_rh, obs_wl,
                             &az, &zd, &hob, &dob, &rob, &eo ) ) return -1;

//          For standalone version we prefix the observatory latitude and longitude 
//            printf ( "%i\t%i\t%.9f\t%.9f\n", lat_d, lon_d, zd * DR2D, az * DR2D );

//          When combining with NOVAS data only the zenith distance and azimuth angle are needed as lat/lon columns already generated
            printf ( "%.15f\t%.15f\n", zd * DR2D, az * DR2D );
        }
        puts("");
    }
    return 0;
}
