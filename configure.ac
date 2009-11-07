AC_INIT([miniature], [0.1], [chess-developer@garage.maemo.org], [miniature])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_SRCDIR([src/miniature.cc])
AC_PREREQ([2.57])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.8.5 no-define subdir-objects])
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

AC_PROG_CXX
AC_LANG([C++])
AT_WITH_QT([core gui svg],[no_keywords])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
