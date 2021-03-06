#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([wake], [1.0.0], [jason@sigio.com])
AC_CONFIG_SRCDIR([src/broadcast.h])
AM_INIT_AUTOMAKE([foreign])

# Check for options
AC_ARG_ENABLE([debug],
[  --enable-debug    Turn on debugging],
[case "${enableval}" in
  yes) debug=true ;;
  no)  debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h ctype.h errno.h net/if.h netinet/in.h pwd.h regex.h stdarg.h stdio.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/stat.h sys/types.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UID_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset regcomp socket strcasecmp strchr strerror strtol])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
