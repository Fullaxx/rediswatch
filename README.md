# rediswatch
A simple utility to watch redis activity in real-time using psubscribe

## Software
[redis](https://redis.io/) \
[hiredis](https://github.com/redis/hiredis)

## Requirements
First we need to make sure we have all the appropriate libraries. \
Please consult this chart for help with installing the required packages. \
If your OS is not listed, please help us fill out the table, or submit a request via github.

| OS     | Commands (as root)                                                               |
| ------ | -------------------------------------------------------------------------------- |
| CentOS | `yum install -y epel-release; yum install -y gcc hiredis-devel libevent-devel`   |
| Debian | `apt update; apt install build-essential libhiredis-dev libevent-dev`            |
| Fedora | `yum install -y gcc hiredis-devel libevent-devel`                                |
| Ubuntu | `apt update; apt install build-essential libhiredis-dev libevent-dev`            |

## Compiling
```
./compile.sh
```

## Running printall
```
./printall.dbg 127.0.0.1 6379
```
To select another DB:
```
./printall.dbg 127.0.0.1 6379 1
```

## Running events
```
./events.dbg 172.17.0.1 6379
```
To select another DB:
```
./events.dbg 127.0.0.1 6379 1
```
