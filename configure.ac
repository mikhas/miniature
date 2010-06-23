AC_INIT([miniature], [0.1.8], [http://wiki.maemo.org/Miniature], [miniature])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_SRCDIR([src/main.cc])
AC_PREREQ([2.57])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.8.5 no-define subdir-objects])
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])
MINIATURE_CHECK_PLATFORM

AC_PROG_CXX
PKG_PROG_PKG_CONFIG

AC_LANG([C++])

PKG_CHECK_EXISTS([osso-af-settings >= 0.9.2])
PKG_CHECK_MODULES([MINIATURE], [TelepathyQt4 >= 0.2.0 hildon-1 >= 2.2.19])
AC_SUBST([MINIATURE_CFLAGS])
AC_SUBST([MINIATURE_LIBS])

# phonon and xmlpatterns are webkit dependencies,
# and we want the configure check to complain when they are missing.
# TODO: how to remove the opengl dependency when configured without opengl?
AT_WITH_QT([core dbus gui network svg opengl maemo5],[debug no_keywords])
AT_REQUIRE_QT_VERSION([4.6])


AC_ARG_ENABLE(opengl,
    [Enable OpenGL to be used as the Qt graphics system.],
    [AC_DEFINE(HAVE_MINIATURE_OPENGL, [1], [Define to enable Qt's OpenGL graphics system])]
)

AC_SUBST([desktopentrydir], [`"$PKG_CONFIG" --variable=desktopentrydir osso-af-settings`])
AC_SUBST([dbusservicedir], [`"$PKG_CONFIG" --variable=dbusservicedir osso-af-settings`])
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([LIBS])

if test -z "x$desktopentrydir"; then
  AC_MSG_ERROR([Cannot find desktop entry file folder])
fi

if test -z "$dbusservicedir"; then
  AC_MSG_ERROR([Cannot find D-Bus service file folder])
fi

AC_CONFIG_FILES([Makefile
                 data/miniature.desktop
                 data/org.maemo.miniature.service
                 data/org.freedesktop.Telepathy.Client.MiniatureApprover.service
                 data/org.freedesktop.Telepathy.Client.Miniature.service])

AC_OUTPUT
