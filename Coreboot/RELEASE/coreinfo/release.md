# Конфигуратор для coreboot и SeaBIOS на основе Coreinfo и nvramcui

### Пейлоад был создан на базе утилиты coreinfo, список добавленных(NEW) и отредактированных(UPDT) файлов:

1. **configurator_module.c (NEW)** - дополнительное окно в coreinfo для настройки параметров загрузки (F3). 

2. **bootorder_module.c (NEW)** - ещё одно дополнительное окно в coreinfo для настройки приоритета загрузки (F4). 

3. **coreinfo.c (UPDT)** - были изменены цвета (в функции main), добавлены описания для двух 
дополнительных окон, категории Configurator и  Bootorder. Модули были добавлены на место удалённого nvram_module.

4. **Makefile (UPDT)** - добавлены объекты для новых окон(bootorder_module.o configurator_module.o)

5. **TinyCURSES** заменён на **PDCurses**

6. Изменён **cmos.layout** и **cmos.default**, добавлены поля для задания приоритетов
    ```
    600 2   h   0   CFG_bootorder_HD
    602 2   h   0   CFG_bootorder_USB
    604 2   h   0   CFG_bootorder_PXE
    ```
    В default прописанны по умолчанию следующие значения 
    ```
    CFG_bootorder_HD=1
    CFG_bootorder_USB=2
    CFG_bootorder_PXE=3
    ```
7. Из coreinfo удалён модуль: **nvram.module**, он выводил дамп памяти NVRAM.