INVALID_MODULE=hid_zydacron
MODULE=hid-xpcrc01.ko
HERE=$(cd `dirname $0`; pwd)


rmmod hid-xpcrc01
rmmod $INVALID_MODULE
insmod $HERE/$MODULE



