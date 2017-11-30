#include <stdio.h>
#include <stdlib.h>
#include <sofa.h>

/* Report result */
void reprd ( char*, double, double );

int main ( int argc, char *argv[] ) {
    iauASTROM astrom;
    iauLDBODY b[3];

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

    int i, ii;

/* Inner loop */
    if ( argc > 1 ) 
      ii = atoi( argv[1] ); 
    else
      ii = 1;

/* Site latitude  & longitude (radians) and height above the geoid (m). */
/*           sign  deg min sec        */
    iauAf2a ( ' ',  90,  0,  0, &phi   );
    iauAf2a ( ' ',   0,  0,  0, &elong );
    hm = 0.0;

/* Ambient pressure (HPa), temperature (C) and rel. humidity (frac). */
    phpa =    0.00;
    tc   =   20.00;
    rh   =    0.50;

/*  Differnce between UTC and UT1 */
//  dut1 = 155.0675e-3;
//   dut1 = -0.387845;
    dut1 = 0.0;

/* Effective color micro metres  */
    wl   =    0.55; 

/* UTC, TAI and TT date. */
    if ( iauDtf2d ( "UTC", 2017, 1, 1, 0, 0, 0, &utc1, &utc2 ) ) return -1;
    if ( iauUtctai ( utc1, utc2, &tai1, &tai2 ) ) return -1;
    if ( iauTaitt  ( tai1, tai2, &tt1,  &tt2  ) ) return -1;

    printf( "Time\n" );
    printf( "  UTC= %15.6f\n", utc1 + utc2 );
    printf( "  TT = %15.6f\n", tt1  + tt2  );
    printf( "  UT1= %15.6f\n", tt1  + tt2  );
//  printf( "  𝚫T = %15.6f\n", delta_t     );
    puts("");

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
    if ( iauTf2a ( ' ', 12.0, 0.0,  0.0, &rc ) ) return -1;
    if ( iauAf2a ( ' ', 90.0, 0.0,  0.0, &dc ) ) return -1;

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
    for ( i=ii ; i-- ; ) {
      if ( iauAtco13 ( rc, dc, pr,  pd, px, rv, utc1, utc2, dut1,
                       elong, phi,  hm, xp, yp, phpa, tc, rh, wl,
                       &aob, &zob, &hob, &dob, &rob, &eo ) ) return -1;

//      if ( iauAtoc13 ( "A", aob, zob, utc1, utc2, dut1,
//                       elong, phi, hm, xp, yp, phpa, tc, rh, wl,
//                       &rob2, &dob2 ) ) return -1; 
    }

//  reprd ( "ICRS, epoch J2000.0:", rc, dc );
  
    iauA2af( ndp, zob, &zd_sign, zd_dmsf );
    iauA2af( ndp, aob, &az_sign, az_dmsf );
    printf ("FK6 zenith dist. and azimuth:\n");
    printf ("  ZD=%15.10f\n  AZ=%15.10f\n", zob * DR2D,  aob * DR2D );
    printf ("  ZD=%c%3.3i:%2.2i:%2.2i.%*.*i\n", zd_sign, zd_dmsf[0], zd_dmsf[1], zd_dmsf[2], ndp, ndp, zd_dmsf[3] );
    printf ("  AZ=%c%3.3i:%2.2i:%2.2i.%*.*i\n", az_sign, az_dmsf[0], az_dmsf[1], az_dmsf[2], ndp, ndp, az_dmsf[3] );
    printf ("\n");


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

