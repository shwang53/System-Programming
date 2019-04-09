
/**
 * Finding Filesystems
 * CS 241 - Spring 2019
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", "myvirt"};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */

static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    inode* myinode = get_inode(fs, path);
    
    if (valid_filename(path) != 1) {
	errno = ENOENT;
	return -1;
    }
    if (!path) {
	errno = ENOENT;
	return -1;
    }
    if (myinode == NULL) {
	return -1;
    }

    uint16_t perm = new_permissions << RWX_BITS_NUMBER;
    myinode->mode = myinode->mode | perm;

    clock_gettime(CLOCK_REALTIME, &myinode->ctim);
    // Thar she blows!
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    inode* myinode = get_inode(fs, path);
    
    if (valid_filename(path) != 1) {
	errno = ENOENT;
	return -1;
    }
    if (!path) {
	errno = ENOENT;
	return -1;
    }
    if (!myinode) {
	return -1;
    }

    if (owner != ((uid_t)-1)) {
	myinode->uid = owner;
    }
    if (group != ((uid_t)-1)) {
	myinode->gid = group;
    }

    clock_gettime(CLOCK_REALTIME, &myinode->ctim);
    
    // Land ahoy!
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    inode* myinode = get_inode(fs, path);
    if (myinode != NULL) {
	//inode already exists
	fprintf(stderr, "Filename not unique");
	return NULL;
    }

    const char* fn;
    inode* node = parent_directory(fs, path, &fn);
    inode* parent = node;
    if (!valid_filename(fn)) {
	return NULL;
    }
    if (strlen(fn) > FILE_NAME_LENGTH) {
	return NULL;
    }
    if (!is_directory(node)) {
	return NULL;
    }

    data_block_number last_data = node->size / sizeof(data_block);
    data_block_number* b_arr = node->direct;
    bool flag = false;
    if (last_data >= NUM_DIRECT_INODES) {
	if (node->indirect == UNASSIGNED_NODE) {
	    inode_number bn = add_single_indirect_block(fs, node);
	    if (bn == -1) {
		return NULL;
	    }
	}
	b_arr = (data_block_number*)(fs->data_root + node->indirect);
	flag = true;
	last_data = last_data - NUM_DIRECT_INODES;
    }

    int ninode = first_unused_inode(fs);
    init_inode(node, (fs->inode_root + ninode));
    size_t off_last = parent->size % sizeof(data_block);
    data_block* bk = fs->data_root + b_arr[last_data];
    if (off_last== 0) {
	data_block_number d_block;
	if (flag == true) {
	    d_block = add_data_block_to_indirect_block(fs, b_arr); 
	}
	else {
	    d_block = add_data_block_to_inode(fs, node);
	}
	if (d_block == -1) {
	    return NULL;
	}
	bk = fs->data_root + d_block;
    }
    char* cp = calloc(1, FILE_NAME_LENGTH);
    strncpy(cp, fn, FILE_NAME_LENGTH);
    memcpy((((char*)bk) + off_last), cp, FILE_NAME_LENGTH);
    sprintf((((char*)bk) + off_last + FILE_NAME_LENGTH), "%08zx", (size_t)ninode);
    parent->size = parent->size + FILE_NAME_ENTRY;
    free(cp);

    
    // Land ahoy!
    return (fs->inode_root + ninode);
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
    }
    
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    int bc = minixfs_min_blockcount(fs, path, count);
    if (bc == 0) {
		inode* newinode = minixfs_create_inode_for_path(fs, path);
	if (newinode != NULL) {
	    data_block_number t1 = add_data_block_to_inode(fs, newinode);
	    if (t1 == -1) {
		errno = ENOSPC;
		return -1;
	    }
	    data_block_number t2 = add_data_block_to_indirect_block(fs, &t1);
	    if (t2 == -1) {
		errno = ENOSPC;
		return -1;
	    }
	    clock_gettime(CLOCK_REALTIME, &newinode->mtim);
	}
	else {
	    add_single_indirect_block(fs, newinode);
	    data_block_number t3 = add_data_block_to_inode(fs, newinode);
	    if (t3 == -1) {
		errno = ENOSPC;
		return -1;
	    }
	    data_block_number t4 = add_data_block_to_indirect_block(fs, &t3);
	    if (t4 == -1) {
		errno = ENOSPC;
		return -1;
	    }
	    clock_gettime(CLOCK_REALTIME, &newinode->mtim);
	}
    }
    if (bc == -1) {
	//errno = ENOENT;
	return bc;
    }

    inode* myinode = get_inode(fs, path);
    clock_gettime(CLOCK_REALTIME, &myinode->ctim);
    off = off + count;
    myinode->size = myinode->size + count;
    
    // X marks the spot
    return count;    
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);

    int bc = minixfs_min_blockcount(fs, path, count);
    if (bc == 0) {
	inode* newinode = minixfs_create_inode_for_path(fs, path);
	if (newinode != NULL) {
	    return 0;
	}
    }
    if (bc == -1) {
	errno = ENOENT;
	return bc;
    }

    inode* myinode = get_inode(fs, path);
    //if (!myinode) {
    //	return -1;
    //}

    clock_gettime(CLOCK_REALTIME, &myinode->atim);
    off = off + count;
    
    
    // 'ere be treasure!
    return count;
}

/*
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    return -1;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    return NULL;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
    }
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    return -1;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!
    return -1;
}
*/
