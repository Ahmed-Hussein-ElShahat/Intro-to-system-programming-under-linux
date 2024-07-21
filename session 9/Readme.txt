Parsing the partition tables

1- MBR partition table
	sudo ./myfdisk /dev/sdb
	
	Device     Boot  Start      End        Sectors    Size   ID        
	/dev/sdb1        2048       536575     534528     261M   83
	/dev/sdb2        536576     921599     385024     188M   7
	/dev/sdb3        921600     1443839    522240     255M   7
	/dev/sdb4        1443840    2207743    763904     373M   f                                                                
	/dev/sdb5        1445888    1611775    165888     81 M   82                                                               
	/dev/sdb6        1613824    1820671    206848     101M   83                                                               
	/dev/sdb7        1822720    2207743    385024     188M   7   
	

	sudo fdisk /dev/sdb

	Device     Boot   Start     End Sectors  Size Id Type                                                                     
	/dev/sdb1          2048  536575  534528  261M 83 Linux                                                                    
	/dev/sdb2        536576  921599  385024  188M  7 HPFS/NTFS/exFAT                                                          
	/dev/sdb3        921600 1443839  522240  255M  7 HPFS/NTFS/exFAT                                                          
	/dev/sdb4       1443840 2207743  763904  373M  f W95 Ext'd (LBA)                                                          
	/dev/sdb5       1445888 1611775  165888   81M 82 Linux swap / Solaris                                                     
	/dev/sdb6       1613824 1820671  206848  101M 83 Linux                                                                    
	/dev/sdb7       1822720 2207743  385024  188M  7 HPFS/NTFS/exFAT    

2- GPT partition table
	sudo ./myfdisk /dev/sdb
	
	Device     Start      End        Sectors    Size  
	/dev/sdb1  2048       280575     278528     136M
	/dev/sdb2  280576     655359     374784     183M
	/dev/sdb3  655360     1075199    419840     205M
	/dev/sdb4  1075200    1298431    223232     109M
	/dev/sdb5  1298432    1576959    278528     136M
	/dev/sdb6  1576960    1888255    311296     152M
	/dev/sdb7  1888256    2207743    319488     156M
	

	sudo fdisk -l /dev/sdb
	
	Device       Start     End Sectors  Size Type
	/dev/sdb1     2048  280575  278528  136M Microsoft basic data
	/dev/sdb2   280576  655359  374784  183M Linux filesystem
	/dev/sdb3   655360 1075199  419840  205M Linux filesystem
	/dev/sdb4  1075200 1298431  223232  109M Linux swap
	/dev/sdb5  1298432 1576959  278528  136M Microsoft basic data
	/dev/sdb6  1576960 1888255  311296  152M Linux filesystem
	/dev/sdb7  1888256 2207743  319488  156M Linux swap
