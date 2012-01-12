#!/bin/bash
#
# Bibble 5 Pro startup script
# make symlinks to this script anywhere in your $PATH
#

BIBBLE_INSTALL_PATH="/opt/AfterShotPro"

LD_LIBRARY_PATH=${BIBBLE_INSTALL_PATH}/lib:${LD_LIBRARY_PATH}
XLIB_SKIP_ARGB_VISUALS=1
export LD_LIBRARY_PATH
export XLIB_SKIP_ARGB_VISUALS
echo "Install Path:          " ${BIBBLE_INSTALL_PATH}
echo "LD_PATH:               " $LD_LIBRARY_PATH
echo "XLIB_SKIP_ARGB_VISUALS:" $XLIB_SKIP_ARGB_VISUALS


if [ -x "${BIBBLE_INSTALL_PATH}/bin/AfterShotPro" ]
then
    cd "${BIBBLE_INSTALL_PATH}/bin"
#    ldd  "./bibble5"
    gdb "./AfterShotPro"
    #echo "all is well in the world"
else
    echo "didn't run it (obviously)"
fi
exit 1
