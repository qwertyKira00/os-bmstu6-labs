#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/slab.h>

#define MYFS_MAGIC_NUMBER 0x1313131313;
#define SLABNAME "my_cache"
static int myfs_fill_sb(struct super_block *, void *, int);

//myfs_mount должна примонтировать устройство и вернуть структуру, 
//описывающую корневой каталог файловой системы
static struct dentry* myfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
     struct dentry *const entry = mount_nodev(fs_type, flags, data, myfs_fill_sb);
     
     //printk(KERN_DEBUG "flags = %d + dev_name = %s", flags, dev_name);
     if (IS_ERR(entry))
          printk(KERN_ERR "myfs mounting failed!\n");
     else
          printk(KERN_DEBUG "myfs mounted");
     return entry;
     
}
static struct file_system_type myfs_type = {
     .owner = THIS_MODULE,    //счетчик ссылок на модуль
     .name = "myfs",
     .mount = myfs_mount,
     .kill_sb = kill_litter_super,
};

struct myfs_inode
{
     int i_mode;
     unsigned long i_ino;
};

int inode_number = 0;
static struct kmem_cache *cache;

static void myfs_put_super(struct super_block *sb)
{
     printk(KERN_DEBUG "myfs super block destroyed\n");
}

int free_alloc_inodes(struct inode *inode)
{
     kmem_cache_free(cache, inode->i_private);
     return 1;
}

static struct super_operations const myfs_super_ops = {
     .put_super = myfs_put_super,
     .statfs = simple_statfs,
     .drop_inode = free_alloc_inodes,
};

static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
     struct inode *ret = new_inode(sb);
     if (ret)
     {
          //get free pages kernel
          struct myfs_inode *my_inode = kmem_cache_alloc(cache, GFP_KERNEL);
           
          //printk( KERN_INFO "allocate %d objects into slab: %s\n", i_number, SLABNAME); 
         
          //Init uid,gid,mode for new inode according to posix standards
          inode_init_owner(ret, NULL, mode);

          printk( KERN_INFO "myfs: inode_number: %lu\n mode: %d\n", ret->i_ino, ret->i_mode); 

          *my_inode = (struct myfs_inode){
               .i_mode = ret->i_mode,
               .i_ino = ret->i_ino
          };
          ret->i_size = PAGE_SIZE;
          //mtime - modification time - время последней модификации (изменения) файла
          //atime - access time - время последнего доступа к файлу
          //ctime - change time - время последнего изменения атрибутов файла (данных которые хранятся в inode-области)
          ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
          ret->i_private = my_inode;

     }
     return ret;
}

//Создает корневой каталог ФС
static int myfs_fill_sb(struct super_block *sb, void *data, int silent)
{
     struct inode *root = NULL;

     /* Устанавливаем поля суперблока */
     sb->s_blocksize = PAGE_SIZE;
     sb->s_blocksize_bits = PAGE_SHIFT;
     sb->s_magic = MYFS_MAGIC_NUMBER;
     sb->s_op = &myfs_super_ops;

     /* Создание inode для корневого каталога */
     // S_IFDIR - создаем каталог (регулярный).
     // 0755 - стандартные права доступа для папок (rwx r-x r-x). 
     //(владелец группа остальные_пользователи)
     root = myfs_make_inode(sb, S_IFDIR|0755);
     if (!root)
     {
          printk(KERN_ERR "myfs inode allocation failed\n");
          return -ENOMEM;
     }
     root->i_op = &simple_dir_inode_operations;
     root->i_fop = &simple_dir_operations;

     /* Создание dentry для корневого каталога */

     // Суперблок - структура, которая описывает ФС (метаданные).
     // inode - метаданные о файле (!не содержит путь!) (содержит тип файла, права доступа и тд...)
     // dentry (directory entry - запись каталога) - держит дескрипторы и файлы вместе, связывая номер индексных дескрипторов файлов с именами файлов.
     sb->s_root = d_make_root(root);

     if (!sb->s_root)
     {
          printk(KERN_ERR "myfs root creation failed\n");
          iput(root);
          return -ENOMEM;
     }

     return 0;
}

void co (void *p)
{
     *(int *)p = (int)p;
     inode_number++;
}

static int __init myfs_init(void)
{
     int ret = register_filesystem(&myfs_type);
     cache = kmem_cache_create(SLABNAME, sizeof(struct myfs_inode), 0, SLAB_HWCACHE_ALIGN, co);
    
     
     //printk( KERN_INFO "object size %d bytes, full size %ld bytes\n", size, (long)size * number ); 
      //printk( KERN_INFO "constructor called %d times\n", sco ); 


     if (ret != 0)
     {
          printk(KERN_ERR "myfs can't register filesystem\n");
          return ret;
     }
     printk(KERN_INFO "myfs filesystem registered");
     return 0;
}

static void __exit myfs_exit(void)
{
     int ret = unregister_filesystem(&myfs_type);
     if (ret != 0)
          printk(KERN_ERR "myfs can't unregister filesystem!\n");
     kmem_cache_destroy(cache);
     printk(KERN_INFO "myfs unregistered; constructor called %d times\n", inode_number);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Namestnik Anastasiia");

module_init(myfs_init);
module_exit(myfs_exit);
