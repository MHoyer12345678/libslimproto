ACLOCAL_AMFLAGS=-I m4

bin_PROGRAMS=test-slimproto test-gstplayer

noinst_LIBRARIES = libsimproto.a

libsimproto_a_SOURCES = \
	slimproto/Utils.cpp					\
	slimproto/Utils.h					\
	slimproto/PlayerController.cpp		\
	slimproto/PlayerController.h		\
	slimproto/LMSConnection.cpp			\
	slimproto/LMSConnection.h			\
	slimproto/CommandFactory.cpp		\
	slimproto/CommandFactory.h			\
	player/IPlayer.cpp					\
	player/GstPlayerConfig.cpp			\
	player/GstPlayer.cpp				\
	player/GstPlayer.h					\
	squeezeclient/SqueezeClientImpl.cpp	\
	squeezeclient/SqueezeClientImpl.h	\
	squeezeclient/SqueezeClient.cpp		\
	include/SqueezeClient.h				\
	include/IPlayer.h					\
	include/GstPlayerConfig.h				
	

libsimproto_a_CPPFLAGS  = \
		-fPIC					\
		-Ilib					\
		-Iinclude				\
		-Iplayer				\
		-Isqueezeclient			\
	   	$(GIO_LIBS_CFLAGS)		\
	   	$(GIO_UNIX_CFLAGS)		\
	   	$(GLIB_CFLAG)			\
	   	$(GOBJECT_CFLAGS)		\
	   	$(LIBSOUP_CFLAGS)		\
		$(GSTREAMER_CFLAGS)
		

test_slimproto_SOURCES =		\
	test/mainSlimProtoTest.cpp	\
	test/SlimProtoTest.cpp		\
	test/SlimProtoTest.h
		
test_slimproto_CPPFLAGS = \
		-Iinclude				\
		$(GIO_LIBS_CFLAGS)		\
		$(GIO_UNIX_CFLAGS)		\
	   	$(GLIB_CFLAG)			\
		$(GSTREAMER_CFLAGS)		\
	   	$(GOBJECT_CFLAGS)		\
	   	$(LIBSOUP_CFLAGS)

test_slimproto_LDADD	  = \
		libsimproto.a			\
		-lCppAppUtils			\
		$(GIO_LIBS_LIBS)		\
		$(GIO_UNIX_LIBS)		\
		$(GLIB_LIBS)			\
		$(GSTREAMER_LIBS)		\
		$(GOBJECT_LIBS)			\
		$(LIBSOUP_LIBS)


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
		$(GOBJECT_CFLAGS)		\
	   	$(LIBSOUP_CFLAGS)

test_gstplayer_LDADD	  = \
		libsimproto.a			\
		-lCppAppUtils			\
		$(GIO_LIBS_LIBS)		\
		$(GIO_UNIX_LIBS)		\
		$(GLIB_LIBS)			\
		$(GOBJECT_LIBS)			\
		$(GSTREAMER_LIBS)		\
		$(LIBSOUP_LIBS)
		