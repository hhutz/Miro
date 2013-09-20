## -*- Makefile -*- ##########################################################
##
## This file is part of Miro (The Middleware For Robots)
## 
## $Id$
##
##############################################################################

AC_DEFUN([AC_SEARCH_MIRO],
[
	AC_ARG_WITH(Miro,
		AC_HELP_STRING([--with-Miro=DIR], [root directory of Miro installation]),
		ac_with_MIRO=$withval
		if test "x${ac_with_MIRO}" != xyes; then
			MIRO_DIR="$withval"
		fi,
		ac_with_MIRO=no
	)

	if test "x${ac_with_MIRO}" != xno ; then
		MIRO_CONFIG=$MIRO_DIR/bin/miro-config
	else
		if test "${MIRO_ROOT}" != "" ; then
			MIRO_CONFIG=$MIRO_ROOT/bin/miro-config
		else
			MIRO_CONFIG=miro-config
		fi
	fi

	AC_MSG_CHECKING(for ACE/TAO/Qt/Miro)
	if ! test -x $MIRO_CONFIG; then
		AC_MSG_ERROR([Miro not (properly) installed. Source tarball and CVS at: http://smart.informatik.uni-ulm.de/MIRO/. Please check your installation! For more details about this problem, look at the end of config.log.])
	else
		AC_MSG_RESULT(yes)
	fi

# miro related stuff

	MIRO_CFLAGS=`$MIRO_CONFIG --cflags`
	MIRO_LDFLAGS=`$MIRO_CONFIG --ldflags`
	MIRO_LIBS=`$MIRO_CONFIG --libs`
	MIRO_MAKEPARAMS=`$MIRO_CONFIG --makeparams`
	AC_SUBST(MIRO_CFLAGS)
	AC_SUBST(MIRO_LDFLAGS)
	AC_SUBST(MIRO_LIBS)
	AC_SUBST(MIRO_MAKEPARAMS)

# ace/tao part

	ACETAO_CFLAGS=`$MIRO_CONFIG --acetao-cflags`
	ACETAO_LDFLAGS=`$MIRO_CONFIG --acetao-ldflags`
	ACETAO_LIBS=`$MIRO_CONFIG --acetao-libs`
	AC_SUBST(ACETAO_CFLAGS)
	AC_SUBST(ACETAO_LDFLAGS)
	AC_SUBST(ACETAO_LIBS)
	TAO_IDL=`$MIRO_CONFIG --acetao-idlcomp`
	AC_SUBST(TAO_IDL)

# qt part

	QT_CFLAGS=`$MIRO_CONFIG --qt-cflags`
	QT_LDFLAGS=`$MIRO_CONFIG --qt-ldflags`
	QT_LIBS=`$MIRO_CONFIG --qt-libs`
	AC_SUBST(QT_CFLAGS)
	AC_SUBST(QT_LDFLAGS)
	AC_SUBST(QT_LIBS)
	QT_MOC=`$MIRO_CONFIG --qt-moc`
	QT_UIC=`$MIRO_CONFIG --qt-uic`
	AC_SUBST(QT_MOC)
	AC_SUBST(QT_UIC)

# check for conditional features within miro (others may be added here)

	MIRO_SPARROW99=`$MIRO_CONFIG --sparrow99`
	MIRO_QUICKCAM=`$MIRO_CONFIG --quickcam`
	MIRO_IEEE1394=`$MIRO_CONFIG --ieee1394`

	AM_CONDITIONAL(COND_SPARROW99, [test "x$MIRO_SPARROW99" = xyes])
	AM_CONDITIONAL(COND_QUICKCAM, [test "x$MIRO_QUICKCAM" = xyes])
	AM_CONDITIONAL(COND_IEEE1394, [test "x$MIRO_IEEE1394" = xyes])

	AH_TEMPLATE([MIRO_HAS_SPARROW99], [build with Sparrow99 support.])
	AH_TEMPLATE([MIRO_HAS_QUICKCAM], [build with QuickCam support.])
	AH_TEMPLATE([MIRO_HAS_1394], [build with IEEE1394 support.])

	if test "x$MIRO_SPARROW99" = xyes; then
		AC_DEFINE(MIRO_HAS_SPARROW99)
	fi
	if test "x$MIRO_QUICKCAM" = xyes; then
		AC_DEFINE(MIRO_HAS_QUICKCAM)
	fi
	if test "x$MIRO_IEEE1394" = xyes; then
		AC_DEFINE(MIRO_HAS_1394)
	fi
])
