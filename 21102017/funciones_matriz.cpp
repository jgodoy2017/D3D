int[2] proxToXY(prox,ancho){

int x=prox%ancho;
int y=prox/ancho;

return [x,y];
}

int XYToProx(x,y,ancho){

int prox=y*ancho+x;

return prox;
}
