SUMMARY = "A suite of basic system administration utilities."
DESCRIPTION = "Util-linux includes a suite of basic system administration utilities \
commonly found on most Linux systems.  Some of the more important utilities include \
disk partitioning, kernel message management, filesystem creation, and system login."

SECTION = "base"

LICENSE = "GPLv2+ & LGPLv2.1+ & BSD"

LIC_FILES_CHKSUM = "file://README.licensing;md5=9c920d811858a74b67a36ba23cbaa95f \
                    file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552 \
                    file://Documentation/licenses/COPYING.GPLv2;md5=8ca43cbc842c2336e835926c2166c28b \
                    file://Documentation/licenses/COPYING.LGPLv2.1;md5=bbb461211a33b134d42ed5ee802b37ff \
                    file://Documentation/licenses/COPYING.BSD-3;md5=58dcd8452651fc8b07d1f65ce07ca8af \
                    file://Documentation/licenses/COPYING.UCB;md5=263860f8968d8bafa5392cab74285262 \
                    file://getopt/COPYING;md5=c3a41733416fb80af7ba1b7e1f712551"

inherit autotools gettext pkgconfig
DEPENDS = "zlib ncurses"
DEPENDS_append_virtclass-native = " lzo-native"

SRC_URI = "${KERNELORG_MIRROR}/linux/utils/util-linux/v${MAJOR_VERSION}/util-linux-${PV}.tar.bz2 \
           file://MCONFIG \
           file://swapargs.h \
           file://defines.h \
           file://make_include \
           "

PACKAGES =+ "util-linux-agetty util-linux-fdisk util-linux-cfdisk util-linux-sfdisk \
             util-linux-swaponoff util-linux-losetup util-linux-umount \
             util-linux-mount util-linux-readprofile util-linux-libblkid \
             util-linux-libblkid-dev util-linux-libuuid util-linux-libuuid-dev \
             util-linux-uuidgen util-linux-lscpu util-linux-fsck util-linux-blkid \
             util-linux-chkdupexe util-linux-mkfs util-linux-mcookie"

EXTRA_OECONF = "--libdir=${base_libdir} --disable-use-tty-group \
                --disable-makeinstall-chown --enable-elvtune --enable-init \
                --enable-kill --enable-last --enable-mesg --enable-partx \
                --enable-raw --enable-rdev --enable-reset --disable-login-utils \
                --enable-write --enable-arch --enable-mount --with-fsprobe=builtin \
                --enable-libuuid --enable-libblkid --enable-fsck --without-udev"

FILES_${PN}-doc += "${datadir}/getopt/getopt-*.*"

FILES_util-linux-agetty = "${base_sbindir}/agetty"
FILES_util-linux-fdisk = "${base_sbindir}/fdisk.${BPN}"
FILES_util-linux-cfdisk = "${base_sbindir}/cfdisk"
FILES_util-linux-sfdisk = "${sbindir}/sfdisk"
FILES_util-linux-swaponoff = "${base_sbindir}/swapon.${BPN} ${base_sbindir}/swapoff.${BPN}"
FILES_util-linux-losetup = "${base_sbindir}/losetup.${BPN}"
FILES_util-linux-mount = "${base_bindir}/mount.${BPN}"
FILES_util-linux-mcookie = "${bindir}/mcookie"
FILES_util-linux-umount = "${base_bindir}/umount.${BPN}"
FILES_util-linux-readprofile = "${base_sbindir}/readprofile.${BPN}"
FILES_util-linux-uuidgen = "${bindir}/uuidgen"

FILES_util-linux-libblkid = "${base_libdir}/libblkid.so.*"
FILES_util-linux-libblkid-dev = "${base_libdir}/libblkid.so ${base_libdir}/libblkid.a ${base_libdir}/libblkid.la ${includedir}/blkid ${libdir}/pkgconfig/blkid.pc"
FILES_util-linux-libuuid = "${base_libdir}/libuuid.so.*"
FILES_util-linux-libuuid-dev = "${base_libdir}/libuuid.so ${base_libdir}/libuuid.a ${base_libdir}/libuuid.la ${includedir}/uuid ${libdir}/pkgconfig/uuid.pc"
FILES_util-linux-lscpu = "${bindir}/lscpu"

FILES_util-linux-fsck = "${base_sbindir}/fsck*"
FILES_util-linux-chkdupexe = "${bindir}/chkdupexe"
FILES_util-linux-mkfs = "${sbindir}/mkfs"

# Util-linux' blkid replaces the e2fsprogs one
FILES_util-linux-blkid = "${base_sbindir}/blkid*"
RCONFLICTS_util-linux-blkid = "e2fsprogs-blkid"
RREPLACES_util-linux-blkid = "e2fsprogs-blkid"

RDEPENDS_${PN} = "util-linux-umount util-linux-swaponoff util-linux-losetup"
RDEPENDS_${PN}-chkdupexe = "perl"
RRECOMMENDS_${PN} = "util-linux-fdisk util-linux-cfdisk util-linux-sfdisk util-linux-mount util-linux-readprofile util-linux-mkfs "

RRECOMMENDS_${PN}_virtclass-native = ""
RDEPENDS_${PN}_virtclass-native = ""

do_compile () {
	set -e
	install ${WORKDIR}/MCONFIG ${S}/MCONFIG
	install ${WORKDIR}/make_include ${S}/make_include
	install ${WORKDIR}/swapargs.h ${S}/mount/swapargs.h
	install ${WORKDIR}/defines.h ${S}/defines.h
	oe_runmake ARCH=${TARGET_ARCH} CPU= CPUOPT= 'OPT=${CFLAGS}' 
}

do_install () {
	# with ccache the timestamps on compiled files may
	# end up earlier than on their inputs, this allows
	# for the resultant compilation in the install step.
	oe_runmake ARCH=${TARGET_ARCH} CPU= CPUOPT= \
		'OPT=${CFLAGS}' 'CC=${CC}' 'LD=${LD}' \
		'LDFLAGS=${LDFLAGS}' 'DESTDIR=${D}' install

	mkdir -p ${D}${base_bindir}

        sbinprogs="agetty ctrlaltdel cfdisk vipw vigr"
        sbinprogs_a="pivot_root hwclock mkswap mkfs.minix fsck.minix losetup swapon fdisk readprofile fsck blkid blockdev"
        usrbinprogs_a="chfn chsh hexdump last logger mesg newgrp renice wall setsid chrt flock"
        binprogs_a="dmesg kill more umount mount login reset"

        if [ "${base_sbindir}" != "${sbindir}" ]; then
        	mkdir -p ${D}${base_sbindir}
                for p in $sbinprogs $sbinprogs_a; do
                        if [ -f "${D}${sbindir}/$p" ]; then
                                mv "${D}${sbindir}/$p" "${D}${base_sbindir}/$p"
                        fi
                done
        fi

        if [ "${base_bindir}" != "${bindir}" ]; then
        	mkdir -p ${D}${base_bindir}
                for p in $binprogs_a; do
                        if [ -f "${D}${bindir}/$p" ]; then
                                mv "${D}${bindir}/$p" "${D}${base_bindir}/$p"
                        fi
                done
        fi

	install -d ${D}${sysconfdir}/default/
	echo 'MOUNTALL="-t nonfs,nosmbfs,noncpfs"' > ${D}${sysconfdir}/default/mountall

	ln -sf swapon.${BPN}   ${D}${base_sbindir}/swapoff.${BPN}
	rm -f ${D}${sbindir}/swapoff
}

inherit update-alternatives

ALTERNATIVE_PRIORITY = "100"

ALTERNATIVE_${PN}  = "dmesg kill more reset mkswap blockdev pivot_root"
ALTERNATIVE_${PN} += "mkfs.minix hexdump last logger mesg renice wall"
ALTERNATIVE_${PN} += "setsid chrt flock hwclock"

ALTERNATIVE_LINK_NAME[dmesg] = "${base_bindir}/dmesg"
ALTERNATIVE_LINK_NAME[kill] = "${base_bindir}/kill"
ALTERNATIVE_LINK_NAME[more] = "${base_bindir}/more"
ALTERNATIVE_LINK_NAME[reset] = "${bindir}/reset"
ALTERNATIVE_TARGET[reset] = "${base_bindir}/reset"
ALTERNATIVE_LINK_NAME[mkswap] = "${base_sbindir}/mkswap"
ALTERNATIVE_LINK_NAME[blockdev] = "${base_sbindir}/blockdev"
ALTERNATIVE_LINK_NAME[pivot_root] = "${base_sbindir}/pivot_root"
ALTERNATIVE_LINK_NAME[mkfs.minix] = "${base_sbindir}/mkfs.minix"

# There seems to be problem, atleast on nslu2, with these, untill they are
# fixed the busybox ones have higher priority
ALTERNATIVE_PRIORITY[hwclock] = "10"
ALTERNATIVE_LINK_NAME[hwclock] = "${base_sbindir}/hwclock"

ALTERNATIVE_util-linux-fdisk = "fdisk"
ALTERNATIVE_LINK_NAME[fdisk] = "${base_sbindir}/fdisk"

ALTERNATIVE_util-linux-agetty = "getty"
ALTERNATIVE_LINK_NAME[getty] = "${base_sbindir}/getty"
ALTERNATIVE_TARGET[getty] = "${base_sbindir}/agetty"

ALTERNATIVE_util-linux-mount = "mount"
ALTERNATIVE_LINK_NAME[mount] = "${base_bindir}/mount"

ALTERNATIVE_util-linux-umount = "umount"
ALTERNATIVE_LINK_NAME[umount] = "${base_bindir}/umount"

ALTERNATIVE_util-linux-losetup = "losetup"
ALTERNATIVE_LINK_NAME[losetup] = "${base_sbindir}/losetup"

ALTERNATIVE_util-linux-swaponoff = "swapoff swapon"
ALTERNATIVE_LINK_NAME[swapoff] = "${base_sbindir}/swapoff"
ALTERNATIVE_LINK_NAME[swapon] = "${base_sbindir}/swapon"

ALTERNATIVE_util-linux-fsck = "fsck.minix fsck"
ALTERNATIVE_LINK_NAME[fsck.minix] = "${base_sbindir}/fsck.minix"
ALTERNATIVE_LINK_NAME[fsck] = "${base_sbindir}/fsck"

ALTERNATIVE_util-linux-blkid = "blkid"
ALTERNATIVE_LINK_NAME[blkid] = "${base_sbindir}/blkid"

BBCLASSEXTEND = "native"
