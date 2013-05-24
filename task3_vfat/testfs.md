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
	BS_jmpBoot[0..2]	 : 0xEB5890 
	BS_OEMName			 : mkdosfs 
	BPB_BytsPerSec		 : 512
	BPB_SecPerClus		 : 8
	BPB_RsvdSecCnt		 : 32
	BPB_NumFATs			 : 2 
	BPB_RootEntCnt		 : 0 
	BPB_TotSec16		 : 0 
	BPB_Media			 : 248 
	BPB_FATSz16			 : 0 
	BPB_SecPerTrk		 : 32 
	BPB_NumHeads		 : 64 
	BPB_HiddSec			 : 0 
	BPB_TotSec32		 : 1024000 
	BPB_FATSz32			 : 999 
	BPB_ExtFlags		 : 0 
	BPB_FSVer			 : 0x0000 
	BPB_RootClus		 : 0x00000002 
	BPB_FSInfo			 : 0x0001 
	BPB_BkBootSec		 : 0x0006 
	BS_DrvNum			 : 0x00 
	BS_BootSig			 : 0x29 
	BS_VolID			 : 0xAF54F70A 
	BS_VolLab			 : NO_FILEnam 
	BS_FilSysType		 : FAT32    
	BS_Sig				 : 0xAA55 

	# clusters per fat 	 : 124
	total # of clusters  : 248

	RootDirSectors 		 : 0x00000000
	FATSz 				 : 999
	FirstDataSector 	 : 0x000007EE

	TotSec 				 : 0x000FA000 (=1024000)
	DataSec 			 : 0x000F9812 (=1021970)
	CountOfClusters 	 : 0x0001F302 (=127746)

### HelloWorld Test
thmx@tsf-452-wpa-4-073 task3_vfat $ ls dest/
hello
thmx@tsf-452-wpa-4-073 task3_vfat $ ls -la dest/
total 8
drwxr-xr-x   2 root  wheel    0 Jan  1  1970 .
drwxr-xr-x  17 thmx  admin  578 May 24 18:13 ..
-r--r--r--   1 root  wheel   13 Jan  1  1970 hello
thmx@tsf-452-wpa-4-073 task3_vfat $ cat dest/hello 
Hello World!

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








### Listing root directory

cluster_begin_lba -> 000007EE
	direntry_offset -> 000FDC00
	(VolumeID) 
	name : NO_FILEn.ame 
	cluster entry : 0x00000000
	count : Empty file
	-----------
	direntry_offset -> 000FDC20
	(Archive) 
	name : LARGE1  .    
	cluster entry : 0x0000030F
	clusters :
		0x0000030F	offset : 0x00004C3C -> 0x0040AC01
		0x00000311	offset : 0x00004C44 -> 0x0040CC01
		0x00000313	offset : 0x00004C4C -> 0x0040EC01
		0x00000314	offset : 0x00004C50 -> 0x0040FC01
		0x00000317	offset : 0x00004C5C -> 0x00412C01
		0x00000318	offset : 0x00004C60 -> 0x00413C01
		0x00000319	offset : 0x00004C64 -> 0x00414C01
		0x0000031D	offset : 0x00004C74 -> 0x00418C01
		0x0000031E	offset : 0x00004C78 -> 0x00419C01
		0x0000031F	offset : 0x00004C7C -> 0x0041AC01
		0x00000320	offset : 0x00004C80 -> 0x0041BC01
		0x00000321	offset : 0x00004C84 -> 0x0041CC01
		0x00000322	offset : 0x00004C88 -> 0x0041DC01
		0x00000323	offset : 0x00004C8C -> 0x0041EC01
		0x0000032B	offset : 0x00004CAC -> 0x00426C01
		0x0000032C	offset : 0x00004CB0 -> 0x00427C01
		0x0000032D	offset : 0x00004CB4 -> 0x00428C01
		0x0000032E	offset : 0x00004CB8 -> 0x00429C01
		0x0000032F	offset : 0x00004CBC -> 0x0042AC01
		0x00000330	offset : 0x00004CC0 -> 0x0042BC01
		0x00000331	offset : 0x00004CC4 -> 0x0042CC01
		0x00000332	offset : 0x00004CC8 -> 0x0042DC01
		0x00000333	offset : 0x00004CCC -> 0x0042EC01
		0x00000334	offset : 0x00004CD0 -> 0x0042FC01
		0x00000335	offset : 0x00004CD4 -> 0x00430C01
		0x00000336	offset : 0x00004CD8 -> 0x00431C01
		0x00000337	offset : 0x00004CDC -> 0x00432C01
		0x00000338	offset : 0x00004CE0 -> 0x00433C01
		0x00000347	offset : 0x00004D1C -> 0x00442C01
		0x00000348	offset : 0x00004D20 -> 0x00443C01
		0x00000349	offset : 0x00004D24 -> 0x00444C01
		0x0000034A	offset : 0x00004D28 -> 0x00445C01
		0x0000034B	offset : 0x00004D2C -> 0x00446C01
		0x0000034C	offset : 0x00004D30 -> 0x00447C01
		0x0000034D	offset : 0x00004D34 -> 0x00448C01
		0x0000034E	offset : 0x00004D38 -> 0x00449C01
		0x0000034F	offset : 0x00004D3C -> 0x0044AC01
		0x00000350	offset : 0x00004D40 -> 0x0044BC01
		0x00000351	offset : 0x00004D44 -> 0x0044CC01
		0x00000352	offset : 0x00004D48 -> 0x0044DC01
		0x00000353	offset : 0x00004D4C -> 0x0044EC01
		0x00000354	offset : 0x00004D50 -> 0x0044FC01
		0x00000355	offset : 0x00004D54 -> 0x00450C01
		0x00000356	offset : 0x00004D58 -> 0x00451C01
		0x00000357	offset : 0x00004D5C -> 0x00452C01
		0x00000358	offset : 0x00004D60 -> 0x00453C01
		0x00000359	offset : 0x00004D64 -> 0x00454C01
		0x0000035A	offset : 0x00004D68 -> 0x00455C01
		0x0000035B	offset : 0x00004D6C -> 0x00456C01
		0x0000035C	offset : 0x00004D70 -> 0x00457C01
		0x0000035D	offset : 0x00004D74 -> 0x00458C01
		0x0000035E	offset : 0x00004D78 -> 0x00459C01
		0x0000035F	offset : 0x00004D7C -> 0x0045AC01
		0x00000360	offset : 0x00004D80 -> 0x0045BC01
		0x00000361	offset : 0x00004D84 -> 0x0045CC01
		0x00000362	offset : 0x00004D88 -> 0x0045DC01
		0x0000037F	offset : 0x00004DFC -> 0x0047AC01
		0x00000380	offset : 0x00004E00 -> 0x0047BC01
		0x00000381	offset : 0x00004E04 -> 0x0047CC01
		0x00000382	offset : 0x00004E08 -> 0x0047DC01
		0x00000383	offset : 0x00004E0C -> 0x0047EC01
		0x00000384	offset : 0x00004E10 -> 0x0047FC01
		0x00000385	offset : 0x00004E14 -> 0x00480C01
		0x00000386	offset : 0x00004E18 -> 0x00481C01
		0x00000387	offset : 0x00004E1C -> 0x00482C01
		0x00000388	offset : 0x00004E20 -> 0x00483C01
		0x00000389	offset : 0x00004E24 -> 0x00484C01
		0x0000038A	offset : 0x00004E28 -> 0x00485C01
		0x0000038B	offset : 0x00004E2C -> 0x00486C01
		0x0000038C	offset : 0x00004E30 -> 0x00487C01
		0x0000038D	offset : 0x00004E34 -> 0x00488C01
		0x0000038E	offset : 0x00004E38 -> 0x00489C01
		0x0000038F	offset : 0x00004E3C -> 0x0048AC01
		0x00000390	offset : 0x00004E40 -> 0x0048BC01
		0x00000391	offset : 0x00004E44 -> 0x0048CC01
		0x00000392	offset : 0x00004E48 -> 0x0048DC01
		0x00000393	offset : 0x00004E4C -> 0x0048EC01
		0x00000394	offset : 0x00004E50 -> 0x0048FC01
		0x00000395	offset : 0x00004E54 -> 0x00490C01
		0x00000396	offset : 0x00004E58 -> 0x00491C01
		0x00000397	offset : 0x00004E5C -> 0x00492C01
		0x00000398	offset : 0x00004E60 -> 0x00493C01
		0x00000399	offset : 0x00004E64 -> 0x00494C01
		0x0000039A	offset : 0x00004E68 -> 0x00495C01
		0x0000039B	offset : 0x00004E6C -> 0x00496C01
		0x0000039C	offset : 0x00004E70 -> 0x00497C01
		0x0000039D	offset : 0x00004E74 -> 0x00498C01
		0x0000039E	offset : 0x00004E78 -> 0x00499C01
		0x0000039F	offset : 0x00004E7C -> 0x0049AC01
		0x000003A0	offset : 0x00004E80 -> 0x0049BC01
		0x000003A1	offset : 0x00004E84 -> 0x0049CC01
		0x000003A2	offset : 0x00004E88 -> 0x0049DC01
		0x000003A3	offset : 0x00004E8C -> 0x0049EC01
		0x000003A4	offset : 0x00004E90 -> 0x0049FC01
		0x000003A5	offset : 0x00004E94 -> 0x004A0C01
		0x000003A6	offset : 0x00004E98 -> 0x004A1C01
		0x000003A7	offset : 0x00004E9C -> 0x004A2C01
		0x000003A8	offset : 0x00004EA0 -> 0x004A3C01
		0x000003A9	offset : 0x00004EA4 -> 0x004A4C01
		0x000003AA	offset : 0x00004EA8 -> 0x004A5C01
		0x000003AB	offset : 0x00004EAC -> 0x004A6C01
		0x000003AC	offset : 0x00004EB0 -> 0x004A7C01
		0x000003AD	offset : 0x00004EB4 -> 0x004A8C01
		0x000003AE	offset : 0x00004EB8 -> 0x004A9C01
		0x000003AF	offset : 0x00004EBC -> 0x004AAC01
		0x000003B0	offset : 0x00004EC0 -> 0x004ABC01
		0x000003B1	offset : 0x00004EC4 -> 0x004ACC01
		0x000003B2	offset : 0x00004EC8 -> 0x004ADC01
		0x000003B3	offset : 0x00004ECC -> 0x004AEC01
		0x000003B4	offset : 0x00004ED0 -> 0x004AFC01
		0x000003B5	offset : 0x00004ED4 -> 0x004B0C01
		0x000003B6	offset : 0x00004ED8 -> 0x004B1C01
		0x000000EF	offset : 0x000043BC -> 0x001EAC01
	count : 113
	-----------
	direntry_offset -> 000FDC40
	(Archive) 
	name : LARGE2  .    
	cluster entry : 0x00000310
	clusters :
		0x00000310	offset : 0x00004C40 -> 0x0040BC01
		0x00000312	offset : 0x00004C48 -> 0x0040DC01
		0x00000315	offset : 0x00004C54 -> 0x00410C01
		0x00000316	offset : 0x00004C58 -> 0x00411C01
		0x0000031A	offset : 0x00004C68 -> 0x00415C01
		0x0000031B	offset : 0x00004C6C -> 0x00416C01
		0x0000031C	offset : 0x00004C70 -> 0x00417C01
		0x00000324	offset : 0x00004C90 -> 0x0041FC01
		0x00000325	offset : 0x00004C94 -> 0x00420C01
		0x00000326	offset : 0x00004C98 -> 0x00421C01
		0x00000327	offset : 0x00004C9C -> 0x00422C01
		0x00000328	offset : 0x00004CA0 -> 0x00423C01
		0x00000329	offset : 0x00004CA4 -> 0x00424C01
		0x0000032A	offset : 0x00004CA8 -> 0x00425C01
		0x00000339	offset : 0x00004CE4 -> 0x00434C01
		0x0000033A	offset : 0x00004CE8 -> 0x00435C01
		0x0000033B	offset : 0x00004CEC -> 0x00436C01
		0x0000033C	offset : 0x00004CF0 -> 0x00437C01
		0x0000033D	offset : 0x00004CF4 -> 0x00438C01
		0x0000033E	offset : 0x00004CF8 -> 0x00439C01
		0x0000033F	offset : 0x00004CFC -> 0x0043AC01
		0x00000340	offset : 0x00004D00 -> 0x0043BC01
		0x00000341	offset : 0x00004D04 -> 0x0043CC01
		0x00000342	offset : 0x00004D08 -> 0x0043DC01
		0x00000343	offset : 0x00004D0C -> 0x0043EC01
		0x00000344	offset : 0x00004D10 -> 0x0043FC01
		0x00000345	offset : 0x00004D14 -> 0x00440C01
		0x00000346	offset : 0x00004D18 -> 0x00441C01
		0x00000363	offset : 0x00004D8C -> 0x0045EC01
		0x00000364	offset : 0x00004D90 -> 0x0045FC01
		0x00000365	offset : 0x00004D94 -> 0x00460C01
		0x00000366	offset : 0x00004D98 -> 0x00461C01
		0x00000367	offset : 0x00004D9C -> 0x00462C01
		0x00000368	offset : 0x00004DA0 -> 0x00463C01
		0x00000369	offset : 0x00004DA4 -> 0x00464C01
		0x0000036A	offset : 0x00004DA8 -> 0x00465C01
		0x0000036B	offset : 0x00004DAC -> 0x00466C01
		0x0000036C	offset : 0x00004DB0 -> 0x00467C01
		0x0000036D	offset : 0x00004DB4 -> 0x00468C01
		0x0000036E	offset : 0x00004DB8 -> 0x00469C01
		0x0000036F	offset : 0x00004DBC -> 0x0046AC01
		0x00000370	offset : 0x00004DC0 -> 0x0046BC01
		0x00000371	offset : 0x00004DC4 -> 0x0046CC01
		0x00000372	offset : 0x00004DC8 -> 0x0046DC01
		0x00000373	offset : 0x00004DCC -> 0x0046EC01
		0x00000374	offset : 0x00004DD0 -> 0x0046FC01
		0x00000375	offset : 0x00004DD4 -> 0x00470C01
		0x00000376	offset : 0x00004DD8 -> 0x00471C01
		0x00000377	offset : 0x00004DDC -> 0x00472C01
		0x00000378	offset : 0x00004DE0 -> 0x00473C01
		0x00000379	offset : 0x00004DE4 -> 0x00474C01
		0x0000037A	offset : 0x00004DE8 -> 0x00475C01
		0x0000037B	offset : 0x00004DEC -> 0x00476C01
		0x0000037C	offset : 0x00004DF0 -> 0x00477C01
		0x0000037D	offset : 0x00004DF4 -> 0x00478C01
		0x0000037E	offset : 0x00004DF8 -> 0x00479C01
		0x000003B7	offset : 0x00004EDC -> 0x004B2C01
		0x000003B8	offset : 0x00004EE0 -> 0x004B3C01
		0x000003B9	offset : 0x00004EE4 -> 0x004B4C01
		0x000003BA	offset : 0x00004EE8 -> 0x004B5C01
		0x000003BB	offset : 0x00004EEC -> 0x004B6C01
		0x000003BC	offset : 0x00004EF0 -> 0x004B7C01
		0x000003BD	offset : 0x00004EF4 -> 0x004B8C01
		0x000003BE	offset : 0x00004EF8 -> 0x004B9C01
		0x000003BF	offset : 0x00004EFC -> 0x004BAC01
		0x000003C0	offset : 0x00004F00 -> 0x004BBC01
		0x000003C1	offset : 0x00004F04 -> 0x004BCC01
		0x000003C2	offset : 0x00004F08 -> 0x004BDC01
		0x000003C3	offset : 0x00004F0C -> 0x004BEC01
		0x000003C4	offset : 0x00004F10 -> 0x004BFC01
		0x000003C5	offset : 0x00004F14 -> 0x004C0C01
		0x000003C6	offset : 0x00004F18 -> 0x004C1C01
		0x000003C7	offset : 0x00004F1C -> 0x004C2C01
		0x000003C8	offset : 0x00004F20 -> 0x004C3C01
		0x000003C9	offset : 0x00004F24 -> 0x004C4C01
		0x000003CA	offset : 0x00004F28 -> 0x004C5C01
		0x000003CB	offset : 0x00004F2C -> 0x004C6C01
		0x000003CC	offset : 0x00004F30 -> 0x004C7C01
		0x000003CD	offset : 0x00004F34 -> 0x004C8C01
		0x000003CE	offset : 0x00004F38 -> 0x004C9C01
		0x000003CF	offset : 0x00004F3C -> 0x004CAC01
		0x000003D0	offset : 0x00004F40 -> 0x004CBC01
		0x000003D1	offset : 0x00004F44 -> 0x004CCC01
		0x000003D2	offset : 0x00004F48 -> 0x004CDC01
		0x000003D3	offset : 0x00004F4C -> 0x004CEC01
		0x000003D4	offset : 0x00004F50 -> 0x004CFC01
		0x000003D5	offset : 0x00004F54 -> 0x004D0C01
		0x000003D6	offset : 0x00004F58 -> 0x004D1C01
		0x000003D7	offset : 0x00004F5C -> 0x004D2C01
		0x000003D8	offset : 0x00004F60 -> 0x004D3C01
		0x000003D9	offset : 0x00004F64 -> 0x004D4C01
		0x000003DA	offset : 0x00004F68 -> 0x004D5C01
		0x000003DB	offset : 0x00004F6C -> 0x004D6C01
		0x000003DC	offset : 0x00004F70 -> 0x004D7C01
		0x000003DD	offset : 0x00004F74 -> 0x004D8C01
		0x000003DE	offset : 0x00004F78 -> 0x004D9C01
		0x000003DF	offset : 0x00004F7C -> 0x004DAC01
		0x000003E0	offset : 0x00004F80 -> 0x004DBC01
		0x000003E1	offset : 0x00004F84 -> 0x004DCC01
		0x000003E2	offset : 0x00004F88 -> 0x004DDC01
		0x000003E3	offset : 0x00004F8C -> 0x004DEC01
		0x000003E4	offset : 0x00004F90 -> 0x004DFC01
		0x000003E5	offset : 0x00004F94 -> 0x004E0C01
		0x000003E6	offset : 0x00004F98 -> 0x004E1C01
		0x000003E7	offset : 0x00004F9C -> 0x004E2C01
		0x000003E8	offset : 0x00004FA0 -> 0x004E3C01
		0x000003E9	offset : 0x00004FA4 -> 0x004E4C01
		0x000003EA	offset : 0x00004FA8 -> 0x004E5C01
		0x000003EB	offset : 0x00004FAC -> 0x004E6C01
		0x000003EC	offset : 0x00004FB0 -> 0x004E7C01
		0x000003ED	offset : 0x00004FB4 -> 0x004E8C01
		0x000003EE	offset : 0x00004FB8 -> 0x004E9C01
		0x000000F0	offset : 0x000043C0 -> 0x001EBC01
	count : 113
	-----------
	direntry_offset -> 000FDC60
	(VFAT Long Name)
	direntry_offset -> 000FDC80
	(Archive) 
	name : DOTFIL~1.    
	cluster entry : 0x00000000
	count : Empty file
	-----------
	direntry_offset -> 000FDCA0
	(Archive) 
	name : 2READ   .ING 
	cluster entry : 0x00000103
	clusters :
		0x00000103	offset : 0x0000440C -> 0x001FEC01
	count : 1
	-----------
	direntry_offset -> 000FDCC0
	(Archive) 
	name : 1GOOD   .JOB 
	cluster entry : 0x00000104
	clusters :
		0x00000104	offset : 0x00004410 -> 0x001FFC01
	count : 1
	-----------
	direntry_offset -> 000FDCE0
	(Archive) 
	name : 3THE    .    
	cluster entry : 0x00000105
	clusters :
		0x00000105	offset : 0x00004414 -> 0x00200C01
	count : 1
	-----------
	direntry_offset -> 000FDD00
	(Archive) 
	name : 4DIR!   .    
	cluster entry : 0x00000106
	clusters :
		0x00000106	offset : 0x00004418 -> 0x00201C01
	count : 1
	-----------
	direntry_offset -> 000FDD20
	(VFAT Long Name)
	direntry_offset -> 000FDD40
	(VFAT Long Name)
	direntry_offset -> 000FDD60
	(Archive) 
	name : LONGFI~1.    
	cluster entry : 0x00000107
	clusters :
		0x00000107	offset : 0x0000441C -> 0x00202C01
	count : 1
	-----------
	direntry_offset -> 000FDD80
	(VFAT Long Name)
	direntry_offset -> 000FDDA0
	(Archive) 
	name : ASHORT~1.    
	cluster entry : 0x00000108
	clusters :
		0x00000108	offset : 0x00004420 -> 0x00203C01
	count : 1
	-----------
	direntry_offset -> 000FDDC0
	(Folder) 
	name : SUB     .DIR 
	cluster entry : 0x00000109
	clusters :
		0x00000109	offset : 0x00004424 -> 0x00204C01
	count : 1
	-----------
	direntry_offset -> 000FDDE0
	(VFAT Long Name)
	direntry_offset -> 000FDE00
	(Folder) 
	name : LONGNA~1.    
	cluster entry : 0x0000010E
	clusters :
		0x0000010E	offset : 0x00004438 -> 0x00209C01
	count : 1
	-----------
	direntry_offset -> 000FDE20
	(Folder) 
	name : BIGDIR  .    
	cluster entry : 0x00000113
	clusters :
		0x00000113	offset : 0x0000444C -> 0x0020EC01
		0x00000153	offset : 0x0000454C -> 0x0024EC01
		0x00000194	offset : 0x00004650 -> 0x0028FC01
		0x000001D5	offset : 0x00004754 -> 0x002D0C01
		0x00000216	offset : 0x00004858 -> 0x00311C01
		0x00000257	offset : 0x0000495C -> 0x00352C01
		0x00000298	offset : 0x00004A60 -> 0x00393C01
		0x000002D9	offset : 0x00004B64 -> 0x003D4C01
	count : 8
	-----------
	direntry_offset -> 000FDE40
	(Archive) 
	name : EMPTY   .    
	cluster entry : 0x00000000
	count : Empty file
	-----------
	direntry_offset -> 000FDE60
	(VFAT Long Name)
	direntry_offset -> 000FDE80
	(Archive) 
	name : EMPTYF~1.    
	cluster entry : 0x00000000
	count : Empty file
	-----------
	direntry_offset -> 000FDEA0
	(Free) (VFAT Long Name)
	direntry_offset -> 000FDEC0
	(Free) (VFAT Long Name)
	direntry_offset -> 000FDEE0
	(Free) (Archive) 
	name : ?ELETE~1.    
	cluster entry : 0x00000000
	-----------
	direntry_offset -> 000FDF00
	(Archive) 
	name : RAND    .OM  
	cluster entry : 0x000004BB
	clusters :
		0x000004BB	offset : 0x000052EC -> 0x005B6C01
	count : 1
	-----------
	direntry_offset -> 000FDF20
	(Free) (Archive) 
	name : ?ERYLA~1.    
	cluster entry : 0x000000EF
	-----------
	direntry_offset -> 000FDF40
	(VFAT Long Name)
	direntry_offset -> 000FDF60
	(Archive) 
	name : HI      .    
	cluster entry : 0x0001F2DB
	clusters :
		0x0001F2DB	offset : 0x00080B6C -> 0x1F3D6C01
	count : 1
	-----------
	direntry_offset -> 000FDF80
	(Free) (Archive) 
	name : ?L2     .    
	cluster entry : 0x0001F2DC
	-----------
	direntry_offset -> 000FDFA0
	(Free) (VFAT Long Name)
	direntry_offset -> 000FDFC0
	(Free) (Archive) 
	name : ?I2     .    
	cluster entry : 0x000000EE
	-----------
	direntry_offset -> 000FDFE0
	
**********************************************