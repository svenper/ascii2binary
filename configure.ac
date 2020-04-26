AC_PREREQ(2.59)
AC_INIT(ascii2binary, 2.14, billposer@alum.mit.edu)
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_DEFUN([AC_C_LONG_LONG],
[AC_CACHE_CHECK(for long long int, ac_cv_c_long_long,
[if test "$GCC" = yes; then
  ac_cv_c_long_long=yes
  else
        AC_TRY_COMPILE(,[long long int i;],
   ac_cv_c_long_long=yes,
   ac_cv_c_long_long=no)
   fi])
   if test $ac_cv_c_long_long = yes; then
     AC_DEFINE(HAVE_LONG_LONG, 1, [compiler understands long long])
   fi
])

AC_DEFUN([AC_C_PRINTF_THSEP],
[AC_TRY_COMPILE(,[printf("%'2d",101);],ac_cv_c_printf_thsep=yes,ac_cv_c_printf_thsep=no)
 if test $ac_cv_c_printf_thsep = yes; then
  AC_DEFINE(HAVE_PRINTF_THSEP, 1, [compiler understands printf flag for thousands separation in ints])
 fi
])

AC_DEFUN([AC_C_PRINTF_THSEP_FLOAT],
[AC_TRY_COMPILE(,[printf("%'2f",101.02);],ac_cv_c_printf_thsep_float=yes,
ac_cv_c_printf_thsep_float=no)
 if test $ac_cv_c_printf_thsep_float = yes; then
   AC_DEFINE(HAVE_PRINTF_THSEP_FLOAT, 1, [compiler understands printf flag for thousands separation in floats])
 fi
])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([ctype.h fcntl.h langinfo.h libintl.h limits.h locale.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_LONG_LONG
AC_C_LONG_DOUBLE
AC_C_PRINTF_THSEP
AC_C_PRINTF_THSEP_FLOAT

# Checks for library functions.
AC_CHECK_FUNCS([getopt_long])
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([nl_langinfo setlocale strtoul strtoull])
AC_DEFINE_UNQUOTED(LOCALEDIR, "${prefix}/${DATADIRNAME}/locale", [Name of gettext locale directory])
AC_OUTPUT(Makefile)








