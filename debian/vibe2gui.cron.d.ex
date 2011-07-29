#
# Regular cron jobs for the vibe2gui package
#
0 4	* * *	root	[ -x /usr/bin/vibe2gui_maintenance ] && /usr/bin/vibe2gui_maintenance
