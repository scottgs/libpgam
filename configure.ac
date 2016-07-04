dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT([libpgam], [1.0.0], [GrantScott@missouri.edu])
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_PROG_LIBTOOL

## Ensure regular C
AC_LANG([C])

# Use pg_config --includedir-server to find out where the PostgreSQL server header files are installed on your system (or the system that your users will be running on).

PG_SERVER_INCDIR=`pg_config --includedir-server`
AC_SUBST([PG_SERVER_INCDIR])

CPPFLAGS="-I$PG_SERVER_INCDIR"
AM_CPPFLAGS="-Wall -Wextra -g -rdynamic"
AC_SUBST([AM_CPPFLAGS])

AC_MSG_NOTICE("************************************")
AC_MSG_NOTICE("PostgreSQL Server Include: $CPPFLAGS")
AC_MSG_NOTICE("************************************")

AC_CHECK_HEADER([postgres.h], [], AC_MSG_ERROR([Header postgres.h not found]))

AC_LANG([C++])

AC_C_CONST



AC_CONFIG_FILES([Makefile
				doc/Makefile
				src/Makefile
				sql/Makefile
				tests/Makefile
				])
AC_OUTPUT
