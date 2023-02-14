# Oberon-A2-Archive
Filtered and unfiltered archive of https://svn.inf.ethz.ch/svn/lecturers/a2/trunk (r10272) and https://www.ocp.inf.ethz.ch/svn/ocp (r829)

## File releases

In [File Releases](https://github.com/schierlm/Oberon-A2-Archive/releases/tag/binary-archives) you can download the raw repository dump of both repositories.

There is also a dump available, containing revisions 1-750 of the main repository, where parts which later were moved into `trunk` are moved there from
the beginning. As some commits needed to be reordered, the commit messages are prefixed by the revision number (`[r1] ` etc).
This dump is used for creating any `trunk` branches listed below.

## Branches

This repository contains several branches.

**[`ocp-svn`](https://github.com/schierlm/Oberon-A2-Archive/tree/ocp-svn)**: Unfiltered contents of repository https://www.ocp.inf.ethz.ch/svn/ocp/ as of May 2019 (r829).

**[`a2-svn-extras`](https://github.com/schierlm/Oberon-A2-Archive/tree/a2-svn-extras)**: Everything that was committed to the repository outide of `trunk`, `tags`, and `branches`.

**[`a2-svn-filtered-trunk`](https://github.com/schierlm/Oberon-A2-Archive/tree/a2-svn-filtered-trunk)**: Filtered contents of `trunk`. These paths are excluded:

- `/trunk/ARM`
- `/trunk/lin32`
- `/trunk/Linux32`
- `/trunk/Linux32G`
- `/trunk/Linux64`
- `/trunk/Linux64G`
- `/trunk/LinuxARM`
- `/trunk/Win32`
- `/trunk/win64`
- `/trunk/Win64`
- `/trunk/Win64G`
- `/trunk/WinAos/Obg`
- `/trunk/WinAos/Obj`
- `/trunk/WinAos/ObjE`
- `/trunk/WinAos/ObjI`
- `/trunk/WinAos/obg`
- `/trunk/WinAos/obj`
- `/trunk/WinAos/A2.exe`
- `/trunk/WinAos/Aos.EXE`
- `/trunk/WinAos/AosDebug.exe`
- `/trunk/WinAos/oberon.exe`

## Grafting history

You can follow this recipe to add history from this repo to a checkout of [the current A2 repository](https://gitlab.inf.ethz.ch/felixf/oberon):

```
git clone https://gitlab.inf.ethz.ch/felixf/oberon.git
cd oberon
git remote add archive https://github.com/schierlm/Oberon-A2-Archive.git
git fetch archive a2-svn-filtered-trunk
git checkout -b replace-9e55f03e archive/a2-svn-filtered-trunk
git reset 9e55f03e -- :/
git commit -m 'Create replace reference point'
git reset --hard main
git clean -fdx
git checkout main
git replace 9e55f03e replace-9e55f03e
```
