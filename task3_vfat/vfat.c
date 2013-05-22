#define FUSE_USE_VERSION 26
#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/mman.h>

#ifdef __unix__
#include <endian.h>
#endif

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct vfat_super {
	uint8_t		res1[3];
	char		oemname[8];
	uint16_t	bytes_per_sector;
	uint8_t		sectors_per_cluster;
	uint16_t	reserved_sectors;
	uint8_t		fat_count;
	uint16_t	root_max_entries;
	uint16_t	total_sectors_small;
	uint8_t		media_info;
	uint16_t	sectors_per_fat_small;
	uint16_t	sectors_per_track;
	uint16_t	head_count;
	uint32_t	fs_offset;
	uint32_t	total_sectors;
	uint32_t	sectors_per_fat;
	uint16_t	fat_flags;
	uint16_t	version;
	uint32_t	root_cluster;
	uint16_t	fsinfo_sector;
	uint16_t	backup_sector;
	uint8_t		res2[12];
	uint8_t		drive_number;
	uint8_t		res3;
	uint8_t		ext_sig;
	uint32_t	serial;
	char		label[11];
	char		type[8];
	char		res4[420];
	uint16_t	sig;
} __attribute__ ((__packed__));

struct vfat_direntry {
	union {
		struct {
			char		name[8];
			char		ext[3];
		};
		char			nameext[11];
	};
	uint8_t		attr;
	uint8_t		res;
	uint8_t		ctime_ms;
	uint16_t	ctime_time;
	uint16_t	ctime_date;
	uint16_t	atime_date;
	uint16_t	cluster_hi;
	uint16_t	mtime_time;
	uint16_t	mtime_date;
	uint16_t	cluster_lo;
	uint32_t	size;
} __attribute__ ((__packed__));

#define VFAT_ATTR_DIR	0x10
#define VFAT_ATTR_LFN	0xf
#define VFAT_ATTR_INVAL	(0x80|0x40|0x08)

// LFN stands for Long File Name!!!
struct vfat_direntry_lfn {
	uint8_t		seq;
	uint16_t	name1[5];
	uint8_t		attr;
	uint8_t		res1;
	uint8_t		csum;
	uint16_t	name2[6];
	uint16_t	res2;
	uint16_t	name3[2];
} __attribute__ ((__packed__));

#define VFAT_LFN_SEQ_START	0x40
#define VFAT_LFN_SEQ_DELETED	0x80
#define VFAT_LFN_SEQ_MASK	0x3f

struct vfat {
	const char	*dev;
	int		fs;
  
	/* XXX add your code here */
  
  // Contains the boot sector, as well as the FS Information Sector and the last reserved sectors.
  // it is the FAT header that contains useful informations about the partition.
  struct vfat_super* boot_sector;
  
  // TODO : Do we store the root directory and the FAT's in here?
  // struct vfat_dir* root_directory;
  
};

struct vfat_search_data {
	const char	*name;
	int		found;
	struct stat	*st;
};

// This is the thing we'll be working with as soon as we have to access the FAT partition.
struct vfat vfat_info, *f = &vfat_info;

iconv_t iconv_utf16;

uid_t mount_uid;
gid_t mount_gid;
time_t mount_time;
size_t pagesize;

static void vfat_init(const char *dev) {
  
	iconv_utf16 = iconv_open("utf-8", "utf-16");
	mount_uid = getuid();
	mount_gid = getgid();
	mount_time = time(NULL);

	f->fs = open(dev, O_RDONLY);
	if (f->fs < 0)
		err(1, "open(%s)", dev);

	/* XXX add your code here */
  
  f->dev = dev;
  
  // Creates the boot sector.
  if ((f->boot_sector = malloc(sizeof(struct vfat_super))) == NULL) {
    printf("Memory allocation problem number 2.\n");
    return -1;
  }
  
  // Gets the boot sector from the FAT partition.
  pread(f->fs, f->boot_sector, sizeof(*f->boot_sector), 0);
  
  printf("\n***********Filesystem useful statistics: ***********\n");
  printf("\n");
  printf("\tbytes_per_sector : %d \n", f->boot_sector->bytes_per_sector);
  printf("\tsectors_per_cluster : %d \n", f->boot_sector->sectors_per_cluster);
  printf("\tsectors_per_fat : %d \n", f->boot_sector->sectors_per_fat);
  printf("\tnumber of clusters per fat : %d \n", f->boot_sector->sectors_per_fat / f->boot_sector->sectors_per_cluster);
  printf("\ttotal number of clusters : %d \n", 2 * (f->boot_sector->sectors_per_fat / f->boot_sector->sectors_per_cluster));
  printf("\treserved_sectors : %d \n", f->boot_sector->reserved_sectors);
  printf("\n****************************************************\n");
  
  // Then do the actual fetching for the root directory.
  printf("\n*********** Listing root directory: ***********\n");
  
  struct vfat_direntry record;
  unsigned char* firstByte = &record;
  
  /*
   *  Gives the address of the first file in the root directory (! In Logical Block Addressing, not in sectors nor bytes !).
   *  To access any particular cluster, you need to use this formula to turn the cluster number into the LBA address for the IDE    *  drive:
   *  
   *  lba_addr = cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
  */
  int cluster_begin_lba = f->boot_sector->reserved_sectors + f->boot_sector->sectors_per_fat * f->boot_sector->fat_count;
  
  int i = 7;
  do {
    
    // Gets the record from the FAT partition.
    pread(f->fs, &record, sizeof(record), (cluster_begin_lba * f->boot_sector->bytes_per_sector + (i * sizeof(record))));
    
    printf("\tname : %s \tcluster_hi : %d, cluster_lo : %d\n", record.nameext, record.cluster_hi, record.cluster_lo);
    
    // TEST
    int nextFATCluster = 0, test = 261, count = 0;
    while (nextFATCluster != 0xFFFFFFF && test != 0xFFFFFFF) {
      nextFATCluster = test;
      printf("\tsubfile : %X \n", test);
      pread(f->fs, &test, sizeof(test), f->boot_sector->reserved_sectors * f->boot_sector->bytes_per_sector + test * 4);
      count++;
    }
    
    printf("COUNT : %d\n", count);
    
    switch(record.attr) {
      case 0x01:
        printf("\t(Read only file) ");
        break;
      case 0x02:
        printf("\t(Hidden file) ");
        break;
      case VFAT_ATTR_DIR:
        printf("\t(FOLDER) ");
        break;
      case 0x20:
        printf("\t(ARCHIVE) ");
        break;
      case 0x0F:
        printf("\t(VFAT LONG FILENAME) ");
        break;
      default:
        printf("\tattr : 0x%X ", record.attr);
    }
    
    firstByte = &record;    
    if (*firstByte == 0x0) continue;
    switch(*firstByte) {
      case 0x2E:
        printf(" (DOT entry!)\n");
        break;
      case 0xE5:
        printf(" (DELETED!)\n");
        break;
      default:
        printf("\n");
    }
    
    i += 1;
    
  } while (*firstByte != 0x0);
  printf("\n**********************************************\n");
  
  // End of the program.
  printf("\nFreeing the memory...\n");
  // free(reservedSectors);
  // free(fileAllocationTables);
  free(f->boot_sector);
}

/* XXX add your code here */

static int vfat_readdir(/* XXX add your code here, */fuse_fill_dir_t filler, void *fillerdata) {
  
	struct stat st;
	void *buf = NULL;
	struct vfat_direntry *e;
	char *name;

	memset(&st, 0, sizeof(st));
	st.st_uid = mount_uid;
	st.st_gid = mount_gid;
	st.st_nlink = 1;

	/* XXX add your code here */
}

static int vfat_search_entry(void *data, const char *name, const struct stat *st, off_t offs) {
  
	struct vfat_search_data *sd = data;

	if (strcmp(sd->name, name) != 0)
		return (0);

	sd->found = 1;
	*sd->st = *st;

	return (1);
}

static int vfat_resolve(const char *path, struct stat *st) {
  
	struct vfat_search_data sd;
  
	/* XXX add your code here */
}

static int vfat_fuse_getattr(const char *path, struct stat *st) {
  
  // TODO : Get the file from the given path.
	int res = lstat(path, st); // Gets informations on the file at path *path and returns a stat structure in st.
  
  // From man 2 stat : 
  // On success, zero is returned. On error, -1 is returned, and errno is set appropriately.
  return (res == -1) ? -errno : st; // By Fuse convention, we should always return a negative value for the error.
}

static int vfat_fuse_readdir(const char *path, void *data, fuse_fill_dir_t filler, off_t offs, struct fuse_file_info *fi) {
	/* XXX add your code here */
}

static int vfat_fuse_read(const char *path, char *buf, size_t size, off_t offs, struct fuse_file_info *fi) {
	/* XXX add your code here */
}

static int vfat_opt_args(void *data, const char *arg, int key, struct fuse_args *oargs) {
	if (key == FUSE_OPT_KEY_NONOPT && !f->dev) {
		f->dev = strdup(arg);
		return (0);
	}
  
	return (1);
}

static struct fuse_operations vfat_ops = {
	.getattr = vfat_fuse_getattr,
	.readdir = vfat_fuse_readdir,
	.read = vfat_fuse_read,
};

int main(int argc, char **argv) {
  
  printf("Starting the program...\n");
  
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  fuse_opt_parse(&args, NULL, NULL, vfat_opt_args);
  
  if (!f->dev)
    errx(1, "missing file system parameter");
  
  printf("Init filesystem...\n");
  
  vfat_init(f->dev);
  
  printf("init has been performed.\n");
  
  fflush(stdout);
  
  // return (fuse_main(args.argc, args.argv, &vfat_ops, NULL));
}
