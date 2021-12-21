dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(slimprotolib, 1.0)

AM_SILENT_RULES([yes])

LT_INIT
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CXX

PKG_CHECK_MODULES([GIO],      [gio-2.0                    >= 2.30.0])
PKG_CHECK_MODULES([GIO_UNIX], [gio-unix-2.0               >= 2.30.0])
PKG_CHECK_MODULES([GLIB],     [glib-2.0                   >= 2.30.0])
PKG_CHECK_MODULES([GOBJECT],  [gobject-2.0                >= 2.30.0])
PKG_CHECK_MODULES([GSTREAMER],[gstreamer-1.0                >= 1.18.0])
PKG_CHECK_MODULES([LIBSOUP],  [libsoup-2.4                >= 2.72.0])
PKG_CHECK_MODULES([ALSA],  	  [alsa			              >= 1.1.0])

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
