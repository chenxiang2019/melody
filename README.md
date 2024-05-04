## Toward Resource-Efficient Packet Header Vector Encoding on Programmable Switches

The programmable switch offers a limited capacity of packet header vector (PHV) words that store packet header fields and metadata fields defined by network functions. However, existing switch compilers employ inefficient strategies of encoding fields on PHV words. Their encoding wastes scarce PHV words and may result in failures when deploying network functions. In this paper, we propose Melody, a new framework that reuses PHV words for as many fields as possible to achieve resource-efficient PHV encoding. Melody offers a field analyzer and an optimization framework. The analyzer identifies which fields can reuse PHV words while preserving the original packet processing logic. The framework integrates analysis results into its encoding to offer the resource-optimal decisions. 

This work has been accepted by IEEE INFOCOM 2023. This is a joint work by Zhejiang University, Peking University, Fuzhou University, Yangzhou University, and Southeast University. 

**This repo mainly provides the source code of Melody that targets BMv2 software switches.** If you want to apply the results of Melody to target switches, please check the Tofino-related paragraphs (marked by **[Tofino]**) in this readme.

The name, Melody, refers to [the famous song written by David Zee Tao](https://www.youtube.com/watch?v=yEtU5tzZMkY&ab_channel=TimelessMusic). 

## Overview 

The code has three parts: preprocessor, analyzer, and optimizer/heuristic. 

- **preprocessor** transforms the input data plane program in P4 into intermediate files.
- **analyzer**
   - It contains two C++ programs, parser.cpp and analyzer.cpp. Those programs read and parse intermediate files generated by the preprocessor. Also, they extract fields, lengths, as well as MAT execution dependencies and mutual exclusions from parsing results.
   - phv.txt describes the capacity of PHV words in the target switch architecture (e.g., Tofino). 
- **optimizer** includes two C++ programs, optimizer.cpp and heuristic.cpp, which respectively represent the two algorithms mentioned by the paper, MIP and heuristic.
- **melody.cpp** is the main program of Melody. 
- **run.sh** is the startup script of Melody. Users run this script with the paths pointing to a P4 program and phv.txt to activate Melody. 
- **libgurobi110.so** is the dynamic link library required for guanxi program running 

Input: 
- P4 program: example.p4
- Switch PHV Capacity: phv.txt

Output: 
- **heuristic_res.txt** and **ilp_res.txt**: the results of MIP and heuristic algorithms. 
- **output.txt**: the logs of Melody execution.

## Tutorial

### 3.1 Environment

Melody is tested on a Ubuntu 20.04 server. 

### 3.1.1 Gurobi Installation

Melody depends on Gurobi to realize its MIP solver.

1. Get a Gurobi academic license: [Gurobi Academic License Program: Free Licenses for Academics | Gurobi - Gurobi Optimization](https://www.gurobi.com/academia/academic-program-and-licenses/). You need to obtain the “Individual Academic License”.

2. Please download the Linux version of Gurobi. 

3. Install Gurobi C++ installation package and dynamic link library, add environment variables, and activate Gurobi. 

4. Add the dynamic link library to `/opt/gurobi1101/linux64/lib`, and add the header file to `/opt/gurobi1101/linux64/include`. 

```bash
tar xvfz gurobi_server11.0.1_linux64.tar.gz
export GUROBI_HOME="/opt/gurobi1101/linux64"
export PATH="${PATH}:${GUROBI_HOME}/bin"
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"
```

For more information about the installation process, see
[Setting up Gurobi with C++ in the Linux environment](https://forum.ysang.org/t/setting-up-gurobi-with-c-in-the-linux-environment/13)
[Linux Installation - Gurobi Optimization](https://www.gurobi.com/documentation/current/remoteservices/linux_installation.html)
[GUROBI OPTIMIZER QUICK START GUIDE ](https://www.gurobi.com/wp-content/plugins/hd_documentations/documentation/9.0/quickstart_linux.pdf)

5. For more information about Gurobi's C++ API, see 
[C++ API Overview - Gurobi Optimization](https://www.gurobi.com/documentation/current/refman/cpp_api_overview.html)

### 3.1.2 Graphviz + libgraph + libcgraph Installation

Melody depends on Graphviz + libgraph + libcgraph libraries to parse intermediate files.

```bash
sudo apt-get update 
sudo apt-get install build-essential
sudo apt-get install libsdl-image1.2 libsdl-image1.2-dev guile-1.8 guile-1.8-dev libart-2.0-dev libaudiofile-dev libesd0-dev libdirectfb-dev libdirectfb-extra libfreetype6-dev libxext-dev x11proto-xext-dev libfreetype6 libaa1 libaa1-dev libslang2-dev libasound2 libasound2-dev
sudo apt install graphviz
sudo apt-get -y install libcgraph6
```

Download libgraph from [libgraph-1.0.2.tar.gz](http://download.savannah.gnu.org/releases/libgraph/libgraph-1.0.2.tar.gz). 

Copy `libgraph-1.0.2.tar.gz` to the project folder of Melody.

Execute the following commands.

```bash
wget http://download.savannah.gnu.org/releases/libgraph/libgraph-1.0.2.tar.gz
cd libgraph-1.0.2
./configure --disable-guile
sudo make
sudo make install
sudo cp /usr/local/lib/libgraph.* /usr/lib+
```

For libgraph installation, see 
[how-to-install-libgraph-on-ubuntu](https://seitcg.blogspot.com/2015/03/how-to-install-libgraph-on-ubuntu.html)

For more information about fixing installation errors, see 
[trying-to-install-libgraph](https://askubuntu.com/questions/942863/trying-to-install-libgraph/995265#995265?newreg=98331645a2de4f689ce4b52526e552d0)

Note that the Makefile needs the-lcgraph option: 
[c - using graphviz as a libirary](https://www.coder.work/article/6799278)

### 3.1.3 p4c Installation  

For more information about p4c and bmv2 installation, see 
[https://github.com/p4lang/p4c](https://github.com/p4lang/p4c)

```bash
git clone --recursive https://github.com/p4lang/p4c.git
git submodule update --init --recursive
sudo apt-get install cmake g++ git automake libtool libgc-dev bison flex \
libfl-dev libboost-dev libboost-iostreams-dev \
libboost-graph-dev llvm pkg-config python3 python3-pip \
tcpdump

pip3 install --user -r requirements.txt
sudo apt-get install -y doxygen graphviz texlive-full
git checkout v3.25.3
pip3 install --user cmake==3.16.3
mkdir build
cd build
cmake .. <optional arguments>
make -j4
make -j4 check
sudo make install
```

### 3.1.4 g++ 

Our code is built with C++ 11 on Ubuntu 20.04 and requires g++. 

```bash
sudo apt-get install build-essential
```

### 3.2 Build and Configure 

### 3.2.1 Build Melody 

Go to the project directory and execute the following commands with root permissions. 

```bash
rm output.txt
make clean
make
```

### 3.2.2 Configure 

Specify the paths of the P4 program and phv.txt to run Melody.

```bash
bash run.sh ./example.p4 analyzer/phv.txt
```

## Step-by-Step Case Study

We use `example.p4` to explain how Melody works.

First, we write this program.

```bash
#include <core.p4>
#include <v1model.p4>

struct ingress_metadata_t {
    bit<32> flow_ipg;
    bit<13> flowlet_map_index;
    bit<16> flowlet_id;
    bit<32> flowlet_lasttime;
    bit<14> ecmp_offset;
    bit<32> nhop_ipv4;
}

header ethernet_t {
    bit<48> dstAddr;
    bit<48> srcAddr;
    bit<16> etherType;
}

header ipv4_t {
    bit<4>  version;
    bit<4>  ihl;
    bit<8>  diffserv;
    bit<16> totalLen;
    bit<16> identification;
    bit<3>  flags;
    bit<13> fragOffset;
    bit<8>  ttl;
    bit<8>  protocol;
    bit<16> hdrChecksum;
    bit<32> srcAddr;
    bit<32> dstAddr;
}

header tcp_t {
    bit<16> srcPort;
    bit<16> dstPort;
    bit<32> seqNo;
    bit<32> ackNo;
    bit<4>  dataOffset;
    bit<3>  res;
    bit<3>  ecn;
    bit<6>  ctrl;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgentPtr;
}

struct metadata {
    @name("ingress_metadata")
    ingress_metadata_t   ingress_metadata;
}

struct headers {
    @name("ethernet")
    ethernet_t ethernet;
    @name("ipv4")
    ipv4_t     ipv4;
    @name("tcp")
    tcp_t      tcp;
}

parser ParserImpl(packet_in packet, out headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("parse_ethernet") state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition select(hdr.ethernet.etherType) {
            16w0x800: parse_ipv4;
            default: accept;
        }
    }
    @name("parse_ipv4") state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition select(hdr.ipv4.protocol) {
            8w6: parse_tcp;
            default: accept;
        }
    }
    @name("parse_tcp") state parse_tcp {
        packet.extract(hdr.tcp);
        transition accept;
    }
    @name("start") state start {
        transition parse_ethernet;
    }
}

control egress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("rewrite_mac") action rewrite_mac(bit<48> smac) {
        hdr.ethernet.srcAddr = smac;
    }
    @name("_drop") action _drop() {
        mark_to_drop(standard_metadata);
    }
    @name("send_frame") table send_frame {
        actions = {
            rewrite_mac;
            _drop;
            NoAction;
        }
        key = {
            standard_metadata.egress_port: exact;
        }
        size = 256;
        default_action = NoAction();
    }
    apply {
        send_frame.apply();
    }
}

control ingress(inout headers hdr, inout metadata meta, inout standard_metadata_t standard_metadata) {
    @name("flowlet_id") register<bit<16>>(32w8192) flowlet_id;
    @name("flowlet_lasttime") register<bit<32>>(32w8192) flowlet_lasttime;
    @name("_drop") action _drop() {
        mark_to_drop(standard_metadata);
    }
    @name("set_ecmp_select") action set_ecmp_select(bit<8> ecmp_base, bit<8> ecmp_count) {
        hash(meta.ingress_metadata.ecmp_offset, HashAlgorithm.crc16, (bit<10>)ecmp_base, { hdr.ipv4.srcAddr, hdr.ipv4.dstAddr, hdr.ipv4.protocol, hdr.tcp.srcPort, hdr.tcp.dstPort, meta.ingress_metadata.flowlet_id }, (bit<20>)ecmp_count);
    }
    @name("set_nhop") action set_nhop(bit<32> nhop_ipv4, bit<9> port) {
        meta.ingress_metadata.nhop_ipv4 = nhop_ipv4;
        standard_metadata.egress_spec = port;
        hdr.ipv4.ttl = hdr.ipv4.ttl + 8w255;
    }
    @name("lookup_flowlet_map") action lookup_flowlet_map() {
        hash(meta.ingress_metadata.flowlet_map_index, HashAlgorithm.crc16, (bit<13>)0, { hdr.ipv4.srcAddr, hdr.ipv4.dstAddr, hdr.ipv4.protocol, hdr.tcp.srcPort, hdr.tcp.dstPort }, (bit<26>)13);
        flowlet_id.read(meta.ingress_metadata.flowlet_id, (bit<32>)meta.ingress_metadata.flowlet_map_index);
        meta.ingress_metadata.flow_ipg = (bit<32>)standard_metadata.ingress_global_timestamp;
        flowlet_lasttime.read(meta.ingress_metadata.flowlet_lasttime, (bit<32>)meta.ingress_metadata.flowlet_map_index);
        meta.ingress_metadata.flow_ipg = meta.ingress_metadata.flow_ipg - meta.ingress_metadata.flowlet_lasttime;
        flowlet_lasttime.write((bit<32>)meta.ingress_metadata.flowlet_map_index, (bit<32>)standard_metadata.ingress_global_timestamp);
    }
    @name("set_dmac") action set_dmac(bit<48> dmac) {
        hdr.ethernet.dstAddr = dmac;
    }
    @name("update_flowlet_id") action update_flowlet_id() {
        meta.ingress_metadata.flowlet_id = meta.ingress_metadata.flowlet_id + 16w1;
        flowlet_id.write((bit<32>)meta.ingress_metadata.flowlet_map_index, (bit<16>)meta.ingress_metadata.flowlet_id);
    }
    @name("ecmp_group") table ecmp_group {
        actions = {
            _drop;
            set_ecmp_select;
            NoAction;
        }
        key = {
            hdr.ipv4.dstAddr: lpm;
        }
        size = 1024;
        default_action = NoAction();
    }
    @name("ecmp_nhop") table ecmp_nhop {
        actions = {
            _drop;
            set_nhop;
            NoAction;
        }
        key = {
            meta.ingress_metadata.ecmp_offset: exact;
        }
        size = 16384;
        default_action = NoAction();
    }
    @name("flowlet") table flowlet {
        actions = {
            lookup_flowlet_map;
            NoAction;
        }
        default_action = NoAction();
    }
    @name("forward") table forward {
        actions = {
            set_dmac;
            _drop;
            NoAction;
        }
        key = {
            meta.ingress_metadata.nhop_ipv4: exact;
        }
        size = 512;
        default_action = NoAction();
    }
    @name("new_flowlet") table new_flowlet {
        actions = {
            update_flowlet_id;
            NoAction;
        }
        default_action = NoAction();
    }
    apply {
        @atomic {
            flowlet.apply();
            if (meta.ingress_metadata.flow_ipg > 32w50000)
                new_flowlet.apply();
        }
        ecmp_group.apply();
        ecmp_nhop.apply();
        forward.apply();
    }
}

control DeparserImpl(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.tcp);
    }
}

control verifyChecksum(inout headers hdr, inout metadata meta) {
    apply {
        verify_checksum(hdr.ipv4.isValid(), { hdr.ipv4.version, hdr.ipv4.ihl, hdr.ipv4.diffserv, hdr.ipv4.totalLen, hdr.ipv4.identification, hdr.ipv4.flags, hdr.ipv4.fragOffset, hdr.ipv4.ttl, hdr.ipv4.protocol, hdr.ipv4.srcAddr, hdr.ipv4.dstAddr }, hdr.ipv4.hdrChecksum, HashAlgorithm.csum16);
    }
}

control computeChecksum(inout headers hdr, inout metadata meta) {
    apply {
        update_checksum(hdr.ipv4.isValid(), { hdr.ipv4.version, hdr.ipv4.ihl, hdr.ipv4.diffserv, hdr.ipv4.totalLen, hdr.ipv4.identification, hdr.ipv4.flags, hdr.ipv4.fragOffset, hdr.ipv4.ttl, hdr.ipv4.protocol, hdr.ipv4.srcAddr, hdr.ipv4.dstAddr }, hdr.ipv4.hdrChecksum, HashAlgorithm.csum16);
    }
}

V1Switch(ParserImpl(), verifyChecksum(), ingress(), egress(), computeChecksum(), DeparserImpl()) main;
```

This program is a basic P4 program that implements flowlet switching.

Second, we write phv.txt describing the switch PHV capacity, e.g.

```bash
5 8
10 16
40 32
```

Each line has two numbers. The first number denotes the number of X-bit PHV words in each switch. The second number denotes the length of PHV words, i.e., X.

For example, `5 8` means the switch has 5 8-bit PHV words.

**Due to NDA, we do not give real numbers of Tofino. Please update these numbers based on Tofino specifications.**

Third, enter root and run Melody:

```bash
bash run.sh ./example.p4 analyzer/phv.txt
```

Fourth, we observe the decisions made by Melody.

Melody (MIP) yields:

```base
hdr.ethernet.dstAddr_1 (32bit) : 21 (32bit) 
hdr.ethernet.dstAddr_2 (16bit) : 11 (16bit) 
hdr.ipv4.dstAddr (32bit) : 20 (32bit) 
hdr.ipv4.protocol (8bit) : 2 (8bit) 
hdr.ipv4.srcAddr (32bit) : 19 (32bit) 
hdr.ipv4.ttl (8bit) : 1 (8bit) 
hdr.tcp.dstPort (16bit) : 10 (16bit) 
hdr.tcp.srcPort (16bit) : 9 (16bit) 
meta.ingress_metadata.ecmp_offset (14bit) : 8 (16bit) 
meta.ingress_metadata.flow_ipg (32bit) : 18 (32bit) 
meta.ingress_metadata.flowlet_id (16bit) : 7 (16bit) 
meta.ingress_metadata.flowlet_lasttime (32bit) : 17 (32bit) 
meta.ingress_metadata.flowlet_map_index (13bit) : 6 (16bit) 
meta.ingress_metadata.nhop_ipv4 (32bit) : 16 (32bit) 
```

The heuristic of Melody yields:

```bash
meta.ingress_metadata.flowlet_map_index (13bit): 6 (16bit)
meta.ingress_metadata.flowlet_lasttime (32bit): 17 (32bit)
meta.ingress_metadata.flow_ipg (32bit): 16 (32bit)
meta.ingress_metadata.flowlet_id (16bit): 16 (32bit)
meta.ingress_metadata.nhop_ipv4 (32bit): 16 (32bit)
hdr.ethernet.dstAddr_1 (32bit): 15 (32bit)
hdr.ipv4.dstAddr (32bit): 15 (32bit)
hdr.ipv4.protocol (8bit): 15 (32bit)
hdr.ipv4.srcAddr (32bit): 15 (32bit)
hdr.ipv4.ttl (8bit): 15 (32bit)
hdr.ethernet.dstAddr_2 (16bit): 5 (16bit)
hdr.tcp.dstPort (16bit): 5 (16bit)
hdr.tcp.srcPort (16bit): 5 (16bit)
meta.ingress_metadata.ecmp_offset (14bit): 5 (16bit)
```

We explain each line via an example: `meta.ingress_metadata.flowlet_map_index (13bit): 6 (16bit)`.

It means that Melody decides to place the field `meta.ingress_metadata.flowlet_map_index`, which is 13-bit, to the 6-th 16-bit PHV word.

**[Tofino]**

Applying our results to Tofino has three steps.

**First, please refer to the Tofino specification (Compiler User Guide). In the specification, users can use target-specific programming instructions to guide PHV encoding during program compilation, e.g.,**
`[Instruction Name] meta.ingress_metadata.flowlet_map_index 6` **means encoding the field "meta.ingress_metadata.flowlet_map_index" on the 6-th PHV word.**
We cannot provide more details, e.g., specifications or compiler instructions, due to NDA. But we believe that this step is easy if you have a Tofino switch. 

Second, inserting these programming instructions to the beginning of your P4 program.

Third, compile the P4 program via the switch compiler. You can check the PHV consumption in the compilation logs.

### Acknowledgement

This project is developed by Wenbin Zhang and Hongyan Liu. 

