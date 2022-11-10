../configure --enable-shared --arch=x86_64 --disable-optimizations --disable-x86asm
make clean
make -j8 && make install
