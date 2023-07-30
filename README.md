# PIMTPCH

## to run

After getting everything install (instructions below), you can run the program!
```bash
cd <INSTALL PATH>
PROJECT=<INSTALL PATH> make -j
sh tpch.sh -t <TEST> -q <QUERY> -i <ITERATIONS>
```
set test as ```upmem``` or ```cpu```

set query as the query number to run.

set iterations to the number of iterations

for more help ```sh tpch.sh -h```

## datasets
The datasets below follow this standard
https://www.tpc.org/TPC_Documents_Current_Versions/pdf/TPC-H_v3.0.1.pdf

### download instructions
```bash

cd <INSTALL PATH>
wget --no-check-certificate https://cs.brown.edu/people/acrotty/tpch/tpch-sf1.tar.gz
tar -xf tpch-sf1.tar.gz 
rm tpch-sf1.tar.gz 

wget --no-check-certificate https://cs.brown.edu/people/acrotty/tpch/tpch-sf10.tar.gz
tar -xf tpch-sf10.tar.gz
rm tpch-sf10.tar.gz

```

## compiler and environment
This repo utilises ```Clang16++``` and UPMEM SDK.

### Clang+LLVM download

https://github.com/llvm/llvm-project/releases/tag/llvmorg-16.0.0

To replicate our Ubuntu install of Clang+LLVM16
```bash
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-16.0.0/clang+llvm-16.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
tar -xf clang+llvm-16.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz 

# add the following to your bashrc
export PATH="$HOME/clang+llvm-16.0.0-x86_64-linux-gnu-ubuntu-18.04/bin:$PATH"
```


### UPMEM SDK download

https://sdk.upmem.com/

To replicate our UPMEM SDK install
```bash
wget http://sdk-releases.upmem.com/2023.2.0/ubuntu_20.04/upmem-2023.2.0-Linux-x86_64.tar.gz
tar -xf upmem-2023.2.0-Linux-x86_64.tar.gz 
cd upmem-2023.2.0-Linux-x86_64
source upmem_env.sh
```

