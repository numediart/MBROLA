IMPORTANT: most of machine dependant moves are kept in Misc/common.h

Please check the following #defines, some of which can be set in the Makefile

depending on your OS you must define:

    TARGET_OS_MAC
    TARGET_OS_DOS
    TARGET_OS_SUN4
    TARGET_OS_VMS
    TARGET_OS_BEOS

If your system is not listed above, then it may be handled directly with:

    __i386
    __powerpc__
    __sparc

If not, then you have to manually define one of those:

    LITTLE_ENDIAN
or

    BIG_ENDIAN

If you are able to handle signals, then #define SIGNAL (able to reset the
mbrola engine on the fly with SIG_USR1).

If you want to build the library mode instead of Standalone then `#define LIBRARY`

If you want to build a Windows DLL, then `#define DLL`
