## Process this file with autoconf to produce configure.
## In general, the safest way to proceed is to run ./autogen.sh

# make sure we're interpreted by some minimal autoconf
AC_PREREQ([2.61])

AC_INIT([btds], [1.0.1], [libbtds@googlegroups.com])

AM_INIT_AUTOMAKE([-Wno-portability])
AC_CONFIG_MACRO_DIR([m4])
LT_INIT

# The argument here is just something that should be in the current directory
# (for sanity checking)
AC_CONFIG_SRCDIR([src/btree.c])
AM_CONFIG_HEADER(src/config.h)

AC_CANONICAL_HOST

AC_LANG([C])

dnl Fix default variables - "prefix" variable if not specified
if test "$prefix" = "NONE"; then
        prefix="/usr"

        dnl Fix "localstatedir" variable if not specified
        if test "$localstatedir" = "\${prefix}/var"; then
                localstatedir="/var"
        fi
        dnl Fix "sysconfdir" variable if not specified
        if test "$sysconfdir" = "\${prefix}/etc"; then
                sysconfdir="/etc"
        fi
        dnl Fix "libdir" variable if not specified
        if test "$libdir" = "\${exec_prefix}/lib"; then
                if test -e /usr/lib64; then
                        libdir="/usr/lib64"
                else
                        libdir="/usr/lib"
                fi
        fi
fi

if test "$srcdir" = "."; then
        AC_MSG_NOTICE([building in place srcdir:$srcdir])
        AC_DEFINE([BUILDING_IN_PLACE], 1, [building in place])
else
        AC_MSG_NOTICE([building out of tree srcdir:$srcdir])
fi

# Checks for programs.

# check stolen from gnulib/m4/gnu-make.m4
if ! ${MAKE-make} --version /cannot/make/this >/dev/null 2>&1; then
        AC_MSG_ERROR([you don't seem to have GNU make; it is required])
fi

AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_CONDITIONAL(GCC, test "$GCC" = yes)   # let the Makefile know if we're gcc

AC_PATH_PROGS(HELP2MAN, help2man)
AC_PATH_PROGS(PKGCONFIG, pkg-config)
AM_CONDITIONAL(HAVE_HELP2MAN, test x"${HELP2MAN}" != x"")

# Checks for libraries.
#AC_CHECK_LIB(eio)

# Checks for headers.
#AC_CHECK_HEADERS([eio.h])

# Check whether some low-level functions/files are available
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_C_VOLATILE

# Write generated configuration file
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

# ===============================================
# Helpers
# ===============================================

## helper for CC stuff
cc_supports_flag() {
        local CPPFLAGS="$CPPFLAGS $@"
        AC_MSG_CHECKING([whether $CC supports "$@"])
        AC_PREPROC_IFELSE([AC_LANG_PROGRAM([])],
                          [RC=0; AC_MSG_RESULT([yes])],
                          [RC=1; AC_MSG_RESULT([no])])
        return $RC
}

## cleanup
AC_MSG_NOTICE(Sanitizing prefix: ${prefix})
case $prefix in
  NONE) prefix=/usr/local;;
esac

AC_MSG_NOTICE(Sanitizing exec_prefix: ${exec_prefix})
case $exec_prefix in
  dnl For consistency with Corosync, map NONE->$prefix
  NONE)   exec_prefix=$prefix;;
  prefix) exec_prefix=$prefix;;
esac

## local defines
PACKAGE_FEATURES=""

LINT_FLAGS="-weak -unrecog +posixlib +ignoresigns -fcnuse \
        -badflag -D__gnuc_va_list=va_list -D__attribute\(x\)="

# local options

AC_ARG_ENABLE([fatal-warnings],
        [  --enable-fatal-warnings         : enable fatal warnings. ],
        [ default="no" ])

AC_ARG_ENABLE([debug],
        [  --enable-debug                  : enable debug build. ],
        [ default="no" ])

AC_ARG_ENABLE([user-flags],
        [  --enable-user-flags             : rely on user environment. ],
        [ default="no" ])

AC_ARG_ENABLE([coverage],
        [  --enable-coverage               : coverage analysis of the codebase. ],
        [ default="no" ])

AC_ARG_ENABLE([small-memory-footprint],
        [  --enable-small-memory-footprint : Use small message queues and small messages sizes. ],
        [ default="no" ])

AC_ARG_WITH([initddir],
        [  --with-initddir=DIR     : path to init script directory. ],
        [ INITDDIR="$withval" ],
        [ INITDDIR="$sysconfdir/init.d" ])

# OS detection
# THIS SECTION MUST DIE!
CP=cp
OS_LDL="-ldl"
have_linux="no"
case "$host_os" in
        *linux*)
                AC_DEFINE_UNQUOTED([BTDS_LINUX], [1],
                                   [Compiling for Linux platform])
                OS_CFLAGS=""
                OS_CPPFLAGS="-D_GNU_SOURCE"
                OS_LDFLAGS=""
                OS_DYFLAGS="-rdynamic"
                DARWIN_OPTS=""
                have_linux="yes"
        ;;
        darwin*)
                AC_DEFINE_UNQUOTED([BTDS_DARWIN], [1],
                                   [Compiling for Darwin platform])
                CP=rsync
                OS_CFLAGS=""
                OS_CPPFLAGS=""
                OS_LDFLAGS=""
                OS_DYFLAGS=""
                DARWIN_OPTS="-dynamiclib -bind_at_load \
                             -current_version ${SONAME} \
                             -compatibility_version ${SONAME} -install_name \$(libdir)/\$(@)"
                AC_DEFINE_UNQUOTED([MAP_ANONYMOUS], [MAP_ANON],
                                   [Shared memory define for Darwin platform])
                AC_DEFINE_UNQUOTED([PATH_MAX], [4096],
                                   [Number of chars in a path name including nul])
                AC_DEFINE_UNQUOTED([NAME_MAX], [255],
                                   [Number of chars in a file name])
        ;;
        *bsd*)
                AC_DEFINE_UNQUOTED([BTDS_BSD], [1],
                                   [Compiling for BSD platform])
                AC_DEFINE_UNQUOTED([MAP_ANONYMOUS], [MAP_ANON],
                                   [Shared memory define for Darwin platform])
                OS_CFLAGS=""
                OS_CPPFLAGS="-I/usr/local/include"
                OS_LDFLAGS="-L/usr/local/lib"
                OS_DYFLAGS="-export-dynamic"
                DARWIN_OPTS=""
                OS_LDL=""
                case "$host_os" in
                        *freebsd[[234567]]*)
                        ;;
                        *freebsd*)
                                AC_DEFINE_UNQUOTED([BTDS_FREEBSD_GE_8], [1],
                                           [Compiling for FreeBSD >= 8 platform])
                        ;;
                esac
        ;;
        *solaris*)
                AC_DEFINE_UNQUOTED([BTDS_SOLARIS], [1],
                                   [Compiling for Solaris platform])
                AC_DEFINE_UNQUOTED([TS_CLASS], [1],
                                   [Prevent being scheduled RR])
                AC_DEFINE_UNQUOTED([_SEM_SEMUN_UNDEFINED], [1],
                                   [The semun structure is undefined])
                CP=rsync
                OS_CFLAGS=""
                OS_CPPFLAGS="-D_REENTRANT"
                OS_LDFLAGS=""
                OS_DYFLAGS="-Wl,-z,lazyload"
                DARWIN_OPTS=""
                SOLARIS_OPTS=" "
        ;;
        *)
                AC_MSG_ERROR([Unsupported OS? hmmmm])
        ;;
esac

AC_SUBST(CP)
# *FLAGS handling goes here

ENV_CFLAGS="$CFLAGS"
ENV_CPPFLAGS="$CPPFLAGS"
ENV_LDFLAGS="$LDFLAGS"

# debug build stuff
if test "x${enable_debug}" = xyes; then
        AC_DEFINE_UNQUOTED([DEBUG], [1], [Compiling Debugging code])
        OPT_CFLAGS="-O0"
        PACKAGE_FEATURES="$PACKAGE_FEATURES debug"
else
        OPT_CFLAGS="-O3"
fi

# gdb flags
if test "x${GCC}" = xyes; then
        GDB_FLAGS="-ggdb3"
else
        GDB_FLAGS="-g"
fi

# extra warnings
EXTRA_WARNINGS=""

WARNLIST="
        all
        shadow
        missing-prototypes
        missing-declarations
        strict-prototypes
        declaration-after-statement
        pointer-arith
        write-strings
        bad-function-cast
        missing-format-attribute
        format=2
        format-security
        format-nonliteral
        no-long-long
        unsigned-char
        gnu89-inline
        no-strict-aliasing
        "

for j in $WARNLIST; do
        if cc_supports_flag -W$j; then
                EXTRA_WARNINGS="$EXTRA_WARNINGS -W$j";
        fi
done

if test "x${enable_coverage}" = xyes && \
                cc_supports_flag -ftest-coverage && \
                cc_supports_flag -fprofile-arcs ; then
        AC_MSG_NOTICE([Enabling Coverage (enable -O0 by default)])
        OPT_CFLAGS="-O0"
        COVERAGE_CFLAGS="-ftest-coverage -fprofile-arcs"
        COVERAGE_LDFLAGS="-ftest-coverage -fprofile-arcs"
        COVERAGE_LCRSO_EXTRA_LDFLAGS="-rdynamic"
        PACKAGE_FEATURES="$PACKAGE_FEATURES coverage"
else
        COVERAGE_CFLAGS=""
        COVERAGE_LDFLAGS=""
        COVERAGE_LCRSO_EXTRA_LDFLAGS=""
fi


if test "x${enable_small_memory_footprint}" = xyes ; then
        AC_DEFINE_UNQUOTED([HAVE_SMALL_MEMORY_FOOTPRINT], 1, [have small_memory_footprint])
        PACKAGE_FEATURES="$PACKAGE_FEATURES small-memory-footprint"
fi

if test "x${enable_ansi}" = xyes && \
                cc_supports_flag -std=iso9899:199409 ; then
        AC_MSG_NOTICE([Enabling ANSI Compatibility])
        ANSI_CPPFLAGS="-ansi -D_GNU_SOURCE -DANSI_ONLY"
        PACKAGE_FEATURES="$PACKAGE_FEATURES ansi"
else
        ANSI_CPPFLAGS=""
fi

if test "x${enable_fatal_warnings}" = xyes && \
                cc_supports_flag -Werror ; then
        AC_MSG_NOTICE([Enabling Fatal Warnings (-Werror)])
        WERROR_CFLAGS="-Werror"
        PACKAGE_FEATURES="$PACKAGE_FEATURES fatal-warnings"
else
        WERROR_CFLAGS=""
fi

# don't add addtional cflags
if test "x${enable_user_flags}" = xyes; then
  OPT_CFLAGS=""
  GDB_FLAGS=""
  EXTRA_WARNINGS=""
fi

# final build of *FLAGS
CFLAGS="$ENV_CFLAGS $OPT_CFLAGS $GDB_FLAGS $OS_CFLAGS \
        $COVERAGE_CFLAGS $EXTRA_WARNINGS $WERROR_CFLAGS $NSS_CFLAGS"
CPPFLAGS="$ENV_CPPFLAGS $ANSI_CPPFLAGS $OS_CPPFLAGS"
LDFLAGS="$ENV_LDFLAGS $COVERAGE_LDFLAGS $OS_LDFLAGS"

# substitute what we need:
AC_SUBST([INITDDIR])

AC_SUBST([COVERAGE_LCRSO_EXTRA_LDFLAGS])
AC_SUBST([OS_DYFLAGS])

AC_SUBST([OS_LDL])

AM_CONDITIONAL(BUILD_DARWIN, test -n "${DARWIN_OPTS}")
AM_CONDITIONAL(BUILD_SOLARIS, test -n "${SOLARIS_OPTS}")
AC_SUBST([DARWIN_OPTS])
AC_SUBST([SOLARIS_OPTS])

AM_CONDITIONAL(BUILD_HTML_DOCS, test -n "${GROFF}")

AC_SUBST([LINT_FLAGS])

AC_DEFINE_UNQUOTED([LCRSODIR], "$(eval echo ${LCRSODIR})", [LCRSO directory])
AC_DEFINE_UNQUOTED([SOCKETDIR], "$(eval echo ${SOCKETDIR})", [Socket directory])
AC_DEFINE_UNQUOTED([LOCALSTATEDIR], "$(eval echo ${localstatedir})", [localstate directory])

BTDSSYSCONFDIR=${sysconfdir}/btds
AC_SUBST([BTDSSYSCONFDIR])
AC_DEFINE_UNQUOTED([BTDSSYSCONFDIR], "$(eval echo ${BTDSSYSCONFDIR})", [btds config directory])

AC_DEFINE_UNQUOTED([PACKAGE_FEATURES], "${PACKAGE_FEATURES}", [btds built-in features])

AC_OUTPUT

AC_MSG_RESULT([])
AC_MSG_RESULT([$PACKAGE configuration:])
AC_MSG_RESULT([  Version                  = ${VERSION}])
AC_MSG_RESULT([  Prefix                   = ${prefix}])
AC_MSG_RESULT([  Executables              = ${sbindir}])
AC_MSG_RESULT([  Man pages                = ${mandir}])
AC_MSG_RESULT([  Doc dir                  = ${docdir}])
AC_MSG_RESULT([  Libraries                = ${libdir}])
AC_MSG_RESULT([  Header files             = ${includedir}])
AC_MSG_RESULT([  Arch-independent files   = ${datadir}])
AC_MSG_RESULT([  State information        = ${localstatedir}])
AC_MSG_RESULT([  System configuration     = ${sysconfdir}])
AC_MSG_RESULT([  System init.d directory  = ${INITDDIR}])
AC_MSG_RESULT([  btds config dir          = ${BTDSSYSCONFDIR}])
AC_MSG_RESULT([  SOCKETDIR                = ${SOCKETDIR}])
AC_MSG_RESULT([  Features                 =${PACKAGE_FEATURES}])
AC_MSG_RESULT([])
AC_MSG_RESULT([$PACKAGE build info:])
AC_MSG_RESULT([  Library SONAME           = ${SONAME}])
LIB_MSG_RESULT(m4_shift(local_soname_list))dnl
AC_MSG_RESULT([  Default optimization     = ${OPT_CFLAGS}])
AC_MSG_RESULT([  Default debug options    = ${GDB_CFLAGS}])
AC_MSG_RESULT([  Extra compiler warnings  = ${EXTRA_WARNING}])
AC_MSG_RESULT([  Env. defined CFLAG       = ${ENV_CFLAGS}])
AC_MSG_RESULT([  Env. defined CPPFLAGS    = ${ENV_CPPFLAGS}])
AC_MSG_RESULT([  Env. defined LDFLAGS     = ${ENV_LDFLAGS}])
AC_MSG_RESULT([  OS   defined CFLAGS      = ${OS_CFLAGS}])
AC_MSG_RESULT([  OS   defined CPPFLAGS    = ${OS_CPPFLAGS}])
AC_MSG_RESULT([  OS   defined LDFLAGS     = ${OS_LDFLAGS}])
AC_MSG_RESULT([  OS   defined LDL         = ${OS_LDL}])
AC_MSG_RESULT([  OS   defined DYFLAGS     = ${OS_DYFLAGS}])
AC_MSG_RESULT([  ANSI defined CPPFLAGS    = ${ANSI_CPPFLAGS}])
AC_MSG_RESULT([  Coverage     CFLAGS      = ${COVERAGE_CFLAGS}])
AC_MSG_RESULT([  Coverage     LDFLAGS     = ${COVERAGE_LDFLAGS}])
AC_MSG_RESULT([  Fatal War.   CFLAGS      = ${WERROR_CFLAGS}])
AC_MSG_RESULT([  Final        CFLAGS      = ${CFLAGS}])
AC_MSG_RESULT([  Final        CPPFLAGS    = ${CPPFLAGS}])
AC_MSG_RESULT([  Final        LDFLAGS     = ${LDFLAGS}])
