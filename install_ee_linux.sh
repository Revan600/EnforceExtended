#!/bin/sh
apt install patchelf
cp DayZServer DayZServer.bak
patchelf --add-needed libEnforceExtended.so DayZServer
