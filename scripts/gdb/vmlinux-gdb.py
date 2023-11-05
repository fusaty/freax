#
# gdb helper commands and functions for freax kernel debugging
#
#  loader module
#
# Copyright (c) Siemens AG, 2012, 2013
#
# Authors:
#  Jan Kiszka <jan.kiszka@siemens.com>
#
# This work is licensed under the terms of the GNU GPL version 2.
#

import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + "/scripts/gdb")

try:
    gdb.parse_and_eval("0")
    gdb.execute("", to_string=True)
except:
    gdb.write("NOTE: gdb 7.2 or later required for freax helper scripts to "
              "work.\n")
else:
    import freax.constants
    if freax.constants.LX_CONFIG_DEBUG_INFO_REDUCED:
        raise gdb.GdbError("Reduced debug information will prevent GDB "
                           "from having complete types.\n")
    import freax.utils
    import freax.symbols
    import freax.modules
    import freax.dmesg
    import freax.tasks
    import freax.config
    import freax.cpus
    import freax.lists
    import freax.rbtree
    import freax.proc
    import freax.timerlist
    import freax.clk
    import freax.genpd
    import freax.device
    import freax.vfs
    import freax.pgtable
    import freax.radixtree
    import freax.interrupts
    import freax.mm
    import freax.stackdepot
    import freax.page_owner
    import freax.slab
    import freax.vmalloc
