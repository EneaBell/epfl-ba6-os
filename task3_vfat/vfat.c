/* ---------------------------------------------------------------------------
//   vfat.c
// ---------------------------------------------------------------------------
//  Project:
//      Musso Claire    (205784)
//      Dieulivol David (185078)
//      Denoréaz Thomas (183785)
//
//  Versions:
//      01.05.2013 - 1.0 - Initial release
//      10.05.2013 - 1.1 - Init vfat
//      22.05.2013 - 1.2 - Read all
//
// ------------------------------------------------------------------------ */

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

#include <ctype.h>

// Changed to fit with the Microsoft FAT32 File System Specification
struct vfat_super {
	uint8_t		BS_jmpBoot[3];		// res1[3];
	char		BS_OEMName[8];		// oemname[8];
	uint16_t	BPB_BytsPerSec;		// bytes_per_sector;
	uint8_t		BPB_SecPerClus;		// sectors_per_cluster;
	uint16_t	BPB_RsvdSecCnt;		// reserved_sectors;
	uint8_t		BPB_NumFATs;		// fat_count;
	uint16_t	BPB_RootEntCnt;		// root_max_entries;
	uint16_t	BPB_TotSec16;		// total_sectors_small;
	uint8_t		BPB_Media;			// media_info;
	uint16_t	BPB_FATSz16;		// sectors_per_fat_small;
	uint16_t	BPB_SecPerTrk;		// sectors_per_track;
	uint16_t	BPB_NumHeads;		// head_count;
	uint32_t	BPB_HiddSec;		// fs_offset;
	uint32_t	BPB_TotSec32;		// total_sectors;
	uint32_t	BPB_FATSz32;		// sectors_per_fat;
	uint16_t	BPB_ExtFlags;		// fat_flags;
	uint16_t	BPB_FSVer;			// version;
	uint32_t	BPB_RootClus;		// root_cluster;
	uint16_t	BPB_FSInfo;			// fsinfo_sector;
	uint16_t	BPB_BkBootSec;		// backup_sector;
	uint8_t		BPB_Reserved[12];	// res2[12];
	uint8_t		BS_DrvNum;			// drive_number;
	uint8_t		BS_Reserved1;		// res3;
	uint8_t		BS_BootSig;			// ext_sig;
	uint32_t	BS_VolID;			// serial;
	char		BS_VolLab[11];		// label[11];
	char		BS_FilSysType[8];	// type[8];
	char		BS_Reserved2[420];	// res4[420];
	uint16_t	BS_Sig;				// sig;
} __attribute__ ((__packed__));

#define VFAT_SIGNATURE	0xAA55
#define VFAT_EOC		0x0FFFFFF8

// Changed to fit with the Microsoft FAT32 File System Specification
struct vfat_direntry {
	union {
		struct {
			char	DIR_Name[8];		// name[8];
			char	DIR_Ext[3];			// ext[3];
		};
		char		DIR_NameExt[11];	// nameext[11];
	};
	uint8_t			DIR_Attr;			// attr;
	uint8_t			DIR_NTRes;			// res;
	uint8_t			DIR_CrtTimeTenth;	// ctime_ms;
	uint16_t		DIR_CrtTime;		// ctime_time;
	uint16_t		DIR_CrtDate;		// ctime_date;
	uint16_t		DIR_LstAccDate;		// atime_date;
	uint16_t		DIR_FstClusHI;		// cluster_hi;
	uint16_t		DIR_WrtTime;		// mtime_time;
	uint16_t		DIR_WrtDate;		// mtime_date;
	uint16_t		DIR_FstClusLO;		// cluster_lo;
	uint32_t		DIR_FileSize;		// size;
} __attribute__ ((__packed__));
// sizeof(struct vfat_direntry) = 32bytes

#define VFAT_DIRNAME_NULL		0x00
#define VFAT_DIRNAME_FREE		0xE5
#define VFAT_DIRNAME_KANJI		0x05

#define VFAT_ATTR_READ_ONLY		0x01
#define VFAT_ATTR_HIDDEN		0x02
#define VFAT_ATTR_SYSTEM		0x04
#define VFAT_ATTR_VOLUME_ID		0x08
#define VFAT_ATTR_DIRECTORY		0x10
#define VFAT_ATTR_ARCHIVE		0x20

#define VFAT_ATTR_LFN			(VFAT_ATTR_READ_ONLY | VFAT_ATTR_HIDDEN | VFAT_ATTR_SYSTEM | VFAT_ATTR_VOLUME_ID)
#define VFAT_ATTR_LFN_MASK		(VFAT_ATTR_READ_ONLY | VFAT_ATTR_HIDDEN | VFAT_ATTR_SYSTEM | VFAT_ATTR_VOLUME_ID | VFAT_ATTR_DIRECTORY | VFAT_ATTR_ARCHIVE)

// Changed to fit with the Microsoft FAT32 File System Specification
struct vfat_direntry_lfn {
	uint8_t		LDIR_Ord;			// seq;
	uint16_t	LDIR_Name1[5];		// name1[5];
	uint8_t		LDIR_Attr;			// attr;
	uint8_t		LDIR_Type;			// res1;
	uint8_t		LDIR_Chksum;		// csum;
	uint16_t	LDIR_Name2[6];		// name2[6];
	uint16_t	LDIR_FstClusLO;		// res2;
	uint16_t	LDIR_Name3[2];		// name3[2];
} __attribute__ ((__packed__));
// sizeof(struct vfat_direntry_lfn) = 32bytes

#define VFAT_LFN_SEQ_START		0x40
#define VFAT_LFN_SEQ_DELETED	0x80
#define VFAT_LFN_SEQ_MASK		0x3F

struct vfat {
	const char	*dev;
	int			fs;
	
	/* XXX add your code here */
	
	// Contains the boot sector, as well as the FS Information Sector and the last reserved sectors.
	// it is the FAT header that contains useful informations about the partition.
	struct vfat_super* boot_sector;

	uint32_t RootDirSectors;
	uint32_t FATSz;
	uint32_t FirstDataSector;
	uint32_t TotSec;
	uint32_t DataSec;
	uint32_t CountOfClusters;

	// struct vfat_super* sectors[4];
	
  struct vfat_direntry currentDirentry;
  uint32_t currentClusterNumber;
  uint32_t nextClusterNumber;
};

#define ComputeFirstSectorOfCluster(N)	(f->FirstDataSector + ((N - 2) * f->boot_sector->BPB_SecPerClus))
#define FATOffset(N)					((f->boot_sector->BPB_TotSec16 != 0) ? (N << 1) : (N << 2))
#define ThisFATSecNum(N) 				(f->boot_sector->BPB_RsvdSecCnt + (FATOffset(N) / f->boot_sector->BPB_BytsPerSec))
#define ThisFATEntOffset(N) 			(FATOffset(N) % f->boot_sector->BPB_BytsPerSec)

#define IS_EOC(x)						(x >= VFAT_EOC)

struct vfat_search_data {
	const char	*name;
	int			found;
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
	mount_uid   = getuid();
	mount_gid   = getgid();
	mount_time  = time(NULL);

	f->fs = open(dev, O_RDONLY);
	if (f->fs < 0) {
		err(1, "open(%s)", dev);
	}

	/* XXX add your code here */
	
	f->dev = dev;
	
	// Creates the boot sector.
	if ((f->boot_sector = malloc(sizeof(struct vfat_super))) == NULL) {
		printf("Memory allocation problem number 2.\n");
		return;
	}
	
	// Gets the boot sector from the FAT partition.
	pread(f->fs, f->boot_sector, sizeof(*f->boot_sector), 0);

	struct vfat_super *bs = f->boot_sector;


	// --- FAT Data Structure ---

	f->RootDirSectors = ((bs->BPB_RootEntCnt * 32) + (bs->BPB_BytsPerSec - 1)) / bs->BPB_BytsPerSec;

	f->FATSz = (bs->BPB_FATSz16 != 0) ? (0x0000FFFF & bs->BPB_FATSz16) : bs->BPB_FATSz32;

	f->FirstDataSector = bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * f->FATSz) + f->RootDirSectors;

	// --- FAT Type Determination ---

	f->TotSec = (bs->BPB_TotSec16 != 0) ? (0xFFFF & bs->BPB_TotSec16) : bs->BPB_TotSec32;

	f->DataSec = f->TotSec - (bs->BPB_RsvdSecCnt + (bs->BPB_NumFATs * f->FATSz) + f->RootDirSectors);

	f->CountOfClusters = f->DataSec / bs->BPB_SecPerClus;

	if (f->CountOfClusters < 4085) {
		/* Volume is FAT12 */
		err(1, "Error: Disk in FAT12 not implemented yet...");
	} else if(f->CountOfClusters < 65525) {
		/* Volume is FAT16 */
	    err(1, "Error: Disk in FAT16 not implemented yet...");
	} else {
	    /* Volume is FAT32 */
	    printf("FileSystem is in FAT32... Welcome :o)\n");
	}

	printf("\n" "*********** Filesystem useful statistics ***********\n");
	printf("\n" "");
	printf("\t" "BS_jmpBoot[0..2]	 : 0x%02X%02X%02X \n", bs->BS_jmpBoot[0], bs->BS_jmpBoot[1], bs->BS_jmpBoot[2]);
	printf("\t" "BS_OEMName		 : %.8s \n", bs->BS_OEMName);
	printf("\t" "BPB_BytsPerSec		 : %d \n", bs->BPB_BytsPerSec);
	printf("\t" "BPB_SecPerClus		 : %d \n", bs->BPB_SecPerClus);
	printf("\t" "BPB_RsvdSecCnt		 : %d \n", bs->BPB_RsvdSecCnt);
	printf("\t" "BPB_NumFATs		 : %d \n", bs->BPB_NumFATs);
	printf("\t" "BPB_RootEntCnt		 : %d \n", bs->BPB_RootEntCnt);
	printf("\t" "BPB_TotSec16		 : %d \n", bs->BPB_TotSec16);
	printf("\t" "BPB_Media		 : %d \n", bs->BPB_Media);
	printf("\t" "BPB_FATSz16		 : %d \n", bs->BPB_FATSz16);
	printf("\t" "BPB_SecPerTrk		 : %d \n", bs->BPB_SecPerTrk);
	printf("\t" "BPB_NumHeads		 : %d \n", bs->BPB_NumHeads);
	printf("\t" "BPB_HiddSec		 : %d \n", bs->BPB_HiddSec);
	printf("\t" "BPB_TotSec32		 : %d \n", bs->BPB_TotSec32);
	printf("\t" "BPB_FATSz32		 : %d \n", bs->BPB_FATSz32);
	printf("\t" "BPB_ExtFlags		 : %d \n", bs->BPB_ExtFlags);
	printf("\t" "BPB_FSVer		 : 0x%04X \n", bs->BPB_FSVer);
	printf("\t" "BPB_RootClus		 : 0x%08X \n", bs->BPB_RootClus);
	printf("\t" "BPB_FSInfo		 : 0x%04X \n", bs->BPB_FSInfo);
	printf("\t" "BPB_BkBootSec		 : 0x%04X \n", bs->BPB_BkBootSec);

	printf("\t" "BS_DrvNum		 : 0x%02X \n", bs->BS_DrvNum);

	printf("\t" "BS_BootSig		 : 0x%02X \n", bs->BS_BootSig);
	printf("\t" "BS_VolID		 : 0x%08X \n", bs->BS_VolID);
	printf("\t" "BS_VolLab		 : %.10s \n", bs->BS_VolLab);
	printf("\t" "BS_FilSysType		 : %.8s \n", bs->BS_FilSysType);

	printf("\t" "BS_Sig			 : 0x%04X \n", bs->BS_Sig);
	printf("\n");
	
	printf("\t" "number of clusters per fat : %d \n", bs->BPB_FATSz32 / bs->BPB_SecPerClus);
	printf("\t" "total number of clusters : %d \n", bs->BPB_NumFATs * (bs->BPB_FATSz32 / bs->BPB_SecPerClus));
	printf("\n");

	printf("\t" "RootDirSectors : 0x%08X \n", f->RootDirSectors);
	printf("\t" "FATSz : %d \n", f->FATSz);
	printf("\t" "FirstDataSector : 0x%08X \n", f->FirstDataSector);
	printf("\n");
	
	printf("\t" "TotSec : %d \n", f->TotSec);
	printf("\t" "DataSec : %d \n", f->DataSec);
	printf("\t" "CountOfClusters : %d \n", f->CountOfClusters);

	printf("\n" "****************************************************\n");

	// Checking FAT signature
	if (bs->BS_Sig != VFAT_SIGNATURE) {
		err(1, "Error in FAT signature, 0x%X instead of 0x%X", bs->BS_Sig, VFAT_SIGNATURE);
	}

	/*
	int i;
	for (i = 2; i < 10; ++i) {
		printf("\t" "ComputeFirstSectorOfCluster(%d) -> 0x%08X\n", i, ComputeFirstSectorOfCluster(i));
		printf("\t" "ThisFATSecNum(%d) -> 0x%08X\n", i, ThisFATSecNum(i));
		printf("\t" "ThisFATEntOffset(%d) -> 0x%08X\n", i, ThisFATEntOffset(i));
	}
	*/
}

/* XXX add your code here */

// http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
size_t trimwhitespace(char *out, size_t len, const char *str)
{
	if(len == 0) return 0;

	const char *end;
	size_t out_size;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0) { // All spaces?
		*out = 0;
		return 1;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	// Copy trimmed string and add null terminator
	memcpy(out, str, out_size);
	out[out_size] = 0;

	return out_size;
}


void split_path_file(char** p, char** f, char *pf) {
    char *slash = pf, *next;
    while ((next = strpbrk(slash + 1, "\\/"))) slash = next;
    if (pf != slash) slash++;
    *p = strndup(pf, slash - pf);
    *f = strdup(slash);
}

// http://stackoverflow.com/questions/1575278/function-to-split-a-filepath-into-path-and-file
void split_path_file_old(char** p, char** f, char *pf) {
    char *slash = pf, *next;
    while ((next = strpbrk(slash + 1, "\\/"))) slash = next;
    if (pf != slash) slash++;
    *p = strndup(pf, slash - pf);
    *f = strdup(slash);
}

// http://en.wikipedia.org/wiki/File_Allocation_Table#VFAT_long_file_names
uint8_t lfn_checksum(const unsigned char *pFCBName) {
	int i;
	uint8_t sum = 0;
 
	for (i = 11; i; i--) {
		sum = ((sum & 1) << 7) + (sum >> 1) + *pFCBName++;
	}

	return sum;
}

static int vfat_readdir(void *fillerdata, fuse_fill_dir_t filler /* XXX add your code here, */) {
	
	struct stat st;
	void *buf = NULL;
	struct vfat_direntry *e;
	char *name;

	memset(&st, 0, sizeof(st));
	st.st_uid = mount_uid;
	st.st_gid = mount_gid;
	st.st_nlink = 1;

	printf("vfat_readdir\n");

	/* XXX add your code here */

	return 1;
}

static int vfat_search_entry(void *data, const char *name, const struct stat *st, off_t offs) {
	
	struct vfat_search_data *sd = data;

	if (strcmp(sd->name, name) != 0)
		return 0;

	sd->found = 1;
	*sd->st = *st;

	return 1;
}

static int vfat_resolve(const char *path, struct stat *st) {
	
	struct vfat_search_data sd;
	
	/* XXX add your code here */

	/*
	if (vfat_search_entry(sd, "abc", st, 0)) {

	}
	*/

	return 1;
}

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

/*
 * Get file attributes.
 *
 * Similar to stat(). The 'st_dev' and 'st_blksize' fields are ignored.
 * The 'st_ino' field is ignored except if the 'use_ino' mount option is given.
 */
static int vfat_fuse_getattr(const char *path, struct stat *st) {

	printf("vfat_fuse_getattr(path -> %s)\n", path);

	int res = 0;

	memset(st, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2;

	} else if (strcmp(path, hello_path) == 0) {
		st->st_mode = S_IFREG | 0444;
		st->st_nlink = 1;
		st->st_size = strlen(hello_str);

	} else {
		res = -ENOENT;
	}

	return res;
}

/*
 * Read directory
 * 
 * This supersedes the old getdir() interface. New applications should use this.
 * The filesystem may choose between two modes of operation:
 * 1) The readdir implementation ignores the offset parameter, and passes zero to the filler function's offset.
 * The filler function will not return '1' (unless an error happens), so the whole directory is read in a single
 * readdir operation. This works just like the old getdir() method.
 * 
 * 2) The readdir implementation keeps track of the offsets of the directory entries. It uses the offset parameter
 * and always passes non-zero offset to the filler function. When the buffer is full (or an error happens) the filler
 * function will return '1'.
 * 
 * Introduced in version 2.3
*/
static int vfat_fuse_readdir(const char *path, void *data, fuse_fill_dir_t filler, off_t offs, struct fuse_file_info *fi) {

	printf("vfat_fuse_readdir(path -> %s)\n", path);

	(void) offs;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(data, ".", NULL, 0);
	filler(data, "..", NULL, 0);
	filler(data, hello_path + 1, NULL, 0);

	return 0;
	
	/* XXX add your code here */

	//vfat_readdir(data, filler)

	//return 0;
}


static int vfat_fuse_open(const char *path, struct fuse_file_info *fi) {
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int get_next_cluster_number() {
  
  f->currentClusterNumber = f->nextClusterNumber;
  
	if (f->currentClusterNumber > 0 && !IS_EOC(f->currentClusterNumber)) {
		int clusterOffset = (f->boot_sector->BPB_RsvdSecCnt * f->boot_sector->BPB_BytsPerSec) + (f->currentClusterNumber * sizeof(uint32_t));
		int sectorOffset = f->boot_sector->BPB_BytsPerSec * ComputeFirstSectorOfCluster(f->currentClusterNumber);
		pread(f->fs, &f->nextClusterNumber, sizeof(uint32_t), clusterOffset);
		f->nextClusterNumber &= 0x0FFFFFFF;
	} else {
    f->currentClusterNumber = -1;
    f->nextClusterNumber = -1;
    return -1;
  }
  
  return 0;
}

/*
 * Read data from an open file
 *
 * Read should return exactly the number of bytes requested except on EOF or error, otherwise the rest of the data
 * will be substituted with zeroes. An exception to this is when the 'direct_io' mount option is specified, in which
 * case the return value of the read system call will reflect the return value of this operation.
 * 
 * Changed in version 2.2
 *
 *
 * struct fuse_file_info {
 *    int 	           flags
 *    unsigned long    fh_old
 *    int 	           writepage
 *    unsigned int     direct_io: 1
 *    unsigned int     keep_cache: 1
 *    unsigned int     flush: 1
 *    unsigned int     nonseekable: 1
 *    unsigned int     padding: 27
 *    uint64_t 	       fh
 *    uint64_t 	       lock_owner
 * }
 * 
 */
static int vfat_fuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  
	/* XXX add your code here */
  
  if (offset >= size) return -1;                          // If the offset is bigger than the size, we quit.

  /********* TEST : Instead of using resolve **********/
  uint32_t direntry_offset = 0x000FDC40;
  pread(f->fs, &f->currentDirentry, sizeof(f->currentDirentry), direntry_offset);
  f->nextClusterNumber = ((0xFFFF & f->currentDirentry.DIR_FstClusHI) << 16) | (0xFFFF & f->currentDirentry.DIR_FstClusLO);
    f->currentClusterNumber = f->nextClusterNumber;
  /********************** END *************************/
  
  int bytes_per_sector = f->boot_sector->BPB_BytsPerSec * f->boot_sector->BPB_SecPerClus;
  
  int to_read, last_round = 0;
  int current_offset = 0;
  int total_bytes_read = size;
  
  int buffer_offset = 0;
  uint32_t dataStart;
  while (!last_round && get_next_cluster_number() != -1) {
    
    dataStart = f->boot_sector->BPB_BytsPerSec * ComputeFirstSectorOfCluster(f->currentClusterNumber);
    
    // We can read everything we have to in this sector.
    if (size + offset <= bytes_per_sector) {
      to_read             = size - offset;
      current_offset      = offset;
      if (size + offset < bytes_per_sector) last_round = 1;
    } else {
      if (offset >= bytes_per_sector) {                     // If offset is bigger than 4096, we continue to the next block.
        offset           -= bytes_per_sector;
        continue;
      } else {
        if (size > bytes_per_sector) {
          to_read         = bytes_per_sector - offset;
          current_offset  = offset;
          size           -= bytes_per_sector;
          offset          = 0;
        } else {
          to_read         = bytes_per_sector - offset;
          current_offset  = offset;
          size           -= to_read;
          offset          = 0;
        }
      }
    }
    
    // TODO : Need to concatenate all the buffers into one big buffer!
    //        Now we only have the last one to display!
    pread(f->fs, buf, to_read, dataStart + current_offset);
    printf("%s", buf);
  }

	return total_bytes_read;
}

static int vfat_opt_args(void *data, const char *arg, int key, struct fuse_args *oargs) {
	
	if (key == FUSE_OPT_KEY_NONOPT && !f->dev) {
		f->dev = strdup(arg);
		return 0;
	}
	
	return 1;
}

static struct fuse_operations vfat_ops = {
	.getattr 	= vfat_fuse_getattr,
	.readdir 	= vfat_fuse_readdir,
	.read 		= vfat_fuse_read,
	.open 		= vfat_fuse_open
};










static void vfat_test_read_all() {

	struct vfat_super *bs = f->boot_sector;

	// Then do the actual fetching for the root directory.
	printf("\n*********** Listing root directory ***********\n");
	
	struct vfat_direntry record;
	unsigned char* firstByte = (unsigned char*) &record;
	
	/*
	 * Gives the address of the first file in the root directory (! In Logical Block Addressing, not in sectors nor bytes !).
	 * To access any particular cluster, you need to use this formula to turn the cluster number into the LBA address for the IDE *drive:
	 *
	 * lba_addr = cluster_begin_lba + (cluster_number - 2) * sectors_per_cluster;
	 */

	uint32_t cluster_begin_lba = ComputeFirstSectorOfCluster(bs->BPB_RootClus);

	printf("cluster_begin_lba -> %08X\n", cluster_begin_lba);

	int i = 0;
	do {

		// Gets the record from the FAT partition.

		//uint32_t direntry_offset = (cluster_begin_lba * bs->BPB_BytsPerSec + (i * sizeof(record)));
		uint32_t direntry_offset = (cluster_begin_lba * bs->BPB_BytsPerSec + (i * sizeof(record)));

		printf("\tdirentry_offset -> %08X\n", direntry_offset);

		pread(f->fs, &record, sizeof(record), direntry_offset);

		uint8_t isFree = 0;

		firstByte = (unsigned char*) &record;
		printf("\t");
		switch(*firstByte) {
			case VFAT_DIRNAME_NULL:
				continue;

			// WTF !?
			case 0x2E:
				printf("(DOT entry!) ");
				break;

			case VFAT_DIRNAME_FREE:
				printf("(Free) ");
				isFree = 1;
				break;

			// Kanji !?!?
		}

		switch(record.DIR_Attr) {
			case VFAT_ATTR_READ_ONLY:
				printf("(Read Only) ");
				break;
			case VFAT_ATTR_HIDDEN:
				printf("(Hidden) ");
				break;
			case VFAT_ATTR_SYSTEM:
				printf("(System) ");
				break;
			case VFAT_ATTR_VOLUME_ID:
				printf("(VolumeID) ");
				break;
			case VFAT_ATTR_DIRECTORY:
				printf("(Folder) ");
				break;
			case VFAT_ATTR_ARCHIVE:
				printf("(Archive) ");
				break;
			case VFAT_ATTR_LFN:
				printf("(VFAT Long Name)\n");
				i++;
				continue;
			default:
				printf("\t(0x%X)", record.DIR_Attr);
		}
		printf("\n");

		printf("\tname : %.8s.%.3s \n", record.DIR_Name, record.DIR_Ext);

		// TODO Check that formula
		uint32_t cluster = ((0xFFFF & record.DIR_FstClusHI) << 16) | (0xFFFF & record.DIR_FstClusLO);
		printf("\tcluster entry : 0x%08X\n", cluster);

		// if (!isFree) {
      // if (cluster > 0) {
				printf("\tclusters :\n");
				int count = 0;
				while (!IS_EOC(cluster)) {
					int clusterOffset = (bs->BPB_RsvdSecCnt * bs->BPB_BytsPerSec) + (cluster * sizeof(uint32_t));
					int sectorOffset = bs->BPB_BytsPerSec * ComputeFirstSectorOfCluster(cluster);
					printf("\t\t" "0x%08X\toffset : 0x%08X -> 0x%08X\n", cluster, clusterOffset, sectorOffset);
					pread(f->fs, &cluster, sizeof(uint32_t), clusterOffset);
					cluster &= 0x0FFFFFFF;
					count++;
				}

				printf("\tcount : %d\n", count);
      // } else {
      //   printf("\tcount : Empty file\n");
      // }
		
		printf("\t-----------\n");
		
		i += 1;
		
	} while (*firstByte != 0x0);
	printf("\n**********************************************\n");
}














int main(int argc, char **argv) {

	printf("Starting the program...\n");
	
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	fuse_opt_parse(&args, NULL, NULL, vfat_opt_args);
	
	if (!f->dev) {
		errx(1, "missing file system parameter");
	}
	
	printf("Init filesystem...\n");
	
	vfat_init(f->dev);

	vfat_test_read_all();
	
	printf("init has been performed.\n");
	
	fflush(stdout);
	
	return fuse_main(args.argc, args.argv, &vfat_ops, NULL);
}

/* ---------------------------------------------------------------------------
//   End of File
// ------------------------------------------------------------------------ */