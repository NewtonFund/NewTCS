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

   int i, ii, j, jj;

/* Inner loop */
  if ( argc > 1 ) 
    ii = atoi( argv[1] ); 
  else
    ii = 1;

/* Outer loop */
  if ( argc > 2 ) 
    jj = atoi( argv[2] ); 
  else
    jj = 1;

  printf( "i=%i j=%i\n", ii,jj );

for ( j = jj ; j-- ; ) {

/* Site longitude, latitude (radians) and height above the geoid (m). */
  iauAf2a ( ' ', 98, 29,  7.0, &elong );
  iauAf2a ( ' ', 18, 34, 21.0, &phi   );
  hm = 2457.0;

/* Ambient pressure (HPa), temperature (C) and rel. humidity (frac). */
  phpa = 1000.00;
  tc   =   20.00;
  rh   =    0.90;

/* Effective color (microns). */
  wl   =    0.55;

/* UTC date. */
  if ( iauDtf2d ( "UTC", 2013, 4, 2, 23, 15, 43.55, &utc1, &utc2 ) ) return -1;

/* TT date. */
  if ( iauUtctai ( utc1, utc2, &tai1, &tai2 ) ) return -1;
  if ( iauTaitt  ( tai1, tai2, &tt1,  &tt2  ) ) return -1;

  /* EOPs: polar motion in radians, UT1-UTC in seconds. */
  xp   = 50.995e-3  * DAS2R;
  yp   = 376.723e-3 * DAS2R;
  dut1 = 155.0675e-3;

  /* Corrections to IAU 2000A CIP (radians). */
  dx =  0.269e-3 * DAS2R;
  dy = -0.274e-3 * DAS2R;

/* Star ICRS RA,Dec (radians). */
  if ( iauTf2a ( ' ', 14, 34, 16.81183, &rc ) ) return -1;
  if ( iauAf2a ( '-', 12, 31, 10.3965,  &dc ) ) return -1;

/* Proper motion: RA/Dec derivatives, epoch J2000.0. */
  pr = atan2 ( -354.45e-3 * DAS2R, cos(dc) );
  pd = 595.35e-3 * DAS2R;

/* Parallax (arcsec) and recession speed (km/s). */
  px = 164.99e-3;
  rv = 0.0;


/* ICRS to observed. */
  for ( i=ii ; i-- ; ) {
    if ( iauAtco13 ( rc, dc, pr,  pd, px, rv, utc1, utc2, dut1,
                   elong, phi,  hm, xp, yp, phpa, tc, rh, wl,
                   &aob, &zob, &hob, &dob, &rob, &eo ) ) return -1;

    if ( iauAtoc13 ( "A", aob, zob, utc1, utc2, dut1,
                   elong, phi, hm, xp, yp, phpa, tc, rh, wl,
                   &rob2, &dob2 ) ) return -1; 
    }
  }

  reprd ( "ICRS, epoch J2000.0:", rc, dc );
  reprd ( "ICRS -> observed:", aob, zob );
  reprd ( "observed -> ICRS:", rob2, dob2 );
  
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

