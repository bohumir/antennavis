AC_PREREQ(2.60)
AC_INIT(antennavis, 0.3, pg4i@amsat.org)
AC_PREFIX_DEFAULT(/usr/local)
AC_CONFIG_SRCDIR([AntennaWidget.c])

# Checks for programs.
AC_PROG_CC

LDFLAGS="$LDFLAGS -L/usr/X11R6/lib"
# Checks for libraries.
AC_CHECK_LIB([GLU], [gluCylinder])
AC_CHECK_LIB([GL], [glXChooseVisual])
AC_CHECK_LIB([Xmu], [XmuLookupStandardColormap])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])

# Check for tcl/tk
AC_ARG_WITH(tcltk,
[  --with-tcltk=DIR        Use DIR/include and DIR/lib for tcl and tk])

if test -n "$tcltk" -a "$tcltk" != "no" ; then
	CPPFLAGS="-I$tcltk/include $CPPFLAGS"
	LDFLAGS"$LDFLAGS -L$tcltk/lib"
fi

AC_ARG_WITH(tkdir,
[  --with-tkdir=TKDIR      Use TKDIR/include and TKDIR/lib for tk if it
                          is in a different place from tcl directory
                          and is not auto-detected properly])

if test -n "$tkdir" -a "$tkdir" != "no" ; then
	CPPFLAGS="-I$tkdir/include $CPPFLAGS"
	LDFLAGS"$LDFLAGS -L$tkdir/lib"
fi

dnl A function to search for header files
AC_DEFUN([AC_SEARCH_HEADERS],
[
	for search_header in $1
	do
        [AC_CHECK_HEADER($search_header,
		    [$2=`AS_DIRNAME($search_header)`]
			
			[if test "$$2" != "/usr/include"; then
			   CPPFLAGS="-I$$2 $CPPFLAGS"
			fi
			break]
			,
			[])]
	done
])

if test "x$with_tcltk" != "xno" ; then
	AC_SEARCH_LIBS([Tcl_Init], [tcl86 tcl8.6 tcl85 tcl8.5 tcl84 tcl8.4 tcl83 tcl8.3 tcl8.0],
	AC_SEARCH_LIBS([Tk_Init], [tk86 tk8.6 tk85 tk8.5 tk84 tk8.4 tk83 tk8.3 tk8.0],
	    [AC_SEARCH_HEADERS(/usr/include/tcl8.4/tcl.h /usr/include/tcl8.3/tcl.h /usr/include/tcl8.0/tcl.h /usr/include/tcl.h
		    ,BASE_TCL_DIR)

		AC_SEARCH_HEADERS("$BASE_TCL_DIR/tk.h" /usr/include/tk8.4/tk.h /usr/include/tk8.3/tk.h /usr/include/tk8.0/tk.h /usr/include/tk.h
		    ,BASE_TK_DIR)]))
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([sqrt strdup strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
