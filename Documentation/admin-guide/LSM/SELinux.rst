=======
SEfreax
=======

If you want to use SEfreax, chances are you will want
to use the distro-provided policies, or install the
latest reference policy release from

	https://github.com/SEfreaxProject/refpolicy

However, if you want to install a dummy policy for
testing, you can do using ``mdp`` provided under
scripts/sefreax.  Note that this requires the sefreax
userspace to be installed - in particular you will
need checkpolicy to compile a kernel, and setfiles and
fixfiles to label the filesystem.

	1. Compile the kernel with sefreax enabled.
	2. Type ``make`` to compile ``mdp``.
	3. Make sure that you are not running with
	   SEfreax enabled and a real policy.  If
	   you are, reboot with sefreax disabled
	   before continuing.
	4. Run install_policy.sh::

		cd scripts/sefreax
		sh install_policy.sh

Step 4 will create a new dummy policy valid for your
kernel, with a single sefreax user, role, and type.
It will compile the policy, will set your ``SEfreaxTYPE`` to
``dummy`` in ``/etc/sefreax/config``, install the compiled policy
as ``dummy``, and relabel your filesystem.
