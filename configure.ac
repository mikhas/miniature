AC_INIT([miniature], [0.1.1], [http://wiki.maemo.org/Miniature], [miniature])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_SRCDIR([src/miniature.cc])
AC_PREREQ([2.57])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.8.5 no-define subdir-objects])
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

AC_PROG_CXX
PKG_PROG_PKG_CONFIG

AC_LANG([C++])

# phonon and xmlpatterns are webkit dependencies,
# and we want the configure check to complain when they are missing.
AT_WITH_QT([core gui network svg webkit phonon xmlpatterns],[no_keywords])
AT_REQUIRE_QT_VERSION([4.6])

PKG_CHECK_EXISTS([osso-af-settings >= 0.9.2])

AC_SUBST([desktopentrydir], [`"$PKG_CONFIG" --variable=desktopentrydir osso-af-settings`])
AC_SUBST([dbusservicedir], [`"$PKG_CONFIG" --variable=dbusservicedir osso-af-settings`])

if test -z "x$desktopentrydir"; then
  AC_MSG_ERROR([Cannot find desktop entry file folder])
fi

if test -z "$dbusservicedir"; then
  AC_MSG_ERROR([Cannot find D-Bus service file folder])
fi

AC_CONFIG_FILES([Makefile
                 data/miniature.desktop
                 data/org.maemo.miniature.service])

AC_OUTPUT
