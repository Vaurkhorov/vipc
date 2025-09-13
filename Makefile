KDIR := /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(PWD)/build
MODULE := vipc

obj-m += $(MODULE).o
$(MODULE)-objs := main.o ProcMap.o

all:
	mkdir -p $(BUILD_DIR)
	make -C $(KDIR) M=$(PWD) modules
	mv $(MODULE).ko $(MODULE).ko.btf *.o .*.o .*.cmd $(MODULE).mod* modules.order Module.symvers $(BUILD_DIR) 2>/dev/null || true

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -rf $(BUILD_DIR)

