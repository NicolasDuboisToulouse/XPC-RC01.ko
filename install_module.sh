INVALID_MODULE=hid_zydacron
MODULE=hid-xpc-rc01.ko
HERE=$(cd `dirname $0`; pwd)


rmmod $MODULE
rmmod $INVALID_MODULE
insmod $HERE/$MODULE



