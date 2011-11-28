--- src/log_pgsql.c.orig	2011-10-30 18:20:13.000000000 -0400
+++ src/log_pgsql.c	2011-11-22 10:38:05.000000000 -0500
@@ -375,6 +375,69 @@
     return answer;    
 }
 
+#ifdef WITH_VIRTUALDIRS
+void pw_pgsql_virtualdirquery(PGconn * const id_sql_server,
+                            const char * const orig_query,
+                            const char * const account,
+                            const char * const ip,
+                            const char * const port,
+                            const char * const peer_ip,
+                            const char * const decimal_ip)
+{
+    char query[PGSQL_MAX_REQUEST_LENGTH];
+    int i;
+    PGresult *virtualdir_res;
+    size_t length_name, length_base;
+
+    if (orig_query == NULL || *orig_query == 0) {
+        goto bye;
+    }
+    if (sqlsubst(orig_query, query, sizeof query,
+                account, ip, port, peer_ip, decimal_ip) == NULL) {
+        goto bye;
+    }
+    if ((virtualdir_res = PQexec(id_sql_server, query)) == NULL) {
+        logfile(LOG_WARNING, MSG_SQL_WRONG_PARMS " : [%s]", query);
+        goto bye;
+    }
+
+    if (PQresultStatus(virtualdir_res) != PGRES_TUPLES_OK ||
+        PQnfields(virtualdir_res) != 2) {
+        goto bye;
+    }
+
+    num_virtualdirs = PQntuples(virtualdir_res);
+    if ((virtualdirs = malloc(num_virtualdirs * sizeof(virtualdir_info *))) == NULL) {
+        die_mem();
+    }
+    for (i=0; i<num_virtualdirs; i++) {
+        length_name = (size_t) PQgetlength(virtualdir_res, i, 0)  + (size_t) 1U;
+        length_base = (size_t) PQgetlength(virtualdir_res, i, 1)  + (size_t) 1U;
+
+	if (length_base <= (size_t) 1U || length_name <= (size_t) 1U) {
+	    goto bye;
+	}
+
+        if ((virtualdirs[i] = malloc(sizeof(virtualdir_info))) == NULL ||
+            (virtualdirs[i]->virtualdir_base = malloc(sizeof(char) * length_base)) == NULL ||
+            (virtualdirs[i]->virtualdir_name = malloc(sizeof(char) * length_name))  == NULL) {
+            die_mem();
+        }
+
+	strncpy(virtualdirs[i]->virtualdir_name, PQgetvalue(virtualdir_res, i, 0), length_name - (size_t) 1U);
+        strncpy(virtualdirs[i]->virtualdir_base, PQgetvalue(virtualdir_res, i, 1), length_base - (size_t) 1U);
+	virtualdirs[i]->virtualdir_name[length_name - (size_t) 1U] = 0;
+        virtualdirs[i]->virtualdir_base[length_base - (size_t) 1U] = 0;
+    }
+    bye:
+    if (virtualdir_res != NULL) {
+        PQclear(virtualdir_res);
+    }
+    return;
+}
+#endif
+
+
 void pw_pgsql_check(AuthResult * const result,
                     const char *account, const char *password,
                     const struct sockaddr_storage * const sa,
@@ -491,6 +554,13 @@
                                  escaped_decimal_ip)) == NULL) {
         goto bye;
     }
+#ifdef WITH_VIRTUALDIRS
+    pw_pgsql_virtualdirquery(id_sql_server, sql_getvirtualdirs,
+                                escaped_account, escaped_ip,
+                                escaped_port, escaped_peer_ip,
+                                escaped_decimal_ip);
+#endif
+
     result->auth_ok--;                  /* -1 */
     if (strcasecmp(crypto, PASSWD_SQL_ANY) == 0) {
         crypto_crypt++;
@@ -706,6 +776,7 @@
     ZFREE(sqlreq_getgid);
     ZFREE(sql_default_gid);
     ZFREE(sqlreq_getdir);
+    ZFREE(sql_getvirtualdirs);
 #ifdef QUOTAS
     ZFREE(sqlreq_getqta_fs);
     ZFREE(sqlreq_getqta_sz);
