BASENAME=hid-xpc-rc01
obj-m += $(BASENAME).o
obj-c += $(BASENAME).c

KDIR := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

${obj-m}: ${obj-c}
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f .$(BASENAME)* $(BASENAME).ko $(BASENAME).o $(BASENAME).mod.* modules.order Module.symvers *~
	rm -rf .tmp_versions



