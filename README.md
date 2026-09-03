# OUX/C+ OS

# Information and instructions to the user

## 1. Contact

Manufacturer: overcq

Contact the manufacturer: overcq@int.pl

Source code: https://github.com/overcq/kernel

Description: https://overcq.ct8.pl/oux-c-plus-os.html

## 2. Vulnerabilities

Report vulnerabilities: https://github.com/overcq/kernel/security/advisories or via e‐mail: overcq@int.pl

## 3. Purpose

Operating system for x86_64 architecture.

## 4. Needed to build

* Linux
* “elf2oux”
* “mkfs.oux”
* boot loader

## 5. Support

Technical security support at: https://overcq.ct8.pl/oux-c-plus-os.html and via e‐mail: overcq@int.pl

Issues at: https://github.com/overcq/kernel/issues

Product support continues for the latest git release.

## 6. Installation and update

Fetching for first use can be done with a command, for example `git clone https://github.com/overcq/kernel.git`.

The user should make regular updates, e.g. with a command `git pull && make`.
Before using this commands can be executed: `make clean`, to remove any remaining intermediate files that depend on source files that have been removed in the new product version.

To completely remove the product from the userʼs device, execute: `rm -fr kernel`.

## 7. Building and launching

On a Linux operating system (e.g., Gentoo), download the https://github.com/overcq/linux.git repository containing the modified Linux kernel, configure, build and install it. Install the virtual machine program and create a virtual machine for the operating system.

Download the https://github.com/overcq/oux.git repository, build and install it. Download the https://github.com/overcq/linux-ouxfs-tools.git repository containing “mkfs.oux” and build it. Download the https://github.com/overcq/elf2oux.git repository containing “elf2oux”, build and install it. Download the https://github.com/overcq/boot.git repository containing the OUX/C+ OS boot loader and the https://github.com/overcq/kernel.git repository containing the OUX/C+ OS kernel.

Set environment variables in “env.mk” file in the boot loader repository. Execute the command in the boot loader directory for the selected target: `make && make init-`\*` && make install-`\*, and then in the kernel directory: `make && make install-`\*. Start the virtual machine.
