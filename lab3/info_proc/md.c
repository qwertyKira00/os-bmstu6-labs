//Модуль, выводящий информацию о процессах в log файл

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> //KERN_INFO
#include <linux/init_task.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Anastasiia Namestnik <anastasia.namestnik@gmail.com>" );

static int __init md_init( void ) 
{
   printk( "+ module md start!\n" );
   
   struct task_struct *task = &init_task;

   do
   {
   		printk(KERN_INFO "+ process_name: %s - process_id: %d, parent_name: %s - parent_id: %d\n",
   			task->comm,
   			task->pid,
   			task->parent->comm,
   			task->parent->pid);
   } while ((task = next_task(task)) != &init_task);
   
   printk(KERN_INFO "+ current_name: %s - current_id: %d, current_parent_name: %s - current_parent_id: %d\n",
   			current->comm,
   			current->pid,
   			current->parent->comm,
   			current->parent->pid);

   return 0;
}
static void __exit md_exit( void ) 
{
   printk( "+ module md unloaded!\n" );
}
module_init( md_init );
module_exit( md_exit );
