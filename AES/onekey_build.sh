rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
./aes-attack -g
./aes-attack -i pdelta.txt -o key.txt --echo 1
cat secret.txt
cat key.txt
