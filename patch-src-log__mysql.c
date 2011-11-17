--- src/log_mysql.c.orig	2011-10-30 18:20:17.000000000 -0400
+++ src/log_mysql.c	2011-11-16 15:37:20.000000000 -0500
@@ -8,7 +8,6 @@
 # include "log_mysql.h"
 # include "messages.h"
 # include "crypto.h"
-# include "alt_arc4random.h"
 
 # ifdef WITH_DMALLOC
 #  include <dmalloc.h>
@@ -295,6 +294,62 @@
     return answer;    
 }
 
+void pw_mysql_virtualdirquery(MYSQL * const id_sql_server,
+			    const char * const orig_query,
+			    const char * const account,
+			    const char * const ip,
+			    const char * const port,
+			    const char * const peer_ip,
+			    const char * const decimal_ip)
+{
+    char query[MYSQL_MAX_REQUEST_LENGTH];
+    char name[MAXPATHLEN + 1U];
+    char base[MAXPATHLEN + 1U];
+    int i;
+    unsigned long *lengths;
+    MYSQL_ROW row;
+    MYSQL_RES *virtualdir_res;
+    size_t length1, length2;
+
+    if (orig_query == NULL || *orig_query == 0) {
+	return;
+    }
+    if (sqlsubst(orig_query, query, sizeof query,
+		account, ip, port, peer_ip, decimal_ip) == NULL) {
+	return;
+    }
+    if (mysql_real_query(id_sql_server, query, strlen(query)) != 0) {
+	logfile(LOG_WARNING, MSG_SQL_WRONG_PARMS " : [%s]", query);
+	return;
+    }
+
+    virtualdir_res = mysql_store_result(id_sql_server);
+
+    num_virtualdirs = mysql_num_rows(virtualdir_res);
+    if ((virtualdirs = malloc(num_virtualdirs * sizeof(virtualdir_info *))) == NULL) {
+        die_mem();
+    }
+    for (i=0; i<num_virtualdirs; i++) {
+        row = mysql_fetch_row(virtualdir_res);
+        lengths = mysql_fetch_lengths(virtualdir_res);
+        length1 = (size_t) lengths[0] + (size_t) 1U;
+        length2 = (size_t) lengths[1] + (size_t) 1U;
+        strncpy(name, row[0], length1 - (size_t) 1U);
+        name[length1 - (size_t) 1U] = 0;
+        strncpy(base, row[1], length2 - (size_t) 1U);
+        base[length2 - (size_t) 1U] = 0;
+
+        if ((virtualdirs[i] = malloc(sizeof(virtualdir_info))) == NULL ||
+            (virtualdirs[i]->virtualdir_base = strdup(base)) == NULL ||
+            (virtualdirs[i]->virtualdir_name = strdup(name)) == NULL) {
+            die_mem();
+        }
+    }
+
+    mysql_free_result(virtualdir_res);
+    return;
+}
+
 void pw_mysql_check(AuthResult * const result,
                     const char *account, const char *password,
                     const struct sockaddr_storage * const sa,
@@ -413,6 +468,12 @@
                                  escaped_decimal_ip)) == NULL) {
         goto bye;
     }
+
+    pw_mysql_virtualdirquery(id_sql_server, sql_getvirtualdirs,
+				escaped_account, escaped_ip,
+				escaped_port, escaped_peer_ip,
+				escaped_decimal_ip);
+
     result->auth_ok--;                  /* -1 */
     if (strcasecmp(crypto, PASSWD_SQL_ANY) == 0) {
         crypto_crypt++;
