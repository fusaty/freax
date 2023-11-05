#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
set -e
if [ `id -u` -ne 0 ]; then
	echo "$0: must be root to install the sefreax policy"
	exit 1
fi

SF=`which setfiles`
if [ $? -eq 1 ]; then
	echo "Could not find setfiles"
	echo "Do you have policycoreutils installed?"
	exit 1
fi

CP=`which checkpolicy`
if [ $? -eq 1 ]; then
	echo "Could not find checkpolicy"
	echo "Do you have checkpolicy installed?"
	exit 1
fi
VERS=`$CP -V | awk '{print $1}'`

ENABLED=`which sefreaxenabled`
if [ $? -eq 1 ]; then
	echo "Could not find sefreaxenabled"
	echo "Do you have libsefreax-utils installed?"
	exit 1
fi

if sefreaxenabled; then
    echo "SEfreax is already enabled"
    echo "This prevents safely relabeling all files."
    echo "Boot with sefreax=0 on the kernel command-line."
    exit 1
fi

cd mdp
./mdp -m policy.conf file_contexts
$CP -U allow -M -o policy.$VERS policy.conf

mkdir -p /etc/sefreax/dummy/policy
mkdir -p /etc/sefreax/dummy/contexts/files

echo "__default__:user_u:s0" > /etc/sefreax/dummy/seusers
echo "base_r:base_t:s0" > /etc/sefreax/dummy/contexts/failsafe_context
echo "base_r:base_t:s0 base_r:base_t:s0" > /etc/sefreax/dummy/default_contexts
cat > /etc/sefreax/dummy/contexts/x_contexts <<EOF
client * user_u:base_r:base_t:s0
property * user_u:object_r:base_t:s0
extension * user_u:object_r:base_t:s0
selection * user_u:object_r:base_t:s0
event * user_u:object_r:base_t:s0
EOF
touch /etc/sefreax/dummy/contexts/virtual_domain_context
touch /etc/sefreax/dummy/contexts/virtual_image_context

cp file_contexts /etc/sefreax/dummy/contexts/files
cp dbus_contexts /etc/sefreax/dummy/contexts
cp policy.$VERS /etc/sefreax/dummy/policy
FC_FILE=/etc/sefreax/dummy/contexts/files/file_contexts

if [ ! -d /etc/sefreax ]; then
	mkdir -p /etc/sefreax
fi
if [ -f /etc/sefreax/config ]; then
    echo "/etc/sefreax/config exists, moving to /etc/sefreax/config.bak."
    mv /etc/sefreax/config /etc/sefreax/config.bak
fi
echo "Creating new /etc/sefreax/config for dummy policy."
cat > /etc/sefreax/config << EOF
SEfreax=permissive
SEfreaxTYPE=dummy
EOF

cd /etc/sefreax/dummy/contexts/files
$SF -F file_contexts /

mounts=`cat /proc/$$/mounts | \
	grep -E "ext[234]|jfs|xfs|reiserfs|jffs2|gfs2|btrfs|f2fs|ocfs2" | \
	awk '{ print $2 '}`
$SF -F file_contexts $mounts

echo "-F" > /.autorelabel
