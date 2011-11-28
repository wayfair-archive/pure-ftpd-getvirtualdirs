--- src/log_pgsql_p.h.orig	2011-11-21 16:38:38.000000000 -0500
+++ src/log_pgsql_p.h	2011-11-21 16:40:53.000000000 -0500
@@ -16,6 +16,7 @@
 static char *sqlreq_getdir;
 static char *sql_default_uid;
 static char *sql_default_gid;
+static char *sql_getvirtualdirs;
 #ifdef QUOTAS
 static char *sqlreq_getqta_sz;
 static char *sqlreq_getqta_fs;
@@ -43,6 +44,7 @@
     { "PGSQLGetGID", &sqlreq_getgid },
     { "PGSQLDefaultGID", &sql_default_gid },
     { "PGSQLGetDir", &sqlreq_getdir },
+    { "PGSQLGetVirtualDirs", &sql_getvirtualdirs },
 #ifdef QUOTAS
     { "PGSQLGetQTAFS", &sqlreq_getqta_fs },    
     { "PGSQLGetQTASZ", &sqlreq_getqta_sz },
