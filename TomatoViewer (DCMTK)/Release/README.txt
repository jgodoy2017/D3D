TomatoViewer - Versión para DCMTK
---------------------------------------------------
* Descripción:
  Visor de imágenes en formato DCM.
  Las imágenes podrán estar comprimidas con el codec D3D o descomprimidas.
  Las imágenes deberán ser monocromáticas, y podrán ser de 8 o 16 bits por pixel, simple o múltiple frame.

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

    # gunzip tomatoViewer.tar.gz
    # tar xvf tomatoViewer.tar
    # ./build.sh

  Esto hará el build de todo el DCMTK, incluyendo el soporte para el codec D3D.
  El ejecutable del visor se encuentra (junto a otros ejecutables) en la siguiente ruta, relativa al .tar.gz:

    dcmtk-3.6.2/Release/bin/

* Uso:
  Para visualizar un archivo .dcm, simplemente se invoca el ejecutable y se le pasa como parámetro la ruta al archivo.
  Por ejemplo, para visualizar el archivo...

    multi16bits_encoded.dcm

  ...necesitamos ejecutar el siguiente comando:

    ./tomatoViewer multi16bits_encoded.dcm

  El visor responde a las flechas en el teclado para avanzar y retroceder de frame, indicando siempre sobre la imágen el número de frame que se está mostrando, y el total de frames.
  Con la tecla "q" se sale del programa en cualquier momento.
