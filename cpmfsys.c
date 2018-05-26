#include "cpmfsys.h"
#include "diskSimulator.h"
#include <regex.h> 

#define Disk_Block_Numbers 16

bool freeList[NUM_BLOCKS]; 

DirStructType  *mkDirStruct(int index,uint8_t *e){ 
	DirStructType *d;
	int i,j,k;
	uint8_t *fv;
	
	fv = e + index * EXTENT_SIZE;	//fixing the variables for ease of use
	
	d = malloc(sizeof(DirStructType));	//Allocate memory to d
	d->status = fv[0];			//Update status of block
	int ecount = 0;				
	int ecount1 = 0;			//Variables to hold extent count one for filename and one for extension
	
	for (i=0;i<8;i++){			//Loop to copy the name of file
		if(fv[i+1]!= ' ')
			(d->name)[i] = fv[i+1];		//If bits are not empty name will be copied to directory structure
		else{
			ecount++;
			(d->name)[i]  = '\0';		//Append termination character at the end
			break;
		}
	}
	if(!ecount)			
	(d->name)[i]  = '\0';		//If name is empty then update it with termination character
	
	for (j=0;j<3;j++){			//Loop to copy extension of file 
		if(fv[j+9]!= ' ')		
		(d->extension)[j] = fv[j+9];	//copy extensions if its not blank
		else{
		ecount1++;				
		(d->extension)[j]  = '\0';		//Append termination character at the end
		break;
		}
	}
	if(!ecount1)
	(d->extension)[j]  = '\0';		//If extension is empty then update it with termination character
	
	 d->XL = fv[12];		
	 d->BC = fv[13];
	 d->XH = fv[14];
	 d->RC = fv[15];			//copy the attributes to directory structure

	for (k=0;k<16;k++){
		(d->blocks)[k] = fv[k+16];	//copy the index values to directory structure
	}
	return d; 	//Return directory structure
}

void writeDirStruct(DirStructType *d,uint8_t index, uint8_t *e){ 
	int i,j,k;
	uint8_t *fv;
	fv = e + index * EXTENT_SIZE;		//fixing the variables for ease of use
	
	fv[0] = d->status;		//get the status of block
	
	for (i=0;i<8;i++){				//get the name of file 
		if((d->name)[i] != '\0')
		fv[i+1] = (d->name)[i];		//if the bit is not null(termination) then copy each bit
		else
		fv[i+1] = ' ';		//Else put a blank space till 8 characters
	}		
	for (j=0;j<3;j++){						//get the extension of the file
		if((d->extension)[j] != '\0')
		fv[j+9] = (d->extension)[j];			//if the bit is not null(termination) then copy each bit
		else
		fv[j+9] = ' ';		//Else put a blank space till 8 characters
	}
	
	fv[12] =  d->XL ;
	fv[13] =  d->BC ;
	fv[14] =  d->BC ;
	fv[15] =  d->RC ;			//copy the attributes of block data

	for (k=0;k<16;k++){
		 fv[k+16] = (d->blocks)[k] ;		//copy the actual data index
	}

} 

void makeFreeList(){
  uint8_t larray[BLOCK_SIZE];
  DirStructType *d; 
  int i,j; 
   
  freeList[0] = false;  //To indicate that the block 0 is used for maintain directory we make the flag to false(Not free)

  for (i=1;i<NUM_BLOCKS;i++){ 
    freeList[i] = true; 		//To initially set all the other blocks to true(false)
  } 
  
  blockRead(larray,0); 		//Read a block starting with 0
  
  for (i=0;i<BLOCK_SIZE/EXTENT_SIZE;i++){ 	//browse through all the blocks 
      d = mkDirStruct(i,larray); 	//get the directory structure 
      if (d->status != 0xe5) { 			//Check if the block is not unused
      for (j=0;j<Disk_Block_Numbers;j++){    
		if (d->blocks[j] != 0){ 
		freeList[(int) d->blocks[j]] = false; //If it is unused update it to not be available
		}
      }
    }
  }
  
} 

void printFreeList(){
  int i,j;
  printf("FREE BLOCK LIST: (* means in-use)\n");  //To bring in a specific format of output 
  for (i = 0; i < Disk_Block_Numbers; i++){		//Iterate through all the blocks
    printf("%2x: ",i*16); 			//Print Hexadecimal Values and have minimum two digits
    for (j = 0; j< Disk_Block_Numbers; j++){ 
    if (freeList[i*16+j])		//Check if the block is free 
	printf(". "); 			//Print if free
	else  
	printf("* "); 			//Print if not free
    }
    printf("\n"); 	//Output formatters
  }

}

bool checkLegalName(char *name){
	regex_t alwdchars;	//variable to store regex
	int i,result;
	
	for(i=0;i<15;i++){
		result = regcomp(&alwdchars, "^[a-zA-Z0-9]{1,8}.[a-z]{0,3}$", REG_EXTENDED|REG_NOSUB);  //Regex to have the allowed characters in filenames(included extension and period)
		if (result){
		printf("Could not compile regex\n");		
		exit(EXIT_FAILURE);				//checks to see if the regular expression fails
		}
		result = regexec(&alwdchars, name, 0, NULL, 0);		//Executes the regular expression against the name passed as a parameter
		if (!result){
		return true;		//If the names is valid
		}
		else if (result == REG_NOMATCH){
		return false;		//If name is inValid
		}
	}
} 

int findExtentWithName(char *name, uint8_t *block0){ 
  char fname[9]; 
  char fext[4];
  int i = 0;
  int ecounter = 0;
  
  while (name[i] != '\0' && name[i] != '.'&& i < 8){
    fname[i] = name[i];
    i++;
  } 
  fname[i] = '\0'; 			//Copy file name to a variable and append termination at the end
   
  if (name[i] == '.'){ 
    i++ ;  
    while (name[i] != '\0' && ecounter  < 3){
      fext[ecounter++] = name[i++]; 
    } 
    fext[ecounter] = '\0'; 		//Copy file extension to a variable and append termination at the end
    }
	 else 
      fext[0] = '\0';		//If extension is empty then just include termination in it
  i=0;
  for (;i<BLOCK_SIZE/EXTENT_SIZE;i++){
	DirStructType *d; 
    d = mkDirStruct(i,block0);
	int namecmp, extcmp;
	namecmp = strcmp(d->name,fname);
	extcmp = strcmp(d->extension,fext);		//Iterate through all the blocks and check to see if the file name and extension matches with anything available
 	
    if (namecmp == 0 && extcmp == 0){ 
      if (d->status == 0xe5) 
		return -1;		//Return false if the block is unused
	  else	
        return i;		//If match found return that block
    }
  }
  return -1;	//Return false if the file not found
}

void cpmDir(){
	uint8_t rarray[BLOCK_SIZE];
	DirStructType *d;
	int i,j;
	int tfl,nblocks,sblk;
	int extlen=0;
	int s_offset = 1024;		//Sector Offset value
	int b_offset = 128;			//Byte Offset value
	
	
	nblocks = BLOCK_SIZE/EXTENT_SIZE;		//Fixing variables for ease of use
	
	blockRead(rarray, 0);
	printf("\nDIRECTORY LISTING\nFileName\tFileSize\n");		//Format output
	for(i=0;i<nblocks;i++){    
		    sblk = 0;
			d = mkDirStruct(i,rarray);	//get the directory structure 
			
			extlen = strlen(d->extension);		//measure the length of extension to see if extension is not empty
			if(d->status != 0xe5){
				for (j=0;j<Disk_Block_Numbers;j++){
					if (d->blocks[j] != 0)
						sblk++;				//Increment the number of blocks
				}
					
				tfl = ((sblk -1)* s_offset ) + (d->RC * b_offset) + (d->BC);		//Calculate the total file length 
				
				if(extlen)
				printf("%s.%s\t%d\n",d->name,d->extension,tfl);
				else
				printf("%s\t\t%d\n",d->name,tfl);		//Format the output based on the existence of extension
			}
		}
		printf("\n"); //Output formatters
}

int cpmDelete(char *fileName){
  int i=0;
  uint8_t block0[1024];
  int extent;
  DirStructType *d;  
  
  blockRead(block0,0); 								//Read the block starting from 0
  extent = findExtentWithName(fileName,block0); 	//Check to find the block with given filename
  if (extent < 0) 
    return -1;		//If doesn't exist return false
  else{ 
    d = mkDirStruct(extent,block0);  //Get the mentioned block
	while(i<Disk_Block_Numbers){
    if (d->blocks[i] != 0)
	freeList[d->blocks[i]] = true;		//make the block free
	i++;
    }
    block0[extent*EXTENT_SIZE] = 0xe5;		//Set status to unused
    blockWrite(block0,0); 				//Put the values back to block
  return 0; 
  }
}

int cpmRename(char *oldName, char *newName){ 
  uint8_t block0[1024];
  int index, nameSize,i=0;
  char nname[9]; 
  char next[4];
  int ecounter = 0;
  char *dot; 
  DirStructType *d; 
  
 
  bool newlegal,oldlegal;
  newlegal = checkLegalName(newName);
  oldlegal = checkLegalName(oldName);			//Check if both the names are legal (valid)
  
  if (newlegal==0||oldlegal == 0){ 
      return -1; 							//If either names are invalid then return false
  }
  
  blockRead(block0,0); 							//Read the block 0
  index = findExtentWithName(oldName,block0); 		//Get the location of old block name
  if (index < 0) 
    return -1; 						//If block not available return false
  else{
    d = mkDirStruct(index,block0);			//get the contents of block
	while (newName[i] != '\0' && newName[i] != '.'&& i < 8){
    nname[i] = newName[i];				//Update the name
    i++;
	} 
	nname[i] = '\0'; 		//Append termination at the end
   
	if (newName[i] == '.'){ 				//Checkpoint to see if extension is available 
    i++ ;  
    while (newName[i] != '\0' && ecounter  < 3){
      next[ecounter++] = newName[i++]; 					//Copy the extension
    } 
    next[ecounter] = '\0'; 		//Append the extension
    }
	strcpy(d->name,nname);				//Copy back the new name
	strcpy(d->extension,next);			//Copy back the new extension 
    writeDirStruct(d,index,block0);		//Write it back to the directory
    blockWrite(block0,0); 
    return 0; 
  }

} 

