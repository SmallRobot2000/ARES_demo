gcc bmp2spr.c -o bmp2spr $(pkg-config --cflags --libs libbmp)
gcc bmp2b0.c -o bmp2b0 $(pkg-config --cflags --libs libbmp)
gcc bmp2til.c -o bmp2til $(pkg-config --cflags --libs libbmp)