BASENAME=hid-xpc-rc01
obj-m += $(BASENAME).o

KDIR := /lib/modules/$(shell uname -r)/build
MODULEDIR := /lib/modules/$(shell uname -r)/kernel/drivers/hid


PWD := $(shell pwd)

${obj-m}:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install: ${obj-m}
	cp ${obj-m} $(MODULEDIR)

uninstall:
	rm -f $(MODULEDIR)/${obj-m}

clean:
	rm -f .$(BASENAME)* $(BASENAME).ko $(BASENAME).o $(BASENAME).mod.* modules.order Module.symvers *~
	rm -rf .tmp_versions



