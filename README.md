## Toward Resource-Efficient Packet Header Vector Encoding on Programmable Switches

The programmable switch offers a limited capacity of packet header vector (PHV) words that store packet header fields and metadata fields defined by network functions. However, existing switch compilers employ inefficient strategies of encoding fields on PHV words. Their encoding wastes scarce PHV words and may result in failures when deploying network functions. In this paper, we propose Melody, a new framework that reuses PHV words for as many fields as possible to achieve resource-efficient PHV encoding. Melody offers a field analyzer and an optimization framework. The analyzer identifies which fields can reuse PHV words while preserving the original packet processing logic. The framework integrates analysis results into its encoding to offer the resource-optimal decisions. 

This work has been accepted by IEEE INFOCOM 2023. This is a joint work by Zhejiang University, Peking University, Fuzhou University, Yangzhou University, and Southeast University. This repo provides the source code of Melody that targets BMv2 software switches. 

The name, Melody, refers to [the famous song written by David Zee Tao](https://www.youtube.com/watch?v=yEtU5tzZMkY&ab_channel=TimelessMusic). 

### Tutorial

First, install p4c on a server. See https://github.com/p4lang/p4c.

Second, download this repo and cd to this directory. 

Third, create a folder:

```
mkdir build
```

Fourth, run the following command to compile your P4 program:

```shell
p4c-bm2-ss --p4v 16 --p4runtime-files build/<name>.p4.p4info.txt -o build/<name>.json <name>.p4
```

For example:

```
p4c-bm2-ss --p4v 16 --p4runtime-files build/basic.p4.p4info.txt -o build/basic.json basic.p4
```

Fifth, generate dot files using p4c:

```shell
mkdir out
p4c-graphs <prog.p4> --graphs-dir out     
cd out    
dot <name>.dot -Tpng > <name>.png 
```

Finally, modify the paths in melody.py and run melody.py to obtain the deployment decisions.

