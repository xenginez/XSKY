obj-m := server.o

KNAME:= $(shell uname -r)
KDIR:= /lib/modules/$(KNAME)/build

default:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f .*.cmd *.o *.ko *.mod *.mod.c modules.order Module.symvers
	rm -rf .tmp_versions
install:
	test -d "$(DESTDIR)/lib/modules/$(KNAME)/extra/net/server" || mkdir -p "$(DESTDIR)/lib/modules/$(KNAME)/extra/net/server"
	cp *.ko "$(DESTDIR)/lib/modules/$(KNAME)/extra/net/server"
