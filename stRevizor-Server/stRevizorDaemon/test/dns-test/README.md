### stRevizor
#### Registry RosKomNadzor DNS transporent filter reester test

+ This part of package stRevizor.
+ Run in local machine, default server ip address 127.0.0.1 and DNS transparent port, default: 5300
+ DNS list compiled is fast hash, based on CRC32.   
+ DNS list support wildcard scheme, compare after first dot substring.  

#### Example wildcard scheme:  

```shell

    # bad all subdomain, example:
    # news.domain.net, image.domain.net, other.domain.net
    .domain.net

    # full domain, no wildcard
    www.domain.organization.zone

```

#### Build and run:   

```shell

    # DNS filter stress test make and run:
     cd test/
     make clean
     make dns-test
     ./.build/revizor-dns-test/revizor-dns-test

```

#### Source:   

* [DNS filter reester test source](https://github.com/PetersSharp/stRevizor/tree/master/stRevizor-Server/test/dns-test)

