#!/bin/bash
echo "utnso" | sudo -S su &> /dev/null

echo "Installing dependencies…"

echo -n " • so-commons-library: downloading…"
git clone -q https://github.com/sisoputnfrba/so-commons-library
echo -ne "\r\e[0K • so-commons-library: installing…"
sudo make install -s -C so-commons-library &> /dev/null
sudo rm -rf so-commons-library
echo -e "\r\e[0K • so-commons-library: done."

echo -n " • ansisop-parser: downloading…"
git clone -q https://github.com/sisoputnfrba/ansisop-parser.git
echo -ne "\r\e[0K • ansisop-parser: installing…"
sudo make install -s -C ansisop-parser/parser
sudo rm -rf ansisop-parser
echo -e "\r\e[0K • ansisop-parser: done."

echo -n " • exec-helper: downloading…"
curl -s -L -o so.sh https://www.dropbox.com/s/h5wrtqem8rouxy3/so.sh?dl=1
echo -ne "\r\e[0K • exec-helper: installing…"
sed -i "3s|.*|PROJECT=\"$PWD\"|" so.sh
chmod +x so.sh; sudo mv so.sh /usr/local/bin/so
echo -e "\r\e[0K • exec-helper: done."

echo "Building modules…"
mkdir -p ~/git/tp-2017-1c-System-Buzz
cp -r Common ~/git/tp-2017-1c-System-Buzz

echo -n " • Terminal: building…"
sudo make all -s -C Consola/Debug &> /dev/null
echo -e "\r\e[0K • Terminal: done."
echo -n " • Processor: building…"
sudo make all -s -C CPU/Debug &> /dev/null
echo -e "\r\e[0K • Processor: done."
echo -n " • File System: building…"
sudo make all -s -C File_System/Debug &> /dev/null
echo -e "\r\e[0K • File System: done."
echo -n " • Kernel: building…"
sudo make all -s -C Kernel/Debug &> /dev/null
echo -e "\r\e[0K • Kernel: done."
echo -n " • Memory: building…"
sudo make all -s -C Memoria/Debug &> /dev/null
echo -e "\r\e[0K • Memory: done."

echo "Ready to run using 'so {module}'"
