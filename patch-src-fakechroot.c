--- src/fakechroot.c.orig	2011-09-07 01:02:03.000000000 -0400
+++ src/fakechroot.c	2011-11-16 15:55:30.000000000 -0500
@@ -11,12 +11,16 @@
 # endif
 
 static char curdir[MAXPATHLEN];
+static char curdir_virtual[MAXPATHLEN];
+static char dir_workspace[MAXPATHLEN];
+static char dir_storage[MAXPATHLEN];
 static char *chroot_base;
 static size_t chroot_len;
 
 int fakechroot(const char *path)
 {
-    char *z;
+    char *z, *curr_virtualdir;
+    int i;
     
     if (path == NULL || *path == 0) {
 # ifdef EINVAL
@@ -50,7 +54,46 @@
 # endif
         return -1;
     }
-    
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
     return 0;
 }
 
@@ -62,10 +105,10 @@
     if (chroot_base == NULL) {
         return getcwd(dir, size);
     }
-    if (strncmp(curdir, chroot_base, chroot_len) != 0) {
+    if (strncmp(curdir_virtual, chroot_base, chroot_len) != 0) {
         abort();
     }
-    curdirchr = curdir + chroot_len;
+    curdirchr = curdir_virtual + chroot_len;
     s = strlen(curdirchr);
     if (s <= (size_t) 0U) {
 # ifdef EFAULT
@@ -97,11 +140,12 @@
 {
     char *sl;     
     size_t curdirlen;
+    int i;
     
-    if ((curdirlen = strlen(curdir)) >= sizeof_curdirtmp) {
+    if ((curdirlen = strlen(curdir_virtual)) >= sizeof_curdirtmp) {
         return -1;
     }
-    memcpy(curdirtmp, curdir, curdirlen + (size_t) 1U);
+    memcpy(curdirtmp, curdir_virtual, curdirlen + (size_t) 1U);
     simplify(curdirtmp);
     loop:
     if (dir[0] == '.' && dir[1] == '.' &&
@@ -153,7 +197,19 @@
         memcpy(curdirtmp + curdirtmplen + 1U, dir, dirlen);
     }     
     simplify(curdirtmp);
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
+
     return 0;
 }
 
@@ -175,7 +231,7 @@
         return -1;
     }
     memcpy(curdir, curdirtmp, curdirtmplen + (size_t) 1U);
-    
+    memcpy(curdir_virtual, dir_storage, sizeof dir_storage + (size_t) 1U);
     return 0;
 }
 
