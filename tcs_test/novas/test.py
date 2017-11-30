# -*- coding: utf-8 -*-
import sys
import numpy as np
from astropy import units as u
from astropy.coordinates import Angle


from math import sqrt, sin, cos
from novas.compat import *
from novas.compat.eph_manager import ephem_open;
from novas.constants import T0, DEG2RAD

#from astropy import units as u

year        = 2013
month       = 1
day         = 1 
hour        = 0.0 
leap_secs   = 33.0
accuracy    = 0 
error       = 0

ut1_utc     = -0.387845

latitude    = 18.5 
longitude   = 98.5 
height      =  0.0
temperature = 20.0
pressure    = 1000.0

x_pole      = 0.0 
y_pole      = 0.0

argc = len(sys.argv)
i = argv[1] if argc > 1 else 1
accuracy = 1 if argc > 2 and argv[1] == 1 else 0

# Establish time arguments
jd_utc  = julian_date(year, month, day, hour)
jd_tt   = jd_utc + (leap_secs + 32.184) / 86400.0
jd_ut1  = jd_utc + ut1_utc / 86400.0
delta_t = 32.184 + leap_secs - ut1_utc
jd_tdb  = jd_tt 

geo_loc = make_on_surface( latitude, longitude, height, temperature, pressure )
obs_loc = make_observer_on_surface( latitude, longitude, height, temperature, pressure )

print 'Geodetic location:\n' \
      '  Long.= {0:.6f}\n'.format(geo_loc.longitude), \
      '  Lat. = {0:.6f}\n'.format(geo_loc.latitude),  \
      '  Hght.= {0:.6f}\n'.format(geo_loc.height), \
      '  𝚫T   = {0:.6f}\n'.format(delta_t)  


print 'Time:\n' \
      '  UTC= {0:.10f}\n'.format(jd_utc), \
      '  TT = {0:.10f}\n'.format(jd_tt),  \
      '  UT1= {0:.10f}\n'.format(jd_ut1), \
      '  𝚫T = {0:.10f}\n'.format(delta_t)  

# Make a test star
star = make_cat_entry( "TEST", "FK6", 0, 14.5, 12.5, 0.0, 0.0, 0.0, 0.0 )

jd_beg, jd_end, de_num = ephem_open("/var/local/novas/DE405.bin")

while i:
    geo_loc   = make_on_surface( latitude, longitude, height, temperature, pressure )
    obs_loc   = make_observer_on_surface( latitude, longitude, height, temperature, pressure )
    ra,  dec  = app_star(  jd_tt, star, accuracy )
    rat, dect = topo_star( jd_tt, delta_t, star, geo_loc, accuracy ) 
    zdaz, rardecr = equ2hor( jd_ut1, delta_t, 0.0, 0.0, geo_loc, rat, dect, 1, accuracy )
    i -= 1
 
zd = Angle(zdaz[0], u.degree )
az = Angle(zdaz[1], u.degree )
print '  ZD= ', zd.to_string(unit=u.degree, sep=':')
print '  AZ= ', az.to_string(unit=u.degree, sep=':')
print (rardecr)
