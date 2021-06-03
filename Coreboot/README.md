# Конфигуратор на основе Coreboot
Промежуточный отчёт по проделанной работе 03.02.2021

## Шаг I. Установка QEMU
Для отладки созданного конфигуратора будет использоваться виртуальная машина, запущенная на Qemu. Для установки **QEMU** необходимо выполнить следующую команду: 
```
sudo apt install qemu-kvm qemu
```

## Шаг II. Сборка и настрйока тестого образа coreboot + coreinfo
Установим необходимые библиотеки и утилиты для сборки и скопируем **coreboot** из репозитория: 

```
sudo apt-get install -y bison build-essential curl flex git gnat-5 libncurses5-dev m4 zlib1g-dev
git clone https://review.coreboot.org/coreboot
cd coreboot
```
Далее необходимо сбрать утилиты coreboot и полезную нагрузку(payload), утилиту Coreinfo, она нужна для вывода информации о системе (CPU, PCI и тд).

```
make crossgcc-i386 CPUS=$(nproc)
make -C payloads/coreinfo olddefconfig
make -C payloads/coreinfo
```
Теперь сконфигурируем mainboard, для этого выполним команду:
```
make menuconfig
```
Нужно убедится что мы собираем coreboot под виртуальную машину QEMU, для этого во вкладке Mainboard, следует убедится что в меню **Mainboard**:
1. В пункте **Vendor** указан параметр - **Emulation**
2. В пункте **Model** узазанно значение - **QEMU x86 i440fx/piix4**
3. В меню payload выбрать пункт **An Elf executable payload** и указать путь к elf файлу **payloads/coreinfo/build/coreinfo.elf**
4. Выйти из конфигуратора, сохранив введённые изменения

Собираем образ coreboot и запускаем его в QEMU:
``` 
make  
qemu-system-x86_64 -bios build/coreboot.rom 
```
![Image alt](coreinfo.png)

## Шаг III. Добавление SeaBIOS и второй "полезной нагрузки" - nvramcui
Снова используем **make menuconfig**, и аналогично меняем payload с Coreinfo на **SeaBIOS**, так же в пункте **Second Payloud** выбираем, **nvramcui**. Теперь при выборе загрузочного диска есть возможность включить nvramcui, на базе которого будет писаться наш конфигуратор. Аналогично как и в шаге II собираем coreboot. Результат: 
![Image alt](coreinfo_second.png)

## Шаг IV. Библиотека ncurses
Для более глубокого понимания того как работает ncurses был просмотрен цикл роликов на youtube и написанно несколько тестовых программ с реализацией многовариантного меню.
1. Все написанные исходники хранятся в папке ncurses   
2. Плейлист с гайдами: [![Watch the video](https://img.youtube.com/vi/lV-OPQhPvSM/maxresdefault.jpg)](https://youtu.be/lV-OPQhPvSM)
3. Итоговый гуй я ещё не нарисовал, в ближайшее время сделаю
  

## Шаг IV. Добавление собственных payload-ов
Исходный код тестового самодельного payload:
```
#include <libpayload.h> // библиотека необходимая для всех payloud
 
int main(void)
{
    printf("Hello, world!\n"); 
    halt();
    return 0;
}
```
Описание в Makefile:
```
unexport $(COREBOOT_EXPORTS)

ARCH = x86_32
OBJS = $(obj)/test.o
TARGET = $(obj)/test.elf

all: real-all

include ../libpayload/Makefile.payload

real-all: $(TARGET)

.PHONY: all real-all

```
### Добавление первчиных payloads
Для добавление первичного payloud достаточно при вызове ```make menuconfig``` указать в качестве **payloud** путь к необходимому **.elf** файлу.
![Image alt](elf.png)
![Image alt](elf1.png)
После этого сохраняем изменения и собираем командой ```make```. В результате при запуске qemu мы должны увидеть следующее:
![Image alt](elf2.png)
### Добавление вторичных payloads
Для добавления вторичных payloud необходимо внести ряд изменений в файлы Makefile.inc и Kconfig, лежащих в папке payloads. Далее приведены добавленные описания в этих файлах:

1. Makefile.inc
```
cbfs-files-$(CONFIG_TEST_SECONDARY_PAYLOAD) += img/test
img/test-file := payloads/test/build/test.elf
img/test-type := payload
img/test-compression := $(CBFS_SECONDARY_PAYLOAD_COMPRESS_FLAG)

payloads/test \

payloads/test/build/test.elf test: export CCACHE := $(CCACHE)
payloads/test/build/test.elf test: force-payload
	$(MAKE) -C payloads/test

```
2. Kconfig

```
config TEST_SECONDARY_PAYLOAD
	bool "Load test as a secondary payload"
	default n
	depends on ARCH_X86
	help
	  test can be loaded as a secondary payload under SeaBIOS, GRUB,
	  or any other payload that can load additional payloads.
```

Теперь в make menuconfig можно установить тестовый payloud в качестве "второго".
![Image alt](second.png)
В качестве "первого" укажем SeaBIOS. Итоговый результат после сборки:
![Image alt](second1.png)

## Шаг V. Изучение принципов работы bayou и nvramcui
Сейчас я работаю над разбором комментированием исходного кода nvramcui и bayou. Что понял и какие есть вопросы:
1. Тут происходит выгрузка данных из CMOS. 
    ```
    lib_get_sysinfo(); // Функция из payloads/libpayload/arch/i386/sysinfo.c. Получает информацию из coreboot tables, если они существуют (ret = get_coreboot_info(&lib_sysinfo);)

	struct cb_cmos_option_table *opttbl = get_system_option_table();//Загружается таблица системных настроек

	if (opttbl == NULL) {
		printf("Could not find coreboot option table.\n");
		halt();
	} // Если их нет то всё, иначе готовим структуру *option к выгрузке параметров из таблиц

	/* prep CMOS layout into libcurses data structures */

	struct cb_cmos_entries *option = first_cmos_entry(opttbl);
	int numopts = 0;
	int maxlength = 0;

    // После выгружки параметров пересчитываем их и находим самый длинный по названию параметр, я так понял, чтобы корректно рисовать ncurses-ом GUI.

	count_cmos_options(option, &numopts, &maxlength);
    ```
2. Далее проихсодит настройка цветов и создание окон для ncurses, после чего следует блок кода, отвечающий за "перекрашивание" элемента меню при наведении "курсора", а так же за вывод других значений выбранного параметра (перелистывание параметров при нажатии на стрелки)
    ```
    done = 0;
        while (!done) {
            render_form(form);
            ch = getch(); // ловим нажатую пользователем клавишу
            if (ch == ERR) 
                continue;
            switch (ch) {
            case KEY_DOWN:
                form_driver(form, REQ_NEXT_FIELD);
                break;
            case KEY_UP:
                form_driver(form, REQ_PREV_FIELD);
                break;
            case KEY_LEFT:
                if (field_type(current_field(form)) == TYPE_ENUM) {
                    form_driver(form, REQ_PREV_CHOICE);
                } else {
                    form_driver(form, REQ_LEFT_CHAR);
                }
                break;
            case KEY_RIGHT:
                if (field_type(current_field(form)) == TYPE_ENUM) {
                    form_driver(form, REQ_NEXT_CHOICE);
                } else {
                    form_driver(form, REQ_RIGHT_CHAR);
                }
                break;
            case KEY_BACKSPACE:
            case '\b':
                form_driver(form, REQ_DEL_PREV);
                break;
            case KEY_DC:
                form_driver(form, REQ_DEL_CHAR);
                break;
            case KEY_F(1):
                done = 1;
                break;
            default:
                form_driver(form, ch);
                break;
            }
        }
    ```
    Для управления полями меню используется функция form_driver(форма, передаваемый параметр/флаг)
3. Не до конца понимаю что происходит вот тут, модификация полей меню, но что-то как-то хитро немного. 
    ```
	for (i = 0; i < numopts; i++) {
		char *name = field_buffer(fields[2 * i], 0);
		char *value = field_buffer(fields[2 * i + 1], 0);
		char *ptr;
		for (ptr = value + strlen(value) - 1;
		     ptr >= value && *ptr == ' '; ptr--)
			;
		ptr[1] = '\0';
		set_option_from_string(use_nvram, opttbl, value, name);
	}
    ```
4. Из последних версий coreboot из дефолтной поставки, видимо, выпилили **bayou**, поэтому для работы с ним нужно скачать более старую версию coreboot с этой страницы - https://www.coreboot.org/downloads.html. По сути bayou ничгео не умеет, кроме запуска "спаршенных" payload-ов из CBFS, но в тонкостях того как он это делает я ещё не разобрался до конца, думаю через 2-3 дня разберусь точно.

5. У меня есть вопрос что такое BootMedium, гугл ничего не говорит, wiki по coreboot такого определения тоже не знает, поэтому я не могу понять что это.

6. Какой итоговый конфигуратор должен получится? Я так понимаю, что он должен уметь редактирвоать параметры из nvramcui, иметь возможность загружать после себя дополнительные payload, как bayou, а так же устанавливать порядок загрузки жёстких дисков(или не дисков, или не жёстких), я правильно всё понимаю?  


Поменять tinyCurses на mdСurses (лоховской на поцанский)

seabios -> post.c -> maininit -> interactive_bootmenu();
coreinfo -> coreinfo.c -> print_submenu();
pci_moudle -> Пример корректной работы форм




15.04.2021
1. home/vxuser/coreboot/payloads/external/SeaBIOS/seabios/src/post.c maininit(void) -> interface_init()

2. interface_init(void) -> boot_init();

3. home/vxuser/coreboot/payloads/external/SeaBIOS/seabios/src/boot.c boot_init(void) -> loadBoorOrder()

4. loadBootOrder - загружает bootmedium

5. Для нахождения доступных методов загрузки нужно смотреть лог запуска SeaBios, в частности строки Searching Bootorder for <pci@.....>

6. Включение отладки для получения этого лога
    ```
    sudo apt install python3-pip
    pip3 install pyserial
    sudo readserial.py /dev/ttyS0 115200
    mkfifo qemudebugpipe
    make menuconfig # debug -> debug_level = 8

    qemu -chardev pipe,path=qemudebugpipe,id=seabios -device isa-debugcon,iobase=0x402,chardev=seabios ...
    qemu-system-x86_64 -bios build/coreboot.rom

    ./readserial.py -nf qemudebugpipe
    ```


29.04.2021
1. План поменялся, никакие отладчики не будем юзать, они конченные. Отлаживать с помощью какого-нибудь флопика, надо его добавить

2. Редактировать будем cmos.layout, там нужно описать состояния и зарезервировать место для хранения параметров в cmos.

3. Редактировать будем файл /home/vxuser/coreboot/payloads/external/SeaBIOS/seabios/src/boot.c 

4. В функции interactive_bootmenu(void) нужно убрать отображение вариантов загрузки, оставив только функцию входа в конфигурационное меню и подпись, с какого сейчас мы грузимся устройства.

5. Сейчас нужно плюнуть что-то в CMOS из конфигуратора и поймать это из сибиоса и через printf вывести.

6. cb_cmos_entries в файле payloads/libpayload/include/coreboot_tables.h

7. Для cmos.layout  **(700     20  r   1   boot_medium)**. Если добавить эти строки в cmos.layout, то вылетает ошибка Unaligned CMOS option table entry boot_medium spans multiple bytes. src/arch/x86/Makefile.inc:25: recipe for target 'build/option_table.h' failed