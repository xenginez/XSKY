obj-m := route.o

KNAME:= $(shell uname -r)
KDIR:= /lib/modules/$(KNAME)/build

default:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f .*.cmd *.o *.ko *.mod *.mod.c modules.order Module.symvers
	rm -rf .tmp_versions
install:
	test -d "$(DESTDIR)/lib/modules/$(KNAME)/extra/net/xsky" || mkdir -p "$(DESTDIR)/lib/modules/$(KNAME)/extra/net/xsky"
	cp *.ko "$(DESTDIR)/lib/modules/$(KNAME)/extra/net/xsky"
