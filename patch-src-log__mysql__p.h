--- src/log_mysql_p.h.orig	2011-09-07 01:02:03.000000000 -0400
+++ src/log_mysql_p.h	2011-11-16 15:37:00.000000000 -0500
@@ -24,6 +24,7 @@
 static char *sqlreq_getgid;
 static char *sql_default_gid;
 static char *sqlreq_getdir;
+static char *sql_getvirtualdirs;
 static char *tildexp_s;
 static int tildexp;
 #ifdef QUOTAS
@@ -55,6 +56,7 @@
     { "MYSQLGetGID", &sqlreq_getgid },
     { "MYSQLDefaultGID", &sql_default_gid },
     { "MYSQLGetDir", &sqlreq_getdir },
+    { "MYSQLGetVirtualDirs", &sql_getvirtualdirs },
     { "MYSQLForceTildeExpansion", &tildexp_s },
 #ifdef QUOTAS
     { "MYSQLGetQTAFS", &sqlreq_getqta_fs },    
