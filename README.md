# OUX/C+ OS

# Information and instructions to the user

## 1. Contact

Manufacturer: overcq

Contact the manufacturer: overcq@int.pl

Source code: https://github.com/overcq/kernel

Description: https://overcq.ct8.pl/system-operacyjny-ouxc.html

## 2. Vulnerabilities

Report vulnerabilities: https://github.com/overcq/kernel/security/advisories or via e‐mail: overcq@int.pl

## 3. Purpose

Operating system for x86_64 architecture.

## 4. Needed to build



## 5. Support

Technical security support at: https://overcq.ct8.pl/system-operacyjny-ouxc.html and via e‐mail: overcq@int.pl

Issues at: https://github.com/overcq/kernel/issues

Product support continues for the latest git release.

## 6. Installation and update

Fetching for first use can be done with a command, for example `git clone https://github.com/overcq/kernel.git`.

The user should make regular updates, e.g. with a command `git pull && make`.
Before using this commands can be executed: `make clean`, to remove any remaining intermediate files that depend on source files that have been removed in the new product version.

To completely remove the product from the userʼs device, execute: `rm -fr kernel`.

## 7. Building and launching

On a Linux operating system (e.g., Gentoo), download the https://github.com/overcq/linux.git repository containing the modified Linux kernel, configure it, build it, and install it. Install the virtual machine program and create a virtual machine for the operating system that boots via UEFI.

Download the https://github.com/overcq/linux-ouxfs-tools.git repository containing “mkfs.oux”, build it, and install it. Download the https://github.com/overcq/boot.git repository containing the OUX/C+ OS bootloader and the https://github.com/overcq/kernel.git repository containing the OUX/C+ OS kernel.

Configure the virtual machine disk path in the “bootloader” and “kernel” Makefiles for the selected “init-”* and “install-”* targets. Execute the command in the bootloader directory for the selected target: `make && make init-`* ` && make install-`*, and then in the kernel directory: `make && make install-`*. Start the virtual machine.
