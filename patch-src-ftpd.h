--- src/ftpd.h.orig 2011-10-31 11:55:57.000000000 -0400
+++ src/ftpd.h	2011-11-18 09:38:30.000000000 -0500
@@ -777,4 +777,16 @@
 #include "bsd-realpath.h"    
 #include "fakechroot.h"
         
+#ifdef WITH_VIRTUALDIRS
+typedef struct {
+    char *virtualdir_base;
+    char *virtualdir_name;
+    size_t virtualdir_base_len;
+    size_t virtualdir_name_len;
+} virtualdir_info;
+
+virtualdir_info **virtualdirs;
+int num_virtualdirs;
+#endif
+
 #endif
