TomatoViewer - Versión independiente
---------------------------------------------------
* Descripción:
  Visor de imágenes en formato PGM, monocromático de 8 o 16 bits por pixel.

* Instalación:
  1. Para que el programa pueda funcionar, primero es necesario instalar en el sistema los entornos de trabajo y desarrollo X11, un emulador de terminal gráfica (sugerimos xterm):

  Por ejemplo, para CentOS/Red Hat:

  # yum install libX11-devel
  # yum install xorg-x11-devel
  # yum install xorg-x11-utils
  # yum install xorg-x11-fonts*
  # yum groupinstall "X Window System"
  # yum install xorg-x11-apps
  # yum install xterm

  2. Para poder interactuar con el ambiente gráfico, si se tiene un entorno de usuario solo texto, se puede instalar un servidor VNC:

  # yum install vnc-server

  3. La configuración del VNC server se puede hacer (en CentOS 7.x) creando el archivo /etc/systemd/system/vncserver@:0.service con el siguiente contenido:

  [Unit]
  Description=Remote desktop service (VNC)
  After=syslog.target network.target

  [Service]
  Type=forking
  User=root

  # Clean any existing files in /tmp/.X11-unix environment
  ExecStartPre=-/usr/bin/vncserver -kill %i
  ExecStart=/usr/bin/vncserver %i
  PIDFile=/root/.vnc/%H%i.pid
  ExecStop=-/usr/bin/vncserver -kill %i

  [Install]
  WantedBy=multi-user.target

  4. Luego se ejecuta lo siguiente, que por única vez, pedirá crear una contraseña para proteger la pantalla (observar que el nombre server1.example.com debe ser sustituido por el nombre o dirección IP del servidor donde estamos trabajando):

  # vncserver
  You will require a password to access your desktops.

  Password: ********
  Verify: ********
  xauth:  file /root/.Xauthority does not exist

  New 'server1.example.com:1 (root)' desktop is server1.example.com:1

  Creating default startup script /root/.vnc/xstartup
  Starting applications specified in /root/.vnc/xstartup
  Log file is /root/.vnc/server1.example.com:1.log

  El comando anterior nos va a asignar un número de pantalla (en este caso ":1").
  
  5. Para conectarnos a la pantalla, necesitamos tener en nuestro equipo instalado el VNC Viewer (por ejemplo RealVNC: https://www.realvnc.com/es/), y abrir una conexión a la siguiente dirección:

    server1.example.com:5901           (se le suma 5900 al número de pantalla obtenido).

  Se nos pedirá la contraseña creada en el punto anterior. (!)

  6. Si tenemos instalado el xterm, podemos levantar una terminal de comandos dentro de la ventana gráfica, invocándola desde la terminal solo texto habitual. Esto nos independizará de la terminal de texto y pasaremos a trabajar exclusivamente en el entorno gráfico.
  
  El procedimiento es el siguiente:

    # export DISPLAY=:1.0               (:1 es la pantalla que se nos asignó antes)
    # xterm &

  Esto nos abre un emulador de terminal en el ambiente gráfico, desde el que podremos ejecutar el visor.

  7. Para compilar el programa, en el directorio donde se copió el .tar.gz, ejecutamos (da igual si es desde el entorno gráfico o no):

    # gunzip tomatoViewer_standalone.tar.gz
    # tar xvf tomatoViewer_standalone.tar
    # cd tomatoViewer/
    # gcc -o tomatoViewer -std=c++11 -g -lX11 -lstdc++ *.cc

  Esto generará el ejecutable "tomatoViewer".

* Uso:
  TomatoViewer está pensado para imágenes extraídas de un DICOM multiframe.
  Si queremos visualizar un conjunto de imágenes que representan los frames del DICOM original, debemos tener cada frame como archivo PGM.
  El nombre de cada uno de estos archivos debe tener un prefijo común y después un número que indica el número de frame, y la extensión .pgm.
  En la invocación del visor, se deben pasar como parámetros:

  1. El prefijo común al nombre de archivo de todos los frames.
  2. El número de frame inicial a visualizar.
  3. El número de frame final a visualizar.

  Por ejemplo, para visualizar los siguientes archivos...

    multi16bits_.f1.pgm   multi16bits_.f14.pgm  multi16bits_.f26.pgm  multi16bits_.f42.pgm  multi16bits_.f48.pgm
    multi16bits_.f2.pgm   multi16bits_.f13.pgm  multi16bits_.f25.pgm  multi16bits_.f41.pgm  multi16bits_.f57.pgm
    multi16bits_.f3.pgm   multi16bits_.f17.pgm  multi16bits_.f24.pgm  multi16bits_.f40.pgm  multi16bits_.f56.pgm
    multi16bits_.f4.pgm   multi16bits_.f16.pgm  multi16bits_.f23.pgm  multi16bits_.f39.pgm  multi16bits_.f55.pgm
    multi16bits_.f6.pgm   multi16bits_.f15.pgm  multi16bits_.f22.pgm  multi16bits_.f38.pgm  multi16bits_.f54.pgm
    multi16bits_.f5.pgm   multi16bits_.f19.pgm  multi16bits_.f21.pgm  multi16bits_.f37.pgm  multi16bits_.f53.pgm
    multi16bits_.f7.pgm   multi16bits_.f18.pgm  multi16bits_.f20.pgm  multi16bits_.f36.pgm  multi16bits_.f52.pgm
    multi16bits_.f9.pgm   multi16bits_.f31.pgm  multi16bits_.f32.pgm  multi16bits_.f35.pgm  multi16bits_.f51.pgm
    multi16bits_.f8.pgm   multi16bits_.f30.pgm  multi16bits_.f33.pgm  multi16bits_.f45.pgm  multi16bits_.f50.pgm
    multi16bits_.f11.pgm  multi16bits_.f29.pgm  multi16bits_.f34.pgm  multi16bits_.f47.pgm
    multi16bits_.f10.pgm  multi16bits_.f28.pgm  multi16bits_.f44.pgm  multi16bits_.f46.pgm

  ...necesitamos ejecutar el siguiente comando:

    # ./tomatoViewer multi16bits_.f 1 57

  Si queremos visualizar una única imagen, igual debe cumplir con esta nomenclatura.
  Por ejemplo, para mostrar el siguiente archivo...

    single16bits1.pgm

  ...necesitamos ejecutar el siguiente comando:

    # ./tomatoViewer single16bits 1 1

  El visor responde a las flechas en el teclado para avanzar y retroceder de frame, indicando siempre sobre la imágen el número de frame que se está mostrando, y el total de frames.
  Con la tecla "q" se sale del programa en cualquier momento.
