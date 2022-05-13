From ubuntu:18.04

WORKDIR /usr/app

RUN apt update
RUN apt install -yqq wget
RUN apt install -yqq git
RUN apt install -yqq cmake
RUN apt install -yqq pkg-config m4 libtool automake autoconf
RUN apt install -yqq libboost-all-dev
RUN wget https://download.libsodium.org/libsodium/releases/LATEST.tar.gz \
        ; tar -xvf LATEST.tar.gz \
        ; cd libsodium-stable/ \
        ; ./configure \
        ; make && make check \
        ; make install \
        ; ldconfig \
        ; cd .. \
        ; rm -rf LATEST.tar.gz libsodium-stable \
        ; git clone https://github.com/zeromq/libzmq.git \
        ; cd libzmq \
        ; ./autogen.sh \
        ; ./configure -with-libsodium && make \
        ; make install  \
        ; ldconfig \
        ; cd .. \
        ; rm -rf libzmq  \
        ; git clone https://github.com/zeromq/zmqpp.git \
        ; cd zmqpp \
        ; make \
        ; make install \
        ; cd .. \
        ; rm -rf zmqpp \
        ; git clone https://github.com/MoyuST/simpleStateMachine.git
