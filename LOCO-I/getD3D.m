function tam=getD3D(vals)
    esp=' ';

    bin='./d3d2.sh ';  % Nombre del script wrapper.
    imagen='/Users/juan/Desktop/pgm8bit/us_28348.pgm';
%    imagen='/Users/juan/Desktop/atipi/baloons.pgm';
    Nmax=num2str(vals(1));  % Nmax corre por potencias de 2.
    T1=num2str(vals(2));
    T2=num2str(vals(3));
    T3=num2str(vals(4));
    RESET=num2str(vals(5)); % RESET corre por potencias de 2.
    comando=[bin imagen esp Nmax esp T1 esp T2 esp T3 esp RESET];
    
    % Ejecuto el wrapper del binario nuestro.
    [~, cmdout]=system(comando);
    
    % Escapo posibles bugs, para que el ga() no corte.
    if(strcmp(cmdout(1:2),'./'))
        tam=9999999;
    else
        tam=str2double(cmdout);
    end
    
    % Muestro la informacion actual en pantalla.
    msg=sprintf('%s\t%s\t%s\t%s\t%s\t%d', Nmax, T1, T2, T3, RESET, tam);
    disp(msg)
    
    % Exporto los valores al entorno.
    assignin('base', 'vals', [str2double(Nmax) str2double(T1) str2double(T2) str2double(T3) str2double(RESET) tam]);
    evalin('base', 'reporte=[reporte; vals];');
end
