/* gpsutils.c -- code shared between low-level and high-level interfaces
 *
 * This file is Copyright (c) 2010 by the GPSD project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 */

/* The strptime prototype is not provided unless explicitly requested.
 * We also need to set the value high enough to signal inclusion of
 * newer features (like clock_gettime).  See the POSIX spec for more info:
 * http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_02_01_02 */
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <ctype.h>

#include "gpsd.h"  /* could be gps.h if we didn't need splint decorations */
#include "libgps.h"

/*
 * Berkeley implementation of strtod(), inlined to avoid locale problems
 * with the decimal point and stripped down to an atof()-equivalent.
 */

/*@-shiftimplementation +charint@*/
double safe_atof(const char *string)
/* Takes a decimal ASCII floating-point number, optionally
 * preceded by white space.  Must have form "-I.FE-X",
 * where I is the integer part of the mantissa, F is
 * the fractional part of the mantissa, and X is the
 * exponent.  Either of the signs may be "+", "-", or
 * omitted.  Either I or F may be omitted, or both.
 * The decimal point isn't necessary unless F is
 * present.  The "E" may actually be an "e".  E and X
 * may both be omitted (but not just one).
 */
{
    static int maxExponent = 511;   /* Largest possible base 10 exponent.  Any
				     * exponent larger than this will already
				     * produce underflow or overflow, so there's
				     * no need to worry about additional digits.
				     */
    static double powersOf10[] = {  /* Table giving binary powers of 10.  Entry */
	10.,			/* is 10^2^i.  Used to convert decimal */
	100.,			/* exponents into floating-point numbers. */
	1.0e4,
	1.0e8,
	1.0e16,
	1.0e32,
	1.0e64,
	1.0e128,
	1.0e256
    };

    bool sign, expSign = false;
    double fraction, dblExp, *d;
    register const char *p;
    register int c;
    int exp = 0;		/* Exponent read from "EX" field. */
    int fracExp = 0;		/* Exponent that derives from the fractional
				 * part.  Under normal circumstatnces, it is
				 * the negative of the number of digits in F.
				 * However, if I is very long, the last digits
				 * of I get dropped (otherwise a long I with a
				 * large negative exponent could cause an
				 * unnecessary overflow on I alone).  In this
				 * case, fracExp is incremented one for each
				 * dropped digit. */
    int mantSize;		/* Number of digits in mantissa. */
    int decPt;			/* Number of mantissa digits BEFORE decimal
				 * point. */
    const char *pExp;		/* Temporarily holds location of exponent
				 * in string. */

    /*
     * Strip off leading blanks and check for a sign.
     */

    p = string;
    while (isspace(*p)) {
	p += 1;
    }
    if (*p == '-') {
	sign = true;
	p += 1;
    } else {
	if (*p == '+') {
	    p += 1;
	}
	sign = false;
    }

    /*
     * Count the number of digits in the mantissa (including the decimal
     * point), and also locate the decimal point.
     */

    decPt = -1;
    for (mantSize = 0; ; mantSize += 1)
    {
	c = *p;
	if (!isdigit(c)) {
	    if ((c != '.') || (decPt >= 0)) {
		break;
	    }
	    decPt = mantSize;
	}
	p += 1;
    }

    /*
     * Now suck up the digits in the mantissa.  Use two integers to
     * collect 9 digits each (this is faster than using floating-point).
     * If the mantissa has more than 18 digits, ignore the extras, since
     * they can't affect the value anyway.
     */

    pExp  = p;
    p -= mantSize;
    if (decPt < 0) {
	decPt = mantSize;
    } else {
	mantSize -= 1;			/* One of the digits was the point. */
    }
    if (mantSize > 18) {
	fracExp = decPt - 18;
	mantSize = 18;
    } else {
	fracExp = decPt - mantSize;
    }
    if (mantSize == 0) {
	fraction = 0.0;
	//p = string;
	goto done;
    } else {
	int frac1, frac2;
	frac1 = 0;
	for ( ; mantSize > 9; mantSize -= 1)
	{
	    c = *p;
	    p += 1;
	    if (c == '.') {
		c = *p;
		p += 1;
	    }
	    frac1 = 10*frac1 + (c - '0');
	}
	frac2 = 0;
	for (; mantSize > 0; mantSize -= 1)
	{
	    c = *p;
	    p += 1;
	    if (c == '.') {
		c = *p;
		p += 1;
	    }
	    frac2 = 10*frac2 + (c - '0');
	}
	fraction = (1.0e9 * frac1) + frac2;
    }

    /*
     * Skim off the exponent.
     */

    p = pExp;
    if ((*p == 'E') || (*p == 'e')) {
	p += 1;
	if (*p == '-') {
	    expSign = true;
	    p += 1;
	} else {
	    if (*p == '+') {
		p += 1;
	    }
	    expSign = false;
	}
	while (isdigit(*p)) {
	    exp = exp * 10 + (*p - '0');
	    p += 1;
	}
    }
    if (expSign) {
	exp = fracExp - exp;
    } else {
	exp = fracExp + exp;
    }

    /*
     * Generate a floating-point number that represents the exponent.
     * Do this by processing the exponent one bit at a time to combine
     * many powers of 2 of 10. Then combine the exponent with the
     * fraction.
     */

    if (exp < 0) {
	expSign = true;
	exp = -exp;
    } else {
	expSign = false;
    }
    if (exp > maxExponent) {
	exp = maxExponent;
	errno = ERANGE;
    }
    dblExp = 1.0;
    for (d = powersOf10; exp != 0; exp >>= 1, d += 1) {
	if (exp & 01) {
	    dblExp *= *d;
	}
    }
    if (expSign) {
	fraction /= dblExp;
    } else {
	fraction *= dblExp;
    }

done:
    if (sign) {
	return -fraction;
    }
    return fraction;
}
/*@+shiftimplementation -charint@*/

#define MONTHSPERYEAR	12	/* months per calendar year */

void gps_clear_fix( /*@out@*/ struct gps_fix_t *fixp)
/* stuff a fix structure with recognizable out-of-band values */
{
    fixp->time = NAN;
    fixp->mode = MODE_NOT_SEEN;
    fixp->latitude = fixp->longitude = NAN;
    fixp->track = NAN;
    fixp->speed = NAN;
    fixp->climb = NAN;
    fixp->altitude = NAN;
    fixp->ept = NAN;
    fixp->epx = NAN;
    fixp->epy = NAN;
    fixp->epv = NAN;
    fixp->epd = NAN;
    fixp->eps = NAN;
    fixp->epc = NAN;
}

void gps_clear_dop( /*@out@*/ struct dop_t *dop)
{
    dop->xdop = dop->ydop = dop->vdop = dop->tdop = dop->hdop = dop->pdop =
        dop->gdop = NAN;
}

void gps_merge_fix( /*@ out @*/ struct gps_fix_t *to,
		   gps_mask_t transfer,
		   /*@ in @*/ struct gps_fix_t *from)
/* merge new data into an old fix */
{
    if ((NULL == to) || (NULL == from))
	return;
    if ((transfer & TIME_SET) != 0)
	to->time = from->time;
    if ((transfer & LATLON_SET) != 0) {
	to->latitude = from->latitude;
	to->longitude = from->longitude;
    }
    if ((transfer & MODE_SET) != 0)
	to->mode = from->mode;
    if ((transfer & ALTITUDE_SET) != 0)
	to->altitude = from->altitude;
    if ((transfer & TRACK_SET) != 0)
	to->track = from->track;
    if ((transfer & SPEED_SET) != 0)
	to->speed = from->speed;
    if ((transfer & CLIMB_SET) != 0)
	to->climb = from->climb;
    if ((transfer & TIMERR_SET) != 0)
	to->ept = from->ept;
    if ((transfer & HERR_SET) != 0) {
	to->epx = from->epx;
	to->epy = from->epy;
    }
    if ((transfer & VERR_SET) != 0)
	to->epv = from->epv;
    if ((transfer & SPEEDERR_SET) != 0)
	to->eps = from->eps;
}

timestamp_t timestamp(void)
{
#ifdef HAVE_CLOCK_GETTIME
     struct timespec ts;
     /*@i2@*/(void)clock_gettime(CLOCK_REALTIME, &ts);
     /*@i3@*/return (timestamp_t)(ts.tv_sec + ts.tv_nsec * 1e-9);
#else
    struct timeval tv;
    (void)gettimeofday(&tv, NULL);
    return (timestamp_t)(tv.tv_sec + tv.tv_usec * 1e-6);
#endif
}

time_t mkgmtime(register struct tm * t)
/* struct tm to seconds since Unix epoch */
{
    register int year;
    register time_t result;
    static const int cumdays[MONTHSPERYEAR] =
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

    /*@ +matchanyintegral @*/
    year = 1900 + t->tm_year + t->tm_mon / MONTHSPERYEAR;
    result = (year - 1970) * 365 + cumdays[t->tm_mon % MONTHSPERYEAR];
    result += (year - 1968) / 4;
    result -= (year - 1900) / 100;
    result += (year - 1600) / 400;
    if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) &&
	(t->tm_mon % MONTHSPERYEAR) < 2)
	result--;
    result += t->tm_mday - 1;
    result *= 24;
    result += t->tm_hour;
    result *= 60;
    result += t->tm_min;
    result *= 60;
    result += t->tm_sec;
    /*@ -matchanyintegral @*/
    return (result);
}

timestamp_t iso8601_to_unix( /*@in@*/ char *isotime)
/* ISO8601 UTC to Unix UTC */
{
    char *dp = NULL;
    double usec;
    struct tm tm;

    /*@i1@*/ dp = strptime(isotime, "%Y-%m-%dT%H:%M:%S", &tm);
    if (dp != NULL && *dp == '.')
	usec = strtod(dp, NULL);
    else
	usec = 0;
    return (timestamp_t)mkgmtime(&tm) + usec;
}

/* *INDENT-OFF* */
/*@observer@*/char *unix_to_iso8601(timestamp_t fixtime, /*@ out @*/
				     char isotime[], size_t len)
/* Unix UTC time to ISO8601, no timezone adjustment */
/* example: 2007-12-11T23:38:51.033Z */
{
    struct tm when;
    double integral, fractional;
    time_t intfixtime;
    char timestr[30];
    char fractstr[10];

    fractional = modf(fixtime, &integral);
    intfixtime = (time_t) integral;
    (void)gmtime_r(&intfixtime, &when);

    (void)strftime(timestr, sizeof(timestr), "%Y-%m-%dT%H:%M:%S", &when);
    /*
     * Do not mess casually with the number of decimal digits in the
     * format!  Most GPSes report over serial links at 0.01s or 0.001s
     * precision.
     */
    (void)snprintf(fractstr, sizeof(fractstr), "%.3f", fractional);
    /* add fractional part, ignore leading 0; "0.2" -> ".2" */
    /*@i2@*/(void)snprintf(isotime, len, "%s%sZ",timestr, strchr(fractstr,'.'));
    return isotime;
}
/* *INDENT-ON* */

#define Deg2Rad(n)	((n) * DEG_2_RAD)

/* Distance in meters between two points specified in degrees, optionally
with initial and final bearings. */
/*@-mustdefine@*/
double earth_distance_and_bearings(double lat1, double lon1, double lat2, double lon2, double *ib, double *fb)
{
    /*
     * this is a translation of the javascript implementation of the
     * Vincenty distance formula by Chris Veness. See
     * http://www.movable-type.co.uk/scripts/latlong-vincenty.html
     */
    double a, b, f;		// WGS-84 ellipsoid params
    double L, L_P, U1, U2, s_U1, c_U1, s_U2, c_U2;
    double uSq, A, B, d_S, lambda;
    double s_L, c_L, s_S, C;
    double c_S, S, s_A, c_SqA, c_2SM;
    int i = 100;

    a = WGS84A;
    b = WGS84B;
    f = 1 / WGS84F;
    L = Deg2Rad(lon2 - lon1);
    U1 = atan((1 - f) * tan(Deg2Rad(lat1)));
    U2 = atan((1 - f) * tan(Deg2Rad(lat2)));
    s_U1 = sin(U1);
    c_U1 = cos(U1);
    s_U2 = sin(U2);
    c_U2 = cos(U2);
    lambda = L;

    do {
	s_L = sin(lambda);
	c_L = cos(lambda);
	s_S = sqrt((c_U2 * s_L) * (c_U2 * s_L) +
		   (c_U1 * s_U2 - s_U1 * c_U2 * c_L) *
		   (c_U1 * s_U2 - s_U1 * c_U2 * c_L));

	if (s_S == 0)
	    return 0;

	c_S = s_U1 * s_U2 + c_U1 * c_U2 * c_L;
	S = atan2(s_S, c_S);
	s_A = c_U1 * c_U2 * s_L / s_S;
	c_SqA = 1 - s_A * s_A;
	c_2SM = c_S - 2 * s_U1 * s_U2 / c_SqA;

	if (isnan(c_2SM))
	    c_2SM = 0;

	C = f / 16 * c_SqA * (4 + f * (4 - 3 * c_SqA));
	L_P = lambda;
	lambda = L + (1 - C) * f * s_A *
	    (S + C * s_S * (c_2SM + C * c_S * (2 * c_2SM * c_2SM - 1)));
    } while ((fabs(lambda - L_P) > 1.0e-12) && (--i > 0));

    if (i == 0)
	return NAN;		// formula failed to converge

    uSq = c_SqA * ((a * a) - (b * b)) / (b * b);
    A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    d_S = B * s_S * (c_2SM + B / 4 *
		     (c_S * (-1 + 2 * c_2SM * c_2SM) - B / 6 * c_2SM *
		      (-3 + 4 * s_S * s_S) * (-3 + 4 * c_2SM * c_2SM)));

    if (ib != NULL)
	*ib = atan2(c_U2 * sin(lambda), c_U1 * s_U2 - s_U1 * c_U2 * cos(lambda));
    if (fb != NULL)
	*fb = atan2(c_U1 * sin(lambda), c_U1 * s_U2 * cos(lambda) - s_U1 * c_U2);

    return (WGS84B * A * (S - d_S));
}
/*@+mustdefine@*/

/* Distance in meters between two points specified in degrees. */
double earth_distance(double lat1, double lon1, double lat2, double lon2)
{
	return earth_distance_and_bearings(lat1, lon1, lat2, lon2, NULL, NULL);
}

