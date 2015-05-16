# How to fix VirtualBox black screen after login

In some rare cases a user might be presented with a black screen after logging into the virtual machine.

The following instructions re-install the VirtualBox guest additions from a root shell. 

With the virtual machine turned off:
* Under Settings -> Storage: Add a CD drive, if(f) you don't have one
* Leave it empty (don't mount anything)

Then boot the VM:
* Hold shift to enter boot menu on startup
* Choose: Advanced options for Ubuntu
* Choose an option with "(recovery mode)"
* In the recovery menu choose: Drop to root shell
* In the VirtualBox menus: Devices -> Insert Guest Additions

Now enter the following in the (root) shell:

```sh
mount -o remount,rw /
mount /dev/cdrom /mnt
cd /mnt
./VBoxLinuxAdditions.run
reboot
```

After the reboot, the machine should work as expected.
