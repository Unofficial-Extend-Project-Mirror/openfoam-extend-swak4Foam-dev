#
# Regular cron jobs for the swak4foam package
#
0 4	* * *	root	[ -x /usr/bin/swak4foam_maintenance ] && /usr/bin/swak4foam_maintenance
