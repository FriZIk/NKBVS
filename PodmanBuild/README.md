# YoctoBuild
Пошаговое руководство по сборке специализированного Linux дистрибутива основанного на Yocto Project, а так же запуску созданного образа на виртуальной машине Qemu под x86 архитектурой, с возможностью запуска образов Docker средствами Podman.
### Шаг I. Установка необходимых зависимостей для сборки Yocto
Для сборки дистрибутива требуется большое количество зависимостей. Существует два подхода к созданию необходимой программной инфраструктуры.
1. В лоб. Заключается в установке всех необходимых утилит непосредственно в свою Linux систему. Список программ (для Debian): 
```
sudo apt-get install sed wget subversion git-core coreutils unzip texi2html texinfo libsdl1.2-dev docbook-utils fop gawk python-pysqlite2 diffstat make gcc build-essential xsltproc g++ desktop-file-utils chrpath libgl1-mesa-dev libglu1-mesa-dev autoconf automake groff libtool xterm libxml-parser-perl1
```

2. Используя Docker и контейнеризацию. Такой подход является более цивилизованным и предпочтительным. Чтобы установить рабочее окружение необходимо выполнить команду:
```
docker run --rm -it -v "путь к предпочитаемой папке":/workdir crops/poky --workdir=/workdir
```

После ввода этой команды будет запущена среда с уже установленными зависимостями для сборки yocto.
### Шаг II. Загрузка инструментов YoctoProject
Клонируем репозиторий Yocto Project командой: 

```git clone -b dunfell git://git.yoctoproject.org/poky.git```

Далее переходим в папку poky ```cd poky```. Теперь нужно запустить специальный сценарий, который запустит окружение сборки и перенесёт нас в папку build. Для этого используется команда ```source oe-init-build-env```.
### Шаг III. Редактирование **local.conf** и bblayers.conf сборки
1. В папке **build/conf** храниться главный конфигурационный файл сборки **local.conf**, редактируя его можно выбрать под какую платформу собрать проект (мы будем собирать под **qemux86-64**, он выбран по умолчанию). 
2. Так же необходимо добавить параметр ```"DISTRO_FEATURES_append = " virtualization"``` для поддержки виртуализации, а так же дополнительные пакеты (сам podman и необходимые для работы утилиты ```IMAGE_INSTALL_append = " tunctl cni util-linux podman podman-compose crun cgroup-lite rng-tools procps ca-certificates python3-setuptools python3-pyyaml python3-json" ```

3. Для установки всех этих утилит необходимо так же отредактировать конфиг **bblayers.conf**. Предварительно в директорию **poky** нужно склонировать ряд дополнительных модулей, а именно: ```meta-openembedded,meta-virtualization,meta-security ```. Обратите внимание, версия модулей и дистрибутива Yocto должна совпадать. Клониурем:
```
git clone -b dunfell git://git.openembedded.org/meta-openembedded && \
git clone -b dunfell git://git.yoctoproject.org/meta-virtualization && \
git clone -b dunfell git://git.yoctoproject.org/meta-security
```
4. Приводим файл bblayers.conf к виду
```
POKY_BBLAYERS_CONF_VERSION = "2"
BBPATH = "${TOPDIR}"
BBFILES ?= ""

BBLAYERS ?= " \
  /workdir/poky/meta \
  /workdir/poky/meta-poky \
  /workdir/poky/meta-yocto-bsp \
  /workdir/poky/meta-openembedded/meta-oe \
  /workdir/poky/meta-openembedded/meta-filesystems \
  /workdir/poky/meta-openembedded/meta-python \
  /workdir/poky/poky/meta-openembedded/meta-networking \
  /workdir/poky/meta-openembedded/meta-perl \
  /workdir/poky/meta-virtualization \
  /workdir/poky/meta-security \"
```
Сохраняем изменния, осталось попросить систему сборки **bitbake** собрать наш образ. 
### Шаг IV. Сборка образа с помощью Bitbake
**Bitbake** скачивает необходимые программы и утилиты, собирает их и добавляет в билд системы. Чтобы запустить его для сборку образа необходимо выполнить команду: 
```
bitbake core-image-minimal
```
После окончания сборки в каталоге **build/tmp/deploy/images** будет находиться готовый образ.
### Шаг V. Запуск созданного образа в эмуляторе Qemu
Для его запуска нужно в директории **build** запустить команду 
```
qemurun qemux86-64
```
Откроется новое окно с эмулятором Qemu, запускающим ОС. Имя пользователя **root**, пароль пустой.