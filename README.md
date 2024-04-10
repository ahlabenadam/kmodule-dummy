# kmodule-dummy
A dummy Linux-Kernel Module named 'dummydevice'
## build the module 'dummydevice'
cd [your working directory]  
make
## install the module 'dummydevice'
sudo insmod dummydevice.ko
## uninstall  the module 'dummydevice'
sudo rmmod dummydevice

## check the node has been added successfully
lsmod | grep dummydevice

## read from the device
cat /dev/dummydevice

## write into the device
echo "test" >> /dev/dummydevice
