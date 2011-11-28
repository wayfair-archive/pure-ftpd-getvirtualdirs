--- src/fakechroot.c.orig 2011-09-07 01:02:03.000000000 -0400
+++ src/fakechroot.c	2011-11-18 09:57:05.000000000 -0500
@@ -13,10 +13,16 @@
 static char curdir[MAXPATHLEN];
 static char *chroot_base;
 static size_t chroot_len;
+#ifdef WITH_VIRTUALDIRS
+static char curdir_virtual[MAXPATHLEN];
+static char dir_workspace[MAXPATHLEN];
+static char dir_storage[MAXPATHLEN];
+#endif
 
 int fakechroot(const char *path)
 {
-    char *z;
+    char *z, *curr_virtualdir;
+    int i;
     
     if (path == NULL || *path == 0) {
 # ifdef EINVAL
@@ -50,7 +56,48 @@
 # endif
         return -1;
     }
-    
+#ifdef WITH_VIRTUALDIRS
+    for (i=0; i<num_virtualdirs; i++) {
+	curr_virtualdir = virtualdirs[i]->virtualdir_base;
+	simplify(curr_virtualdir);
+	z = curr_virtualdir;
+        while (*z != 0) {
+            z++;
+        }
+        for (;;) {
+            z--;
+            if (z == curr_virtualdir || *z != '/') {
+                break;
+            }
+            *z = 0;
+        }
+        if ((virtualdirs[i]->virtualdir_base_len = strlen(curr_virtualdir)) >= sizeof curdir) {
+# ifdef ENAMETOOLONG
+            errno = ENAMETOOLONG;
+# endif
+            return -1;
+        }
+	curr_virtualdir = virtualdirs[i]->virtualdir_name;
+        simplify(curr_virtualdir);
+        z = curr_virtualdir;
+        while (*z != 0) {
+            z++;
+        }
+        for (;;) {
+            z--;
+            if (z == curr_virtualdir || *z != '/') {
+                break;
+            }
+            *z = 0;
+        }
+        if ((virtualdirs[i]->virtualdir_name_len = strlen(curr_virtualdir)) >= sizeof curdir_virtual) {
+# ifdef ENAMETOOLONG
+            errno = ENAMETOOLONG;
+# endif
+            return -1;
+        }
+    }
+#endif
     return 0;
 }
 
@@ -62,10 +109,17 @@
     if (chroot_base == NULL) {
         return getcwd(dir, size);
     }
+#ifdef WITH_VIRTUALDIRS
+    if (strncmp(curdir_virtual, chroot_base, chroot_len) != 0) {
+        abort();
+    }
+    curdirchr = curdir_virtual + chroot_len;
+#else
     if (strncmp(curdir, chroot_base, chroot_len) != 0) {
         abort();
     }
     curdirchr = curdir + chroot_len;
+#endif
     s = strlen(curdirchr);
     if (s <= (size_t) 0U) {
 # ifdef EFAULT
@@ -97,11 +151,19 @@
 {
     char *sl;     
     size_t curdirlen;
+#ifdef WITH_VIRTUALDIRS
+    int i;
     
+    if ((curdirlen = strlen(curdir_virtual)) >= sizeof_curdirtmp) {
+        return -1;
+    }
+    memcpy(curdirtmp, curdir_virtual, curdirlen + (size_t) 1U);
+#else
     if ((curdirlen = strlen(curdir)) >= sizeof_curdirtmp) {
         return -1;
     }
     memcpy(curdirtmp, curdir, curdirlen + (size_t) 1U);
+#endif
     simplify(curdirtmp);
     loop:
     if (dir[0] == '.' && dir[1] == '.' &&
@@ -153,7 +215,19 @@
         memcpy(curdirtmp + curdirtmplen + 1U, dir, dirlen);
     }     
     simplify(curdirtmp);
-    
+#ifdef WITH_VIRTUALDIRS
+    memcpy(dir_storage, curdirtmp, sizeof dir_storage);
+    for(i=0; i<num_virtualdirs; i++) {
+	if (strncmp(curdirtmp + chroot_len, virtualdirs[i]->virtualdir_name, virtualdirs[i]->virtualdir_name_len) == 0) {
+	    memcpy(dir_workspace, virtualdirs[i]->virtualdir_base, virtualdirs[i]->virtualdir_base_len);
+	    snprintf(dir_workspace + virtualdirs[i]->virtualdir_base_len, sizeof dir_workspace - virtualdirs[i]->virtualdir_base_len, 
+		"%s/", curdirtmp + chroot_len + virtualdirs[i]->virtualdir_name_len);
+	    snprintf(curdirtmp, sizeof_curdirtmp, "%s/", dir_workspace);
+	    simplify(curdirtmp);
+	    return 0;
+	}
+    }
+#endif
     return 0;
 }
 
@@ -175,7 +249,9 @@
         return -1;
     }
     memcpy(curdir, curdirtmp, curdirtmplen + (size_t) 1U);
-    
+#ifdef WITH_VIRTUALDIRS
+    memcpy(curdir_virtual, dir_storage, sizeof dir_storage + (size_t) 1U);
+#endif
     return 0;
 }
 
