#include "fs.h"
#include <iostream>
using namespace std;

myFileSystem::myFileSystem(char diskName[16]) {
  // open the file with the above name
  // this file will act as the "disk" for your file system
  disk.open(diskName, std::fstream::in | std::fstream::out | std::fstream::binary);
}

bool haveSufficientBlock(char *freeList,int size) {
  int block_needed = (size<block_size) ? 1:(size/block_size);
  int counter = 0;
  for (int i = 0; i < 128; i++) {
    if (freeList[i] == 0) {
      counter += 1;
    }
  }
  if (counter >= block_needed) {
    return true;
  } else {
    return false;
  }
}

void write_freeList(char* freeList,fstream* disk) {
  disk->seekg(0, (*disk).beg);
  disk->write(freeList,128);
}

void write_inode(idxNode* inode,int inode_index,fstream* disk) {
  disk->seekg(128+sizeof(idxNode)*inode_index,(*disk).beg);
  char* temp = new char[sizeof(idxNode)];
  memcpy(temp, inode, sizeof(idxNode));
  disk->write(temp,sizeof(idxNode));
}

int myFileSystem::create_file(char name[8], int size) {
  //create a file with this name and this size

  // high level pseudo code for creating a new file

  // Step 1: Check to see if we have sufficient free space on disk by
  //   reading in the free block list. To do this:
  // Move the file pointer to the start of the disk file.
  // Read the first 128 bytes (the free/in-use block information)
  // Scan the list to make sure you have sufficient free blocks to
  //   allocate a new file of this size

  // Step 2: we look for a free inode on disk
  // Read in an inode
  // Check the "used" field to see if it is free
  // If not, repeat the above two steps until you find a free inode
  // Set the "used" field to 1
  // Copy the filename to the "name" field
  // Copy the file size (in units of blocks) to the "size" field

  // Step 3: Allocate data blocks to the file
  // for(i=0;i<size;i++)
  // Scan the block list that you read in Step 1 for a free block
  // Once you find a free block, mark it as in-use (Set it to 1)
  // Set the blockPointer[i] field in the inode to this block number.
  // end for

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
  char* freeList = new char[128];
  disk.seekg(0);
  disk.read(freeList,128);
  for (int i = 0; i < 128; i++) {
    if (freeList[i] == 0) {
      std::cout << "0" << '\n';
    } else {
      std::cout << "1" << '\n';
    }
  }
  idxNode *inode;
  if (haveSufficientBlock(freeList,size)) {
    // skip to inode start position
    disk.seekg(128);
    std::cout << "Sufficent works" << '\n';
    int inode_index = 0;
    // look for free inode
    while (inode_index < 16) {
      inode = new idxNode();
      char *temp = new char[sizeof(idxNode)];
      std::cout << "=======>" << temp << '\n';
      disk.seekg(sizeof(idxNode)*inode_index);
      disk.read(temp,sizeof(idxNode));
      std::cout << "inode byte ======>" <<temp << '\n';
      memcpy(inode, temp, sizeof(idxNode));
      std::cout << "mapping byte to inode works" << '\n';
      if ((inode->used) == 0) {
        strcpy(inode->name,name);
        inode->used = 1;
        inode->size = (size<block_size) ? 1:(size/block_size);
        break;
      }
      inode = NULL;
    }
    std::cout << inode->size << '\n';
    disk.seekg(0);
    // find avaliable free list
    int* tempPointers = new int[8];
    int blockPointer_index = 0;
    for (int i = 0; i < 128; i++) {
      if (blockPointer_index < inode->size) {
        if(freeList[i] == 0) {
          freeList[i] = 1;
          std::cout << freeList[i] << '\n';
          tempPointers[blockPointer_index] = i;
          blockPointer_index += 1;
        }
      } else {
        break;
      }
    }
    std::cout <<  tempPointers << '\n';
    write_freeList(freeList,&disk);
    write_inode(inode,inode_index,&disk);
    return 1;
  }
  return 0;
} // End Create

int myFileSystem::delete_file(char name[8]){
  // Delete the file with this name

  // Step 1: Locate the inode for this file
  // Move the file pointer to the 1st inode (129th byte)
  // Read in an inode
  // If the inode is free, repeat above step.
  // If the inode is in use, check if the "name" field in the
  //   inode matches the file we want to delete. If not, read the next
  //   inode and repeat

  // Step 2: free blocks of the file being deleted
  // Read in the 128 byte free block list (move file pointer to start
  //   of the disk and read in 128 bytes)
  // Free each block listed in the blockPointer fields as follows:
  // for(i=0;i< inode.size; i++)
  // freeblockList[ inode.blockPointer[i] ] = 0;

  // Step 3: mark inode as free
  // Set the "used" field to 0.

  // Step 4: Write out the inode and free block list to disk
  // Move the file pointer to the start of the file
  // Write out the 128 byte free block list
  // Move the file pointer to the position on disk where this inode was stored
  // Write out the inode
  disk.seekg(128, disk.beg);
  int inode_index = 0;
  idxNode *inode;
  int* blockPointer;
  // look inode
  while (inode_index < 16) {
    char* temp = new char[sizeof(idxNode)];
    disk.seekg(sizeof(idxNode)*inode_index,disk.cur);
    disk.read(temp,sizeof(idxNode));
    if ((inode->used) == 1) {
      if (strcmp(inode->name,name)) {
        blockPointer = inode->blockPointers;
      }
      break;
    }
    inode = NULL;
  }
  disk.seekg(0,disk.beg);
  if (inode == NULL) {
    return -1;
  }else {
    // set the free list back to free
    char *freeList = new char[128];
    disk.seekg(0,disk.beg);
    disk.read(freeList,128);
    for (int i = 0; i < 8; i++) {
      if (blockPointer[i] != 0 ) {
        freeList[i] = 0;
      }
    }
    inode->used = 0;
    write_freeList(freeList,&disk);
    write_inode(inode,inode_index,&disk);
    return 1;
  }
} // End Delete


int myFileSystem::ls(){
  // List names of all files on disk

  // Step 1: read in each inode and print
  // Move file pointer to the position of the 1st inode (129th byte)
  // for(i=0;i<16;i++)
  // Read in an inode
  // If the inode is in-use
  // print the "name" and "size" fields from the inode
  // end for
    return 0;
}; // End ls

int myFileSystem::read(char name[8], int blockNum, char buf[1024]){
  // read this block from this file

  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in an inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  // Step 2: Read in the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., to byte #
  //   addr*1024 in the file)

  // Read in the block => Read in 1024 bytes from this location
  //   into the buffer "buf"
  return 0;
} // End read


int myFileSystem::write(char name[8], int blockNum, char buf[1024]){
  // write this block to this file

  // Step 1: locate the inode for this file
  // Move file pointer to the position of the 1st inode (129th byte)
  // Read in a inode
  // If the inode is in use, compare the "name" field with the above file
  // If the file names don't match, repeat

  // Step 2: Write to the specified block
  // Check that blockNum < inode.size, else flag an error
  // Get the disk address of the specified block
  // That is, addr = inode.blockPointer[blockNum]
  // Move the file pointer to the block location (i.e., byte # addr*1024)

  // Write the block! => Write 1024 bytes from the buffer "buff" to
  //   this location
  return 0;
} // end write

int myFileSystem::close_disk(){
    return 0;
}
