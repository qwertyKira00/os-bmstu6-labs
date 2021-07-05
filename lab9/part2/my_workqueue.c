#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/atomic.h>
#include <asm/io.h> //inb
#include <linux/delay.h> //msleep

/*
struct work_struct
{
	atomic_long_t data;
	struct list_head entry;
	work_func_t func;
}
*/

#define KEYBOARD_IRQ 1          //IRQ number for a keyboard (i8042)
#define KBD_DATA_REG 0x60       //I/O port for keyboard data 
#define KBD_SCANCODE_MASK 0x7f
#define KBD_STATUS_MASK 0x80

static int counter = 0;
static int my_dev_id;
static char scancode;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anastasiia Namestnik");

static struct workqueue_struct *my_wq; //очередь работ

//Функции работ 
static void my_wq_function1(struct work_struct *work);
static void my_wq_function2(struct work_struct *work);

//имя структуры work_struct, указатель на обработчик работы
//Чтобы поместить задачу в очередь задач, необходимо заполнить структуру work_struct. 
//Этот можно сделать во время компиляции следующим образом:
DECLARE_WORK(my_work1, my_wq_function1);
DECLARE_WORK(my_work2, my_wq_function2);


// Обработчик прерывания
//При нажатии любой клавиши контроллер клавиатуры распознаёт клавишу и 
//посылает её скан-код в порт 60h.
irqreturn_t irq_handler(int irq, void *dev_id)
{
    if (irq == KEYBOARD_IRQ)
    {
       	++counter;
       	printk(KERN_INFO "Workqueue: my IRQ handler was called %d times\n", counter);

        scancode = inb(KBD_DATA_REG);

        //Добавить работы в очередь
        printk(KERN_INFO "Workqueue: Queue the first work\n\n");
        queue_work(my_wq, &my_work1);
        
        printk(KERN_INFO "Workqueue: Queue the second work\n\n");
        queue_work(my_wq, &my_work2);

        return IRQ_HANDLED; // прерывание обработано
    }
    else
        return IRQ_NONE; // прерывание не обработано

}


static void my_wq_function1(struct work_struct *work) // вызываемая функция
{
	printk(KERN_INFO "Workqueue_Work1: Scan Code %x %s\n", 
      scancode & KBD_SCANCODE_MASK,
      scancode & KBD_STATUS_MASK ? "Released" : "Pressed");

  return;
}


static void my_wq_function2(struct work_struct *work) // вызываемая функция
{
   printk(KERN_INFO "Workqueue_Work2: starts sleeping");
   //msleep - sleep safely even with waitqueue interruptions
   msleep(10);
   printk(KERN_INFO "Workqueue_Work2: stops sleeping");

   return;
}


static int __init myworkqueue_module_init(void)
{
	printk(KERN_INFO "myworkqueue_module: Loading module...\n");
	
  //Драйверы регистрируют обработчик аппаратного прерывания и 
  //разрешают определенную линию irq посредством функции:
	if (request_irq(KEYBOARD_IRQ, irq_handler, IRQF_SHARED, "keyboard", &my_dev_id))
    {
        printk(KERN_ERR "Workqueue: Error on request_irq\n");
        return -ENOMEM;
    }

	//создание очереди работ
	if ((my_wq = alloc_workqueue("my_queue", WQ_UNBOUND, 2))) {
		printk(KERN_INFO "Workqueue was successfully created!\n");
	}
	else
	{
		free_irq(KEYBOARD_IRQ, (void *)(irq_handler)); // Освобождение линии прерывания
		printk(KERN_ERR "Workqueue was not created\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "myworkqueue_module: Module is now loaded\n");
    return 0;
}


static void __exit myworkqueue_module_exit(void) 
{
    //Завершение очереди
    flush_workqueue(my_wq);
    destroy_workqueue(my_wq);
    printk(KERN_INFO "Workqueue: workqueue was destroyed\n");
    free_irq(KEYBOARD_IRQ, &my_dev_id);
    printk(KERN_INFO "Workqueue: my IRQ handler was removed\n");
    printk(KERN_INFO "Workqueue: Module is now unloaded\n");
}

module_init(myworkqueue_module_init);
module_exit(myworkqueue_module_exit); 








