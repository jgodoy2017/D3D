/*int[2] proxToXY(int prox, int ancho){

int x=prox%ancho;
int y=prox/ancho;

return [x,y];
}*/

int XYToProx(int x,int y,int ancho){

int prox=y*ancho+x;

return prox;
}
