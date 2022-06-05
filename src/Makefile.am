ACLOCAL_AMFLAGS=-I m4

SUBDIRS=include

bin_PROGRAMS=squeezeclient

noinst_PROGRAMS=test-libsqueezeclient test-gstplayer

lib_LTLIBRARIES=libsqueezeclient.la

libsqueezeclient_la_SOURCES = \
	libsqueezeclient/Utils.cpp					\
	libsqueezeclient/Utils.h					\
	libsqueezeclient/ClientController.cpp		\
	libsqueezeclient/ClientController.h			\
	libsqueezeclient/LMSConnection.cpp			\
	libsqueezeclient/LMSConnection.h			\
	libsqueezeclient/CommandFactory.cpp			\
	libsqueezeclient/CommandFactory.h			\
	libsqueezeclient/SqueezeClientImpl.cpp		\
	libsqueezeclient/SqueezeClientImpl.h		\
	libsqueezeclient/SqueezeClientBuilder.cpp	\
	include/SqueezeClient.h						\
	player/IPlayer.cpp							\
	player/GstPlayerConfig.cpp					\
	player/GstPlayer.cpp						\
	player/GstPlayer.h							\
	volcontrol/AlsaVolumeControl.cpp			\
	volcontrol/AlsaVolumeControl.h				\
	include/SqueezeClient.h						\
	include/SqueezeClientBuilder.h				\
	include/IPlayer.h							\
	include/IVolumeControl.h					\
	include/GstPlayerConfig.h				
	
libsqueezeclient_la_CPPFLAGS  = \
		-fPIC					\
		-Ilib					\
		-Iinclude				\
		-Iplayer				\
		-Ivolcontrol			\
	   	$(GIO_LIBS_CFLAGS)		\
	   	$(GIO_UNIX_CFLAGS)		\
	   	$(GLIB_CFLAG)			\
	   	$(GOBJECT_CFLAGS)		\
	   	$(LIBSOUP_CFLAGS)		\
		$(GSTREAMER_CFLAGS)		\
		$(ALSA_CFLAGS)

libsqueezeclient_la_LIBADD	  =	\
		-lCppAppUtils			\
		$(GIO_LIBS_LIBS)		\
		$(GIO_UNIX_LIBS)		\
		$(GLIB_LIBS)			\
		$(GSTREAMER_LIBS)		\
		$(GOBJECT_LIBS)			\
		$(LIBSOUP_LIBS)			\
		$(ALSA_LIBS)

libsqueezeclient_la_LDFLAGS = -version-info 1:0:0 -shared -fPIC
		
pkgconfig_DATA = libsqueezeclient/libsqueezeclient.pc
		
squeezeclient_SOURCES = \
	squeezeclient-bin/mainSqueezeClientBin.cpp							\
	squeezeclient-bin/SqueezeClientBin.cpp								\
	squeezeclient-bin/SqueezeClientBin.h								\
	squeezeclient-bin/SCBConfig.cpp										\
	squeezeclient-bin/SCBConfig.h										\
	squeezeclient-bin/SCBController.cpp									\
	squeezeclient-bin/SCBController.h									\
	squeezeclient-bin/generated/SqueezeClientInteface.c					\
	squeezeclient-bin/generated/SqueezeClientInteface.h

squeezeclient_CPPFLAGS = \
		-Iinclude				\
		$(GIO_LIBS_CFLAGS)		\
		$(GIO_UNIX_CFLAGS)		\
	   	$(GLIB_CFLAG)			\
		$(GSTREAMER_CFLAGS)		\
	   	$(GOBJECT_CFLAGS)		

squeezeclient_LDADD	  = \
		libsqueezeclient.la	\
		-lCppAppUtils			\
		$(GIO_LIBS_LIBS)		\
		$(GIO_UNIX_LIBS)		\
		$(GLIB_LIBS)			\
		$(GSTREAMER_LIBS)		\
		$(GOBJECT_LIBS)

# List generated headers here to assert that they are available when built is started
BUILT_SOURCES =  squeezeclient-bin/generated/SqueezeClientInteface.h

# Delete all generated files on 'make clean'
CLEANFILES    =  squeezeclient-bin/generated/SqueezeClientInteface.c  \
                 squeezeclient-bin/generated/SqueezeClientInteface.h

# Targets to create generated sources during the build
squeezeclient-bin/generated/SqueezeClientInteface.c squeezeclient-bin/generated/SqueezeClientInteface.h: squeezeclient-bin/dbus/squeezeclient-bin.xml
	gdbus-codegen --interface-prefix=SqueezeClientBinDBus. --generate-c-code=squeezeclient-bin/generated/SqueezeClientInteface squeezeclient-bin/dbus/squeezeclient-bin.xml

squeezeclientconfdir=$(sysconfdir)/squeezeclient
squeezeclientconf_DATA = squeezeclient-bin/conf/squeezeclient.conf

DISTCHECK_CONFIGURE_FLAGS = \
  --with-systemdsystemunitdir=$$dc_install_base/$(systemdsystemunitdir)
  
if HAVE_SYSTEMD
systemdsystemunit_DATA = \
  		squeezeclient-bin/conf/squeezeclient.service
endif

test_libsqueezeclient_SOURCES =		\
	test/mainSlimProtoTest.cpp	\
	test/SlimProtoTest.cpp		\
	test/SlimProtoTest.h

test_libsqueezeclient_CPPFLAGS = \
		-Iinclude				\
		$(GIO_LIBS_CFLAGS)		\
		$(GIO_UNIX_CFLAGS)		\
	   	$(GLIB_CFLAG)			\
		$(GSTREAMER_CFLAGS)		\
	   	$(GOBJECT_CFLAGS)
	   	

test_libsqueezeclient_LDADD	  = \
		libsqueezeclient.la		\
		-lCppAppUtils			\
		$(GIO_LIBS_LIBS)		\
		$(GIO_UNIX_LIBS)		\
		$(GLIB_LIBS)			\
		$(GSTREAMER_LIBS)		\
		$(GOBJECT_LIBS)
		
test_gstplayer_SOURCES =		\
	test/mainGstPlayerTest.cpp	\
	test/GstPlayerTest.cpp		\
	test/GstPlayerTest.h
	
test_gstplayer_CPPFLAGS = \
		-Iinclude				\
		-Iplayer				\
		$(GIO_LIBS_CFLAGS)		\
		$(GIO_UNIX_CFLAGS)		\
		$(GLIB_CFLAG)			\
		$(GSTREAMER_CFLAGS)		\
		$(GOBJECT_CFLAGS)

test_gstplayer_LDADD	  = \
		libsqueezeclient.la		\
		-lCppAppUtils			\
		$(GIO_LIBS_LIBS)		\
		$(GIO_UNIX_LIBS)		\
		$(GLIB_LIBS)			\
		$(GOBJECT_LIBS)			\
		$(GSTREAMER_LIBS)
