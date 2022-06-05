dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(squeezeclient, 1.0.0)

AM_SILENT_RULES([yes])

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

LT_INIT
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CXX

# Derive path for storing systemd service files (e. g. /lib/systemd/system)
AC_ARG_WITH([systemdsystemunitdir],
        AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files]),
        [],
        [with_systemdsystemunitdir=$(pkg-config --silence-errors --variable=systemdsystemunitdir systemd)])

if test "x${with_systemdsystemunitdir}" = "x"; then
            with_systemdsystemunitdir="/lib/systemd/system"
fi

AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
AS_IF([test "x$with_systemdsystemunitdir" != "xno"],
      [AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])])

AM_CONDITIONAL([HAVE_SYSTEMD], [test "x$with_systemdsystemunitdir" != "xno"])

PKG_CHECK_MODULES([GIO],      [gio-2.0                    >= 2.30.0])
PKG_CHECK_MODULES([GIO_UNIX], [gio-unix-2.0               >= 2.30.0])
PKG_CHECK_MODULES([GLIB],     [glib-2.0                   >= 2.30.0])
PKG_CHECK_MODULES([GOBJECT],  [gobject-2.0                >= 2.30.0])
PKG_CHECK_MODULES([GSTREAMER],[gstreamer-1.0                >= 1.14.0])
PKG_CHECK_MODULES([LIBSOUP],  [libsoup-2.4                >= 2.64.0])
PKG_CHECK_MODULES([ALSA],  	  [alsa			              >= 1.1.0])

AC_CONFIG_FILES(Makefile src/Makefile src/include/Makefile src/libsqueezeclient/libsqueezeclient.pc)
AC_OUTPUT
