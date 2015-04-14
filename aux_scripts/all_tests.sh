#!/bin/sh

cd /home/user/src/mipster-for-students/;

(set -x; make)

cd /home/user/src/mipster-for-students/userland/cpu-torture-tests;

for I in $(ls |grep '^cputorture-[a-z]\+$'); do
	(set -x; ~/src/mipster-for-students/out/Default/mipster -torture_mode $I)
done

