#   sw_rename
Rename files with regular expressions. 

Created with Qt with the purpose to be used alongside KDEs Dolphin file manager as a service or an extension. Although it's written and tested in Opensuse Leap 42.1 - 15.1. it should be working as expected in many other environments with or without Dolphin, just try it out. Also feel free to report bugs, propose features or extend it/modify it by yourself 

As of now, changes done in files with this rename utility are not reversible, if you're renaming something important keep backups before doing so. I'm in no case to be held responsible for any harm it may bring to your computer or files, use at your own risk.

#   How to use with Dolphin/Linux 
1. Compile the source code with Qt
2. Copy the executable to the wanted location
3. Edit rename.desktop file to point to that location 
4. Drop rename.desktop into $HOME/.local/share/kservices5/
5. You can now access it from the Actions submenu by right clicking inside a folders empty space
