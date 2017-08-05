#!/bin/bash

echo -n "Obteniendo datos..."
ST_DECO=`grep -n "// START DECODER" $1 | cut -d: -f1`
LINS=`wc -l $1 | tr -d ' ' | cut -d/ -f1`
echo "[OK]"

echo -n "Generando CODER...."
head -$((${ST_DECO} - 1)) $1 > $2
echo "[OK]"

echo -n "Generando DECO....."
tail -$((${LINS} + 1 - ${ST_DECO})) $1 > $3
echo "[OK]"

echo "Comparando..."
diff $2 $3 | head -50
