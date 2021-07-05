//Вызываемый модуль ядра Linux
//Модуль md1 демонстрирует возможность создания экспортируемых данных и функций

#include <linux/init.h> //Макросы __init и __exit 
#include <linux/module.h>
#include "md.h"

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Anastasiia Namestnik <anastasia.namestnik@gmail.com>" );

//Экспортируемые строковые данные
char* md1_data = "Привет мир!";

//Экспортируемые численные данные 
int md1_int_data = 21;

//Экспортируемая функция, возвращающая строковое значение
extern char* md1_proc( void ) 
{
   return md1_data;
}

//Экспортируемая функция, возвращающая численное значение
extern int md1_factorial(int x)
{
	int res =  1;

	while (x > 1)
	{
		res *= x;
		x -= 1;
	}

	return res;
}

//Неэкспортируемые функции
static char* md1_local( void ) 
{
   return md1_data;
}

extern char* md1_noexport( void ) 
{
   return md1_data;
}

EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_int_data);

EXPORT_SYMBOL(md1_proc);
EXPORT_SYMBOL(md1_factorial);

static int __init md_init( void ) 
{
   //Как правило, функция инициализации предназначена для 
   //запроса ресурсов и выделения памяти под структуры данных и т.п.
   printk( "+ module md1 start!\n" );
   return 0;
}

static void __exit md_exit( void ) 
{
   //Обычно эта функция выполняет задачу освобождения ресурсов. 
   // Не имеет возвращаемого значения
   printk( "+ module md1 unloaded!\n" );
}

//Макрос module_init служит для регистрации функции инициализации модуля. 
//Макрос принимает имя функции в качестве фактического параметра. 
//В результате эта функция будет вызываться при загрузке модуля в ядро. 
module_init( md_init );

//Макрос module_exit служит для регистрации функции,
//которая вызывается при удалении модуля из ядра. 
//После завершения функции модуль выгружается.
module_exit( md_exit );
