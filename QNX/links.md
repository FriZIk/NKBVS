Статьи для знакомства:
	1. https://habr.com/ru/post/124656/
	2. https://habr.com/ru/post/154269/
	3. https://habr.com/ru/post/124778/

Получение студенческой лицензии QNX:
	https://blackberry.qnx.com/en/company/qnx-in-education

СВД, ЗОСРВ-нейтрино:
	1. https://kpda.ru/products/kpda10964/
	2. https://kpda.ru/products/komplrazrab/

Гайд по установке QNX на Virtual Box:
	https://ru.bmstu.wiki/QNX#cite_note-26
	https://www.youtube.com/watch?v=AbM_KF_n4sw

03.08.2021 
Последовательность действий для напаисания всяческой шняги:
1. Скачиваем и устанавливаем среду Momentics
2. Создаём виртуальную машину с QNX Neutrino
3. В QNX активируем менеджер сервисов для удаленных компонентов среды IDE - qconn(дефолтый порт 8000)
4. С помощью ifconfig узнаём ip-фдресс вирутальной машины с QNX
5. При билде в Momentics прокидываем по ip виртуальную машину
6. Сейчас буду писать какой-нибудь HelloWorld
7. Ну впринципе ничего сложного, надо теперь попробовать уже конкретно в PCI пойти

```
#include <stdio.h>
#include <stdlib.h>

int main(void) 
{
	puts("Hello world!!!"); /* prints Hello World!!! */
	return EXIT_SUCCESS;
}
```
8. Пока мне никто ничего особо не объяснял, надо погуглить и почитать в книжечках тех что мне выдали впринципе о драйверах, о связи с перефирией и тд. В целом задача не выглядит так уж супер страшной. Ссылки на посмотреть:
	1. https://www.cta.ru/cms/f/352129.pdf
	2. https://www.swd.ru/print.php3?pid=984
	3. https://osdev.fandom.com/ru/wiki/%D0%93%D0%BB%D0%B0%D0%B2%D0%BD%D0%B0%D1%8F (Наверное мастхев по теории, но надо ещё 		посмотреть)

04.08.2021
1. Ну поразбирался сегодня немного, ряд интерсных вещей и ссылок нашёл, надо их сюда по возможности побольше прикрепить.
2 Ссылки на интересные материалы:
	http://forum.kpda.ru/index.php/topic,1546.0.html
	http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fp%2Fpci_find_device.html
	http://www.qnx.com/developers/docs/qnxcar2/index.jsp?topic=%2Fcom.qnx.doc.neutrino.sys_arch%2Ftopic%2Fipc_mmap.html
	http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fp%2Fpci_attach_device.html&anchor=DevInfo
	http://www.qnx.com/developers/docs/qnxcar2/index.jsp?topic=%2Fcom.qnx.doc.neutrino.utilities%2Ftopic%2Fp%2Fpci.html
	https://pci-ids.ucw.cz/read/PC/1022/2000
	https://wiki.debian.org/DeviceDatabase/PCI
	https://pcisig.com/membership/member-companies?combine=
3. В целом начал писать какой-то код, вышло что-то вот такое:
```
#include <hw/pci.h>
#include <hw/pci_devices.h>
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
    int pidx;
    int phdl;
    struct pci_dev_info inf;

    /* Connect to the PCI server */
    phdl = pci_attach( 0 );
    if( phdl == -1 ) {
        fprintf( stderr, "Unable to initialize PCI\n" );

        return EXIT_FAILURE;
    }

    /* Initialize the pci_dev_info structure */
    memset( &inf, 0, sizeof( inf ) );
    pidx = 0;
    inf.VendorId = 0x8086;
    inf.DeviceId = 0x2415;

    fprintf(stdout, "VendorId:%x\nDeviceId:%x\n", inf.VendorId, inf.DeviceId);

    unsigned int devfunc, bus;
    int status = pci_find_device(inf.DeviceId, inf.VendorId, pidx, &bus, &devfunc);
    if (status == PCI_SUCCESS) // Если устройство найдено, то пытаемся подконектиться
    {
    	fprintf(stdout,"Device is exist");
    	hdl = pci_attach_device( NULL, PCI_INIT_ALL, pidx, &inf);
    }
    else
    {
    	fprintf(stdout,"Device not found");
    }
    return EXIT_SUCCESS;
}
```
4. Пока что всё, потом вечером, может быть, ещё что-то поделаю.

05.08.2021
1. Ничего я вечером не делал, то Волошин со своими солнечными батареями, то я сам сижу уже и ничего не хочу. Итог один, я ничего не делал.
2. Зато сегодня достаточно неплохо углубился в тему pci. Получилось подконектиться к устройству ethernet-контроллера и вытащить из него данные. Код приложу сюда, но не сейчас, может быть ещё что-то получится написать. В целом могу сказать, что разработка идёт намного бодрее чем с биосом, так как есть об этом хоть какая-то инфомрация в интернете.

```
#include <hw/pci.h>
#include <hw/pci_devices.h>
#include <stdio.h>
#include <stdlib.h>

int main( void )
{
    int pidx;
    int phdl;
    struct pci_dev_info pci_info;

    /* Connect to the PCI server */
    phdl = pci_attach(0);
    if( phdl == -1 ) {
        fprintf( stderr, "Unable to initialize PCI\n" );

        return EXIT_FAILURE;
    }

    /* Initialize the pci_dev_info structure */
    memset( &pci_info, 0, sizeof(pci_info) );
    pidx = 0;

    /* VendorId and DeviceId for network controller */
    pci_info.VendorId = 0x1022;
    pci_info.DeviceId = 0x2000;

    fprintf(stdout, "VendorId:%x\nDeviceId:%x\n", pci_info.VendorId, pci_info.DeviceId);

    unsigned int devfunc, bus;
    int status = pci_find_device(pci_info.DeviceId, pci_info.VendorId, pidx, &bus, &devfunc);

    /* Handles for attach to device */
    void* pci_dev_hdl;
    void* retval;

    if (status == PCI_SUCCESS)
    {
    	fprintf(stdout,"\nDevice is exist!\n");
    	/* Work with two attaches */
    	pci_dev_hdl = pci_attach_device( NULL, 0, pidx, &pci_info);
    	if(pci_dev_hdl == NULL)
    	{
    		fprintf(stderr,"Error attach device");
    	}
    	else
    	{
    		fprintf(stdout,"PCI_attach_device good!\n");
    		retval = pci_attach_device(pci_dev_hdl, PCI_INIT_ALL, pidx, &pci_info);
    		if(retval == NULL)
    		{
    			fprintf( stderr, "Unable allocate resources\n" );
    		}
    		else
    		{
    			fprintf(stdout, "Recourses allocate successfully!\n");

    			/* Summary information about device */
    			fprintf(stdout, "\nSUMMARY DEVICE INFO:\n");
    			fprintf(stdout, "Device_ID = 0x%x\n", pci_info.DeviceId);
    			fprintf(stdout, "Vendor_ID = 0x%x\n", pci_info.VendorId);
    			fprintf(stdout, "Bus Number = 0x%x\n", pci_info.BusNumber);
    			fprintf(stdout, "Device Function = 0x%x\n", pci_info.DevFunc);
    			fprintf(stdout, "Device Class = 0x%x\n", pci_info.Class);
    			fprintf(stdout, "Device IRQ = 0x%x(%d)\n", pci_info.Irq);
    			fprintf(stdout, "MSI REG = 0x%x\n", pci_info.msi);
    			fprintf(stdout, "CPU base address = 0x%x\n", pci_info.CpuBaseAddress);
    			fprintf(stdout, "PCI base address = 0x%x\n", pci_info.PciBaseAddress);
    		}
    		pci_detach_device(pci_dev_hdl);
    	}
    }
    else
    {
    	fprintf(stdout,"Device not found");
    }
    pci_detach(phdl);
    return EXIT_SUCCESS;
}

```
3. То что выводит программа:
	```
	VendorId:1022
	DeviceId:2000

	Device is exist!
	PCI_attach_device good!
	Recourses allocate successfully!

	SUMMARY DEVICE INFO:
	Device_ID = 0x2000
	Vendor_ID = 0x1022
	Bus Number = 0x0
	Device Function = 0x18
	Device Class = 0x20000
	Device IRQ = 0x9(134510924)
	MSI REG = 0x0
	CPU base address = 0x80479b4
	PCI base address = 0x8047984
	```
06.08.2021
Сегодня сидел просто статьи читал, в целом что-то стало яснее, по крайней мере в какую сторону копать.
1. https://habr.com/ru/post/211751/
2. https://tldp.org/LDP/lkmpg/2.6/html/lkmpg.html#AEN121
3. http://forum.kpda.ru/index.php/topic,1546.0.html