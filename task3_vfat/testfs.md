# testfs.fat

## Sectors
Boot Sector
FSInfo Sector
Sector 1 -> Empty
Sector 2 -> Empty
Sector 3 -> Empty
Sector 4 -> Empty
Backup

## Filesystem useful statistics:
	- bytes_per_sector				: 512
	- sectors_per_cluster 			: 8
	- sectors_per_fat 				: 999
	- number of clusters per fat 	: 124
	- total number of clusters 		: 248
	- reserved_sectors 				: 32
	- signature 					: 0xAA55

## Files & Directories

### 3THE (Archive)
- DirEntry 		->	0x000FDCE0 (=1039584)
- Cluster0		->	0x00000105 (=261)
	- Offset	->	0x00004414 (=17428)
	- Data		->	0x00200C00 (=2100224)


### HI (Archive)
- DirEntry 		->	0x000FDF60 (=1040224)
- Cluster0		->	0x0001F2DB (=127707)
	- Offset	->	0x00080B6C (=527212)
	- Data		->	0x1F3D6C01 (=524119041)


### BIGDIR (Folder)
- DirEntry 	->	0x000FDE20 (=1039904)

- Cluster0		->	0x00000113
	- Offset	->	0x0000444C
- Cluster1		->	0x00000153
	- Offset	->	0x0000454C
- Cluster2		->	0x00000194
	- Offset	->	0x00004650
- Cluster3		->	0x000001D5
	- Offset	->	0x00004754
- Cluster4		->	0x00000216
	- Offset	->	0x00004858
- Cluster5		->	0x00000257
	- Offset	->	0x0000495C
- Cluster6		->	0x00000298
	- Offset	->	0x00004A60
- Cluster7		->	0x000002D9
	- Offset	->	0x00004B64







