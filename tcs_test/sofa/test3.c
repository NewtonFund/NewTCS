#include <stdio.h>
#include <stdlib.h>
#include <sofa.h>

//#define DEBUG

/* Report result */
void reprd ( char*, double, double );

int main ( int argc, char *argv[] ) {
    iauASTROM astrom;
    iauLDBODY b[3];

    double latitude ;
    double longitude;

    double phi, elong, hm, phpa, tc, rh, wl, utc1, utc2, tai1, tai2, 
           tt1, tt2, xp, yp, dut1,dx, dy, rc, dc, pr, pd, px, rv, 
           eo, ri, di, rca, dca, ra, da, aot, zot, hot, dot, rot, 
           aob, zob, hob, dob, rob, pvh[2][3], pvb[2][3], r[3][3], x, y, s,
           rob2, dob2;

    char zd_sign;
    char az_sign;
    int  zd_dmsf[4];
    int  az_dmsf[4];
    int  ndp = 8;   

    int i;

/* Inner loop */
    if ( argc > 1 ) 
        i = atoi( argv[1] ); 
    else
        i = 1;

/* Ambient pressure (HPa), temperature (C) and rel. humidity (frac). */
    phpa =   1000.00;
    tc   =   10.00;
    rh   =    0.50;

/*  Difference between UTC and UT1 */
  dut1 = +0.3; // 
//    dut1 = 0.0;

/* Effective color micro metres  */
    wl   = 0.55; 

/* UTC, TAI and TT date.  NB leap second handling is hardcoded */
    if ( iauDtf2d ( "UTC", 2017, 10, 13, 13, 30, 27, &utc1, &utc2 ) ) return -1;
    if ( iauUtctai ( utc1, utc2, &tai1, &tai2 ) ) return -1;
    if ( iauTaitt  ( tai1, tai2, &tt1,  &tt2  ) ) return -1;

#ifdef DEBUG
    printf( "DBG: Time\n" );
    printf( "DBG: UTC= %15.6f\n", utc1 + utc2 );
    printf( "DBG: TT = %15.6f\n", tt1  + tt2  );
    printf( "DBG: TAI= %15.6f\n", tai1 + tai2 );
    printf( "DBG: 𝚫T = %15.6f\n", dut1        );
    puts("");
    return 0; 
#endif



  /* EOPs: polar motion in radians, UT1-UTC in seconds. */
//  xp   = 50.995e-3  * DAS2R;
//  yp   = 376.723e-3 * DAS2R;
    xp = yp =  0;

/* Corrections to IAU 2000A CIP (radians). */
//  dx =  0.269e-3 * DAS2R;
//  dy = -0.274e-3 * DAS2R;
    dx = 0.0;
    dy = 0.0;

/* Star ICRS RA,Dec (radians). */
    if ( iauTf2a ( ' ', 14, 0,  0, &rc ) ) return -1;
    if ( iauAf2a ( ' ', 30, 0,  0, &dc ) ) return -1;

/* Proper motion: RA/Dec derivatives, epoch J2000.0. */
//  pr = atan2 ( -354.45e-3 * DAS2R, cos(dc) );
//  pd = 595.35e-3 * DAS2R;
    pr = 0.0;
    pd = 0.0; 

/* Parallax (arcsec) and recession speed (km/s). */
//  px = 164.99e-3;
//  rv = 0.0;
    px = 0.0;
    rv = 0.0;

/* ICRS to observed. */
/*     Site latitude  & longitude (radians) and height above the geoid (m). */
/*               sign  deg min sec        */
        iauAf2a ( '+',  28,  45,  44.8, &phi   );
        iauAf2a ( '-',  17,  52,  45.2, &elong );
        hm = 2344.0;

        if ( iauAtco13 ( rc, dc, pr,  pd, px, rv, utc1, utc2, dut1,
                       elong, phi,  hm, xp, yp, phpa, tc, rh, wl,
                       &aob, &zob, &hob, &dob, &rob, &eo ) ) return -1;

//      if ( iauAtoc13 ( "A", aob, zob, utc1, utc2, dut1,
//                       elong, phi, hm, xp, yp, phpa, tc, rh, wl,
//                       &rob2, &dob2 ) ) return -1; 

//  reprd ( "ICRS, epoch J2000.0:", rc, dc );
      iauA2af( ndp, DPI/2 - zob, &zd_sign, zd_dmsf );
      iauA2af( ndp, aob, &az_sign, az_dmsf );
//    printf ("  ZD=%15.10f\n  AZ=%15.10f\n", zob * DR2D,  aob * DR2D );
      printf ("%c%3.3i:%2.2i:%2.2i.%*.*i\t",           az_sign, az_dmsf[0], az_dmsf[1], az_dmsf[2], ndp, ndp, az_dmsf[3] );
      printf ("%c%3.3i:%2.2i:%2.2i.%*.*i\n",  zd_sign, zd_dmsf[0], zd_dmsf[1], zd_dmsf[2], ndp, ndp, zd_dmsf[3] );


//  reprd ( "observed -> ICRS:", rob2, dob2 );
  
  return 0;
}


void reprd ( char* s, double ra, double dc ) {
  char pm;
  int i[4];

  printf ( "%25s", s );
  iauA2tf ( 7, ra, &pm, i );
  printf ( " %2.2d %2.2d %2.2d.%7.7d", i[0],i[1],i[2],i[3] );
  iauA2af ( 6, dc, &pm, i );
  printf ( " %c%2.2d %2.2d %2.2d.%6.6d\n", pm, i[0],i[1],i[2],i[3] );
}

