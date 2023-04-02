# swiftres
Simple tool for performing a large number of reverse DNS lookups.<br>
Provide it either with a single IP, a subnet, or a file containing a mixture of IPs/Subnets in CIDR notation.<br>
This can be very valuable during reconnaissance / information gathering stages of something like bug bounty hunting, provided that you know the IP Address ranges. 

## Installation

```sh
git clone https://github.com/essinghigh/swiftres
cd swiftres
make
```

## Usage
```
./swiftres [-s subnet / single ip | -f filename]
```
```
./swiftres -s 10.0.0.0/24
./swiftres -s 1.1.1.1
./swiftres -f ip_list.txt
```

## TODO
* Allow the selection of DNS servers (or multiple DNS servers with round-robin selection)
* Allow the user to select the number of threads used for lookups, currently this is 4 unless manually specified before building
