rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
./aes-pdelta -g
./aes-pdelta -i pdelta.txt -o key.txt --echo 1
