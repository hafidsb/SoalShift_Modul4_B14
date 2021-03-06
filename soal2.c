#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/aldinata/Documents";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ;
//start_1
char cek[5]; //buat ngecek ekstensi
memset(cek, '\0', sizeof(cek));
int eks= strlen(fpath)-4;
cek[0]=fpath[eks];
cek[1]=fpath[eks+1];
cek[2]=fpath[eks+2];
cek[3]=fpath[eks+3];
cek[4]='\0';
if(strcmp(cek, ".txt") == 0 || strcmp(cek, ".doc") == 0 || strcmp(cek, ".pdf") == 0){
	char tampileror[2000]={"zenity --error --text=\"Terjadi kesalahan! File berisi konten berbahaya.\""};
	system(tampileror);
	//menambahkan .ditandai	
	char new[1000];
	strcpy(new, fpath);
	strcat(new, ".ditandai");
	sprintf(tampileror, "mv %s %s", fpath, new); //mengisi string tampileror dg mv ...
	system(tampileror); // merename file di disk
	//finish_1
	//start_2
	mode_t modedir = 0777;
	sprintf(tampileror, "%s/rahasia", dirpath);
	xmp_mkdir(tampileror, modedir);
	sprintf(tampileror,"mv %s %s/rahasia/%s.ditandai", new, dirpath, path);
  	system(tampileror);
	mode_t ubah = 0;
	xmp_chmod(new, ubah);
	return 0;
}
//finish2
	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.chmod	= xmp_chmod, //soal2
	.mkdir	= xmp_mkdir,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
