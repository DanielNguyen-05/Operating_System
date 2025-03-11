#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// Helper function to extract the file name from a path
char* fmtname(char* path) {
  static char buf[DIRSIZ + 1]; 
  char* p;

  // Find the first character after the last slash ('/')
  for (p = path + strlen(path); p >= path && *p != '/'; p--);
  p++;

  if (strlen(p) >= DIRSIZ)
    return p;

  // Otherwise, copy the file name into the buffer and ensure null termination
  memmove(buf, p, strlen(p));
  buf[strlen(p)] = '\0'; 
  return buf;
}

// Recursive function to search for a file with a specific name
void find(char* path, char* filename) {
  char buf[512], * p;         
  int fd;                    
  struct dirent de;          
  struct stat st;           

  // Try to open the directory; print error if it fails
  if ((fd = open(path, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // If the path is a file, check if the name matches
  if (st.type == T_FILE) {
    if (strcmp(fmtname(path), filename) == 0) {
      // Print the full path of the matching file
      printf("%s\n", path);
    }
  }
  // If the path is a directory, recursively search its contents
  else if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      printf("find: path too long\n");
      close(fd);
      return;
    }

    // Prepare the base path for constructing full paths of entries
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    // Read directory entries one by one
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      // Skip unused entries or the special entries "." and ".."
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      // Append the entry name to the base path
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      // Recursively search this entry
      find(buf, filename);
    }
  }
  close(fd);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
