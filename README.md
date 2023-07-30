# PIMTPCH

## to run

After getting everything installed (instructions below), you can run the program!
```bash
cd <INSTALL PATH>
PROJECT=<INSTALL PATH> make -j
sh tpch.sh -t <TEST> -q <QUERY> -i <ITERATIONS>
```

set ```<TEST>``` as ```upmem``` or ```cpu``` <br> 
set ```<QUERY>``` as the query number to run  <br>
set ```<ITERATIONS>``` to the number of iterations <br>


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
This repo utilises ```Clang16++``` and UPMEM SDK. <br>
The downloaded Clang16 needs to be loaded at a higher level than the UPMEM SDK. <br>
If you run into compilation errors, (1) load the UPMEM SDK env file. (2) add Clang16 to your path. <br>
```bash
source $HOME/upmem-2023.2.0-Linux-x86_64/upmem_env.sh # (1)
export PATH=$HOME/clang+llvm-16.0.0-x86_64-linux-gnu-ubuntu-18.04/bin:$PATH # (2)
```

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

https://sdk.upmem.com/ <br>

To replicate our UPMEM SDK install
```bash
wget http://sdk-releases.upmem.com/2023.2.0/ubuntu_<OS VERSION>/upmem-2023.2.0-Linux-x86_64.tar.gz
tar -xf upmem-2023.2.0-Linux-x86_64.tar.gz 
cd upmem-2023.2.0-Linux-x86_64
source upmem_env.sh
```
Make sure to match your OS version! <br>


