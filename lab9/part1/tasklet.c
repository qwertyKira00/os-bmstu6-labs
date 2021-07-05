#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h> //kmalloc
#include <linux/proc_fs.h> //create_proc
//#include <linux/fs.h>   
#include <linux/seq_file.h> //sequence_file ops
#include <linux/interrupt.h>

/*
struct tasklet_struct {
    struct tasklet_struct *next;
    unsigned long state;
    atomic_t count;
    void (*func)(unsigned long);
    unsigned long data;
}
*/

#define DEFINE_IRQ 1
static int counter = 0;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anastasiia Namestnik");

static struct proc_dir_entry *mytasklet_proc_file;
static struct tasklet_struct *mytasklet_struct;
void tasklet_handler(unsigned long data);
static int my_dev_id;
//char tasklet_data[] = "Anas: mytasklet data";


static int mytasklet_proc_show(struct seq_file *m, void *v)
{
    mytasklet_struct->data++;
    //seq_printf(m, "Scheduling mytasklet\n");
    //tasklet_schedule(mytasklet_struct);

    seq_printf(m, "mytasklet_module: counter is %d, state is %lu, count is %d, data is %lu\n",
        counter,
        mytasklet_struct->state,
        atomic_read(&mytasklet_struct->count),
        mytasklet_struct->data);
    return 0;
}

static int tasklet_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, mytasklet_proc_show, NULL);
}

static const struct proc_ops mytasklet_proc_fops = {
    .proc_open = tasklet_proc_open,
    .proc_read = seq_read,
    //.proc_lseek = seq_lseek,
    .proc_release = single_release,
}; 

static void mytasklet_function(unsigned long arg)
{
    printk(KERN_INFO "mytasklet_module: Now in myrasklet_function.\n");
    printk(KERN_INFO "mytasklet_module: state is %lu, count is %d, data is %lu\n",
        mytasklet_struct->state,
        atomic_read(&mytasklet_struct->count),
        mytasklet_struct->data);
}


// Обработчик прерывания
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    if (irq == DEFINE_IRQ)
    {
        ++counter;
        printk(KERN_INFO "Scheduling mytasklet\n\n");
        printk(KERN_INFO "mytasklet_module: counter is %d, state is %lu, count is %d, data is %lu\n",
        counter,
        mytasklet_struct->state,
        atomic_read(&mytasklet_struct->count),
        mytasklet_struct->data);

        tasklet_schedule(mytasklet_struct);

        printk(KERN_INFO "mytasklet_module: counter is %d, state is %lu, count is %d, data is %lu\n",
        counter,
        mytasklet_struct->state,
        atomic_read(&mytasklet_struct->count),
        mytasklet_struct->data);

        return IRQ_HANDLED; // прерывание обработано
    }
    else
        return IRQ_NONE; // прерывание не обработано

}


static int __init mytasklet_module_init(void)
{
    printk(KERN_INFO "mytasklet_module: Loading module...\n");

    mytasklet_proc_file = proc_create("mytasklet", 0444, NULL, &mytasklet_proc_fops);
    printk(KERN_INFO "mytasklet_module: proc entry created.\n");

    mytasklet_struct = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);

    //Динамическая регистрация тасклета
    //struct tasklet_struct
    //tasklet_handler
    //user-provided tasklet data
    tasklet_init(mytasklet_struct, mytasklet_function, (void *)(irq_handler));
    printk(KERN_INFO "mytasklet_module: tasklet initialized.\n");

    printk(KERN_INFO "mytasklet_module: Loading complete.\n");

    if (request_irq(DEFINE_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED, "keyboard", &my_dev_id))
    {
        printk(KERN_ERR "Error on request_irq\n");
        return -1;
    }
 
    return 0;
}

static void __exit mytasklet_module_exit(void)
{
    free_irq(DEFINE_IRQ, (void *)(irq_handler)); // Освобождение линии прерывания
    //Удаление тасклета
    tasklet_disable(mytasklet_struct);
    //Ждет завершения тасклета и удаляет из очереди на выполнение
    tasklet_kill(mytasklet_struct);
    printk(KERN_INFO "mytasklet_module: mytasklet killed.\n");

    proc_remove(mytasklet_proc_file);
    printk(KERN_INFO "mytasklet_module: proc entry removed.\n");
    
    printk(KERN_INFO "mytasklet_module: Module unloaded.\n");
}

module_init(mytasklet_module_init);
module_exit(mytasklet_module_exit); 