/**
 * DOUBLE PRECISION APPROXIMATION FOR ARCTAN AND COSINUS
 * source: https://people.sc.fsu.edu/~jburkardt/c_src/fn/fn.html
 */

double RAYX_API r8_aint(double x)

/******************************************************************************/
/*
  Purpose:

    R8_AINT truncates an R8 argument to an integer.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    John Burkardt.

  Parameters:

    Input, double X, the argument.

    Output, double R8_AINT, the truncated version of X.
*/
{
    double value;

    if (x < 0.0E+00) {
        value = -(double(int(abs(x))));
    } else {
        value = (double(int(abs(x))));
    }
    return value;
}

int RAYX_API r8_inits(double dos[16], int nos, double eta)
/******************************************************************************/
/*
  Purpose:

    R8_INITS initializes a Chebyshev series.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    22 April 2016

  Author:

    C version by John Burkardt.

  Reference:

    Roger Broucke,
    Algorithm 446:
    Ten Subroutines for the Manipulation of Chebyshev Series,
    Communications of the ACM,
    Volume 16, Number 4, April 1973, pages 254-256.

  Parameters:

    Input, double DOS[NOS], the Chebyshev coefficients.

    Input, int NOS, the number of coefficients.

    Input, double ETA, the desired accuracy.

    Output, int R8_INITS, the number of terms of the series needed
    to ensure the requested accuracy.
*/
{
    double err;
    int i;
    int value;

    if (nos < 1) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_INITS - Fatal error!\n" );
        // fprintf ( stderr, "  Number of coefficients < 1.\n" );
        return 1;
    }

    if (eta < dos[nos - 1]) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_INITS - Warning!\n" );
        // fprintf ( stderr, "  ETA may be too small.\n" );
        // fprintf ( stderr, "  The requested accuracy cannot be guaranteed.\n" );
        // fprintf ( stderr, "  even if all available coefficients are used.\n" );
        value = nos;
    } else {
        err = 0.0;

        for (i = nos - 1; 0 <= i; i--) {
            value = i + 1;
            err   = err + abs(dos[i]);
            if (eta < err) {
                break;
            }
        }
    }

    return value;
}

void RAYX_API r8_upak(double x, RAYX_INOUT(double) y, RAYX_INOUT(int) n)
/******************************************************************************/
/*
  Purpose:

    R8_UPAK unpacks an R8 into a mantissa and exponent.

  Discussion:

    This function unpacks a floating point number x so that

      x = y * 2.0^n

    where

      0.5 <= abs ( y ) < 1.0 .

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    C version by John Burkardt.

  Parameters:

    Input, double X, the number to be unpacked.

    Output, double *Y, the mantissa.

    Output, int *N, the exponent.
*/
{
    double absx;

    absx = abs(x);
    n    = 0;
    y    = 0.0;

    if (x == 0.0) {
        return;
    }

    while (absx < 0.5) {
        n    = n - 1;
        absx = absx * 2.0;
    }

    while (1.0 <= absx) {
        n    = n + 1;
        absx = absx * 0.5;
    }

    if (x < 0.0) {
        y = -absx;
    } else {
        y = +absx;
    }

    return;
}

int i4_mach(int i)

/******************************************************************************/
/*
  Purpose:

    I4_MACH returns integer machine constants.

  Discussion:

    Input/output unit numbers.

      I4_MACH(1) = the standard input unit.
      I4_MACH(2) = the standard output unit.
      I4_MACH(3) = the standard punch unit.
      I4_MACH(4) = the standard error message unit.

    Words.

      I4_MACH(5) = the number of bits per integer storage unit.
      I4_MACH(6) = the number of characters per integer storage unit.

    Integers.

    Assume integers are represented in the S digit base A form:

      Sign * (X(S-1)*A^(S-1) + ... + X(1)*A + X(0))

    where 0 <= X(1:S-1) < A.

      I4_MACH(7) = A, the base.
      I4_MACH(8) = S, the number of base A digits.
      I4_MACH(9) = A^S-1, the largest integer.

    Floating point numbers

    Assume floating point numbers are represented in the T digit
    base B form:

      Sign * (B^E) * ((X(1)/B) + ... + (X(T)/B^T) )

    where 0 <= X(I) < B for I=1 to T, 0 < X(1) and EMIN <= E <= EMAX.

      I4_MACH(10) = B, the base.

    Single precision

      I4_MACH(11) = T, the number of base B digits.
      I4_MACH(12) = EMIN, the smallest exponent E.
      I4_MACH(13) = EMAX, the largest exponent E.

    Double precision

      I4_MACH(14) = T, the number of base B digits.
      I4_MACH(15) = EMIN, the smallest exponent E.
      I4_MACH(16) = EMAX, the largest exponent E.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    24 April 2007

  Author:

    Original FORTRAN77 version by Phyllis Fox, Andrew Hall, Norman Schryer.
    C version by John Burkardt.

  Reference:

    Phyllis Fox, Andrew Hall, Norman Schryer,
    Algorithm 528,
    Framework for a Portable Library,
    ACM Transactions on Mathematical Software,
    Volume 4, Number 2, June 1978, page 176-188.

  Parameters:

    Input, int I, chooses the parameter to be returned.
    1 <= I <= 16.

    Output, int I4_MACH, the value of the chosen parameter.
*/
{
    int return_values[18] = {1, 5, 6, 7, 6, 32, 4, 2, 31, 2147483647, 2, 24, -125, 128, 53, -1021, 1024, 0};

    if (i < 1) {
        // fprintf ( stderr, "  The input argument I is out of bounds.\n" );
        // fprintf ( stderr, "  Legal values satisfy 1 <= I <= 16.\n" );
        // fprintf ( stderr, "  I = %d\n", i );
        return 1;
    } else if (16 < i) {
        // fprintf ( stderr, "  The input argument I is out of bounds.\n" );
        // fprintf ( stderr, "  Legal values satisfy 1 <= I <= 16.\n" );
        // fprintf ( stderr, "  I = %d\n", i );
        return 0;
    }

    return return_values[i];
}

double RAYX_API r8_mach(int i)

/******************************************************************************/
/*
  Purpose:

    R8_MACH returns double precision real machine constants.

  Discussion:

    Assuming that the internal representation of a double precision real
    number is in base B, with T the number of base-B digits in the mantissa,
    and EMIN the smallest possible exponent and EMAX the largest possible
    exponent, then

      R8_MACH(1) = B^(EMIN-1), the smallest positive magnitude.
      R8_MACH(2) = B^EMAX*(1-B^(-T)), the largest magnitude.
      R8_MACH(3) = B^(-T), the smallest relative spacing.
      R8_MACH(4) = B^(1-T), the largest relative spacing.
      R8_MACH(5) = log10(B).

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    24 April 2007

  Author:

    Original FORTRAN77 version by Phyllis Fox, Andrew Hall, Norman Schryer.
    C version by John Burkardt.

  Reference:

    Phyllis Fox, Andrew Hall, Norman Schryer,
    Algorithm 528:
    Framework for a Portable Library,
    ACM Transactions on Mathematical Software,
    Volume 4, Number 2, June 1978, page 176-188.

  Parameters:

    Input, int I, chooses the parameter to be returned.
    1 <= I <= 5.

    Output, double R8_MACH, the value of the chosen parameter.
*/
{
    double return_values[6] =
        {0, 4.450147717014403E-308, 8.988465674311579E+307, 1.110223024625157E-016, 2.220446049250313E-016, 0.301029995663981E+000};
    return return_values[i];
}

double RAYX_API r8_pak(double y, int n)

/******************************************************************************/
/*
  Purpose:

    R8_PAK packs a base 2 exponent into an R8.

  Discussion:

    This routine is almost the inverse of R8_UPAK.  It is not exactly
    the inverse, because abs(x) need not be between 0.5 and 1.0.
    If both R8_PAK and 2.0^n were known to be in range, we could compute
    R8_PAK = x * 2.0^n .

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    C version by John Burkardt.

  Parameters:

    Input, double Y, the mantissa.

    Input, int N, the exponent.

    Output, double R8_PAK, the packed value.
*/
{
    const double aln210 = 3.321928094887362347870319429489;
    double aln2b;
    int nmax = 0;
    int nmin = 0;
    int nsum;
    int ny       = 0;
    double value = 0;

    if (nmin == 0) {
        aln2b = 1.0;
        if (i4_mach(10) != 2) {
            aln2b = r8_mach(5) * aln210;
        }
        nmin = int(aln2b * i4_mach(15));
        nmax = int(aln2b * i4_mach(16));
    }

    r8_upak(y, value, ny);

    nsum = n + ny;

    if (nsum < nmin) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_PAK - Warning!\n" );
        // fprintf ( stderr, "  Packed number underflows.\n" );
        value = 0.0;
        return value;
    }

    if (nmax < nsum) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_PAK - Fatal error!\n" );
        // fprintf ( stderr, "  Packed number overflows.\n" );
        return (1);
    }

    while (nsum < 0) {
        value = 0.5 * value;
        nsum  = nsum + 1;
    }

    while (0 < nsum) {
        value = 2.0 * value;
        nsum  = nsum - 1;
    }
    return value;
}

double RAYX_API r8_csevl(double x, double a[16], int n)

/******************************************************************************/
/*
  Purpose:

    R8_CSEVL evaluates a Chebyshev series.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    C version by John Burkardt.

  Reference:

    Roger Broucke,
    Algorithm 446:
    Ten Subroutines for the Manipulation of Chebyshev Series,
    Communications of the ACM,
    Volume 16, Number 4, April 1973, pages 254-256.

  Parameters:

    Input, double X, the evaluation point.

    Input, double CS[N], the Chebyshev coefficients.

    Input, int N, the number of Chebyshev coefficients.

    Output, double R8_CSEVL, the Chebyshev series evaluated at X.
*/
{
    double b0;
    double b1;
    double b2;
    int i;
    double twox;
    double value;

    if (n < 1) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_CSEVL - Fatal error!\n" );
        // fprintf ( stderr, "  Number of terms <= 0.\n" );
        return 1;
    }

    if (1000 < n) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_CSEVL - Fatal error!\n" );
        // fprintf ( stderr, "  Number of terms greater than 1000.\n" );
        return 1;
    }

    if (x < -1.1 || 1.1 < x) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_CSEVL - Fatal error!\n" );
        // fprintf ( stderr, "  X outside (-1,+1).\n" );
        return 1;
    }

    twox = 2.0 * x;
    b1   = 0.0;
    b0   = 0.0;

    for (i = n - 1; 0 <= i; i--) {
        b2 = b1;
        b1 = b0;
        b0 = twox * b1 - b2 + a[i];
    }

    value = 0.5 * (b0 - b2);
    return value;
}

double RAYX_API r8_cos(const double x)
/******************************************************************************/
/*
  Purpose:

    R8_COS evaluates the cosine of an R8 argument.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    Original FORTRAN77 version by Wayne Fullerton.
    C version by John Burkardt.

  Reference:

    Wayne Fullerton,
    Portable Special Function Routines,
    in Portability of Numerical Software,
    edited by Wayne Cowell,
    Lecture Notes in Computer Science, Volume 57,
    Springer 1977,
    ISBN: 978-3-540-08446-4,
    LC: QA297.W65.

  Parameters:

    Input, double X, the argument.

    Output, double R8_COS, the cosine of X.
*/
{
    double absx;
    double f;
    int n2;
    int ntsn            = 0;
    const double pi2    = 1.57079632679489661923132169163975;
    const double pi2rec = 0.63661977236758134307553505349006;
    const double pihi   = 3.140625;
    const double pilo   = 9.6765358979323846264338327950288E-04;
    const double pirec  = 0.31830988618379067153776752674503;
    double sincs[16] =
        {-0.374991154955873175839919279977323464, -0.181603155237250201863830316158004754, 0.005804709274598633559427341722857921,
                   -0.000086954311779340757113212316353178, 0.000000754370148088851481006839927030, -0.000000004267129665055961107126829906,
                   0.000000000016980422945488168181824792, -0.000000000000050120578889961870929524, 0.000000000000000114101026680010675628,
                   -0.000000000000000000206437504424783134, 0.000000000000000000000303969595918706, -0.000000000000000000000000371357734157,
                   0.000000000000000000000000000382486123, -0.000000000000000000000000000000336623, 0.000000000000000000000000000000000256, 0};
    double value;
    double xmax = 0.0;
    double xn;
    double xsml  = 0.0;
    double y;

    if (ntsn == 0) {
        ntsn  = r8_inits(sincs, 15, 0.1 * r8_mach(3));
        xsml  = sqrt(2.0 * r8_mach(3));
        xmax  = 1.0 / r8_mach(4);
    }

    absx = abs(x);
    y    = absx + pi2;

    if (xmax < y) {
        value = 0.0;
        return value;
    }

    value = 1.0;

    if (absx < xsml) {
        return value;
    }

    xn = floor(y * pirec + 0.5);
    n2 = int(mod(xn, 2.0) + 0.5);
    xn = xn - 0.5;
    f  = (absx - xn * pihi) - xn * pilo;

    xn    = 2.0 * (f * pi2rec) * (f * pi2rec) - 1.0;
    value = f + f * r8_csevl(xn, sincs, ntsn);

    if (n2 != 0) {
        value = -value;
    }

    if (value < -1.0) {
        value = -1.0;
    } else if (1.0 < value) {
        value = 1.0;
    }
    return value;
}

double RAYX_API r8_atan(const double x)
/******************************************************************************/
/*
  Purpose:

    R8_ATAN evaluates the arc-tangent of an R8 argument.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    Original FORTRAN77 version by Wayne Fullerton.
    C version by John Burkardt.

  Reference:

    Wayne Fullerton,
    Portable Special Function Routines,
    in Portability of Numerical Software,
    edited by Wayne Cowell,
    Lecture Notes in Computer Science, Volume 57,
    Springer 1977,
    ISBN: 978-3-540-08446-4,
    LC: QA297.W65.

  Parameters:

    Input, double X, the argument.

    Output, double R8_ATAN, the arc-tangent of X.
*/
{
    double atancs[16] =
        {+0.48690110349241406474636915902891E+00, -0.65108316367174641818869794945974E-02, +0.38345828265245177653569992430456E-04,
                   -0.26872212876223146539595410518788E-06, +0.20500930985824269846636514686688E-08, -0.16450717395484269455734135285348E-10,
                   +0.13650975274390773423813528484428E-12, -0.11601779591998246322891309834666E-14, +0.10038333943866273835797657402666E-16,
                   -0.88072747152163859327073696000000E-19, +0.78136321005661722180580266666666E-21, -0.69954535148267456086613333333333E-23,
                   +0.63105905713702136004266666666666E-25, -0.57296075370213874346666666666666E-27, +0.52274796280602282666666666666666E-29,
                   -0.48327903911608320000000000000000E-31};
    const double conpi8[4] = {0.375, 0.75, 1.125, 1.5};
    int n;
    int nterms          = 0;
    const double pi8[4] = {+0.17699081698724154807830422909937E-01, +0.35398163397448309615660845819875E-01,
                                    +0.53097245096172464423491268729813E-01, +0.70796326794896619231321691639751E-01};
    double sqeps        = 0.0;
    double t;
    const double tanp8[3] = {+0.41421356237309504880168872420969, +1.0, +2.4142135623730950488016887242096};
    double value;
    double xbig        = 0.0;
    const double xbnd1 = +0.19891236737965800691159762264467;
    const double xbnd2 = +0.66817863791929891999775768652308;
    const double xbnd3 = +1.4966057626654890176011351349424;
    const double xbnd4 = +5.0273394921258481045149750710640;
    double y;

    nterms = r8_inits(atancs, 16, 0.1 * r8_mach(3));
    sqeps  = sqrt(6.0 * r8_mach(3));
    xbig   = 1.0 / r8_mach(3);

    y = abs(x);

    if (y <= xbnd1) {
        value = x;
        if (sqeps < y) {
            value = x * (0.75 + r8_csevl(50.0 * y * y - 1.0, atancs, nterms));
        }
    } else if (y <= xbnd4) {
        if (xbnd3 < y) {
            n = 3;
        } else if (xbnd2 < y) {
            n = 2;
        } else {
            n = 1;
        }
        t     = (y - tanp8[n - 1]) / (1.0 + y * tanp8[n - 1]);
        value = conpi8[n - 1] + (pi8[n - 1] + t * (0.75 + r8_csevl(50.0 * t * t - 1.0, atancs, nterms)));
    } else {
        value = conpi8[3] + pi8[3];
        if (y < xbig) {
            value = conpi8[3] + (pi8[3] - (0.75 + r8_csevl(50.0 / y / y - 1.0, atancs, nterms)) / y);
        }
    }

    if (x < 0.0) {
        value = -abs(value);
    } else {
        value = +abs(value);
    }
    return value;
}

double RAYX_API r8_atan2(double sn, double cs)
/******************************************************************************/
/*
  Purpose:

    R8_ATAN2 evaluates the arc-tangent of two R8 arguments.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    Original FORTRAN77 version by Wayne Fullerton.
    C version by John Burkardt.

  Reference:

    Wayne Fullerton,
    Portable Special Function Routines,
    in Portability of Numerical Software,
    edited by Wayne Cowell,
    Lecture Notes in Computer Science, Volume 57,
    Springer 1977,
    ISBN: 978-3-540-08446-4,
    LC: QA297.W65.

  Parameters:

    Input, double SN, CS, the Y and X coordinates of a
    point on the angle.

    Output, double R8_ATAN2, the arc-tangent of the angle.
*/
{
    double abscs;
    double abssn;
    double big      = 0.0;
    double sml      = 0.0;
    double value;

    if (sml == 0.0) {
        sml = r8_mach(1);
        big = r8_mach(2);
    }
    /*
        We now make sure SN can be divided by CS.  It is painful.
    */
    abssn = abs(sn);
    abscs = abs(cs);

    if (abscs <= abssn) {
        if (abscs < 1.0 && abscs * big <= abssn) {
            if (sn < 0.0) {
                value = -0.5 * PI;
            } else if (sn == 0.0) {
                // fprintf ( stderr, "\n" );
                // fprintf ( stderr, "R8_ATAN2 - Fatal error!\n" );
                // fprintf ( stderr, "  Both arguments are 0.\n" );
                return 1;
            } else {
                value = 0.5 * PI;
            }
            return value;
        }
    } else {
        if (1.0 < abscs && abssn <= abscs * sml) {
            if (0.0 <= cs) {
                value = 0.0;
            } else {
                value = PI;
            }
            return value;
        }
    }
    value = r8_atan(sn / cs);

    if (cs < 0.0) {
        value = value + PI;
    }

    if (PI < value) {
        value = value - 2.0 * PI;
    }
    return value;
}

double RAYX_API r8_log(double x)
/******************************************************************************/
/*
  Purpose:

    R8_LOG evaluates the logarithm of an R8.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    Original FORTRAN77 version by Wayne Fullerton.
    C version by John Burkardt.

  Reference:

    Wayne Fullerton,
    Portable Special Function Routines,
    in Portability of Numerical Software,
    edited by Wayne Cowell,
    Lecture Notes in Computer Science, Volume 57,
    Springer 1977,
    ISBN: 978-3-540-08446-4,
    LC: QA297.W65.

  Parameters:

    Input, double X, the evaluation point.

    Output, double R8_LOG, the logarithm of X.
*/
{
    const double aln2 = 0.06814718055994530941723212145818;
    // double atancs[16] = double[16](
    double alncen[5] = {0.0, +0.22314355131420975576629509030983, +0.40546510810816438197801311546434, +0.55961578793542268627088850052682,
                                 +0.69314718055994530941723212145817};
    double alncs[16] =
        {+0.13347199877973881561689386047187E+01, +0.69375628328411286281372438354225E-03, +0.42934039020450834506559210803662E-06,
                   +0.28933847795432594580466440387587E-09, +0.20512517530340580901741813447726E-12, +0.15039717055497386574615153319999E-15,
                   +0.11294540695636464284521613333333E-18, +0.86355788671171868881946666666666E-22, +0.66952990534350370613333333333333E-25,
                   +0.52491557448151466666666666666666E-28, +0.41530540680362666666666666666666E-31, 0, 0, 0, 0, 0};  // fill up to length 16
    double center[4] = {1.0, 1.25, 1.50, 1.75};
    int n;
    int nterms = 0;
    int ntrval;
    double t = 0;
    double t2;
    double value;
    double xn;
    double y;

    nterms = r8_inits(alncs, 11, 28.9 * r8_mach(3));

    if (x <= 0.0) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_LOG - Fatal error!\n" );
        // fprintf ( stderr, "  X <= 0.\n" );
        return 1;
    }

    r8_upak(x, y, n);  // inout

    xn     = double(n - 1);
    y      = 2.0 * y;
    ntrval = int(4.0 * y - 2.5);

    if (ntrval == 5) {
        t = ((y - 1.0) - 1.0) / (y + 2.0);
    } else if (ntrval < 5) {
        t = (y - center[ntrval - 1]) / (y + center[ntrval - 1]);
    }

    t2    = t * t;
    value = 0.625 * xn + (aln2 * xn + alncen[ntrval - 1] + 2.0 * t + t * t2 * r8_csevl(578.0 * t2 - 1.0, alncs, nterms));

    return value;
}

double RAYX_API r8_exp(double x)

/******************************************************************************/
/*
  Purpose:

    R8_EXP evaluates the exponential of an R8 argument.

  Licensing:

    This code is distributed under the GNU LGPL license.

  Modified:

    17 January 2012

  Author:

    Original FORTRAN77 version by Wayne Fullerton.
    C version by John Burkardt.

  Reference:

    Wayne Fullerton,
    Portable Special Function Routines,
    in Portability of Numerical Software,
    edited by Wayne Cowell,
    Lecture Notes in Computer Science, Volume 57,
    Springer 1977,
    ISBN: 978-3-540-08446-4,
    LC: QA297.W65.

  Parameters:

    Input, double X, the argument.

    Output, double R8_EXP, the exponential of X.
*/
{
    double aln216 = +0.83120654223414517758794896030274E-01;
    double expcs[16] =
        {+0.866569493314985712733404647266231E-01, +0.938494869299839561896336579701203E-03, +0.677603970998168264074353014653601E-05,
                   +0.366931200393805927801891250687610E-07, +0.158959053617461844641928517821508E-09, +0.573859878630206601252990815262106E-12,
                   +0.177574448591421511802306980226000E-14, +0.480799166842372422675950244533333E-17, +0.115716376881828572809260000000000E-19,
                   +0.250650610255497719932458666666666E-22, +0.493571708140495828480000000000000E-25, +0.890929572740634240000000000000000E-28,
                   +0.148448062907997866666666666666666E-30, +0.229678916630186666666666666666666E-33, 0, 0};
    double f;
    int n;
    int n16;
    int ndx;
    int nterms = 0;
    double twon16[17] =
        {+0.0, +0.44273782427413840321966478739929E-01, +0.90507732665257659207010655760707E-01, +0.13878863475669165370383028384151,
                   +0.18920711500272106671749997056047, +0.24185781207348404859367746872659, +0.29683955465100966593375411779245,
                   +0.35425554693689272829801474014070, +0.41421356237309504880168872420969, +0.47682614593949931138690748037404,
                   +0.54221082540794082361229186209073, +0.61049033194925430817952066735740, +0.68179283050742908606225095246642,
                   +0.75625216037329948311216061937531, +0.83400808640934246348708318958828, +0.91520656139714729387261127029583, +1.0};
    double value;
    double xint;
    double xmax;
    double xmin;
    double y;

    double eta = 0.1 * r8_mach(3);
    int nos    = 14;
    nterms     = r8_inits(expcs, nos, eta);
    xmin       = r8_log(r8_mach(1)) + 0.01;
    xmax       = r8_log(r8_mach(2)) - 0.001;
    if (x < xmin) {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_EXP - Warning!\n" );
        // fprintf ( stderr, "  X so small that exp(X) underflows.\n" );
        value = 0.0;
    } else if (x <= xmax) {
        xint = int(x);
        y    = x - xint;

        y = 23.0 * y + x * aln216;
        n = int(floor(y));  // cast to int
        f = y - double(n);
        n = int(23.0 * xint + (double(n)));
        // return n;
        n16 = int(n / 16);
        if (n < 0) {
            n16 = n16 - 1;
        }
        ndx   = n - 16 * n16 + 1;
        value = 1.0 + (twon16[ndx - 1] + f * (1.0 + twon16[ndx - 1]) * r8_csevl(f, expcs, nterms));
        value = r8_pak(value, n16);
    } else {
        // fprintf ( stderr, "\n" );
        // fprintf ( stderr, "R8_EXP - Fatal error!\n" );
        // fprintf ( stderr, "  X so large that exp(X) overflows.\n" );
        return 1;
    }
    return value;
}

// DERIVED FUNCTIONS FOR SIN AND ASIN AND ACOS:

double RAYX_API r8_sin(const double x) {
    double pi_half = 1.570796326794896619231321691639751442;
    return r8_cos(pi_half - x);
}

double RAYX_API r8_asin(const double x) { return sign(x) * r8_atan(sqrt((x * x) / (1 - x * x))); }

double RAYX_API r8_acos(const double x) { return PI / 2 - r8_atan(x / (sqrt(1 - x * x))); }

double RAYX_API r8_sinh(const double x) { return (r8_exp(x) - r8_exp(-x)) / 2; }

double RAYX_API r8_cosh(const double x) { return (r8_exp(x) + r8_exp(-x)) / 2; }

// double pow simple
double RAYX_API dpow(double a, int b) {
    double res = 1;
    for (int i = 1; i <= b; i++) {
        res *= a;
    }
    return res;
}
