import sys
from scapy.all import *
from datetime import datetime

test_content = """HTTP/1.1 200 OK\r\nDate: Wed, 22 Nov 2017 02:13:40 GMT\r\nServer: Apache/2.2.22 (Ubuntu)\r\nLast-Modified: Tue, 21 Nov 2017 04:35:07 GMT\r\nAccept-Ranges: bytes\r\nContent-Length: 177\r\nKeep-Alive: timeout=500, max=100\r\nConnection: Keep-Alive\r\nContent-Type: text/html\r\n\r\n<html><body><h1>It !!!!!!</h1>
<p>This is the default web page for this server.!!!!!</p>
<p>The web server software is running but no content has been added, yet.</p>
</body></html>""" + datetime.now().strftime("%m/%y %H:%M:%S")

def main():
    """"""
    dst_ip = sys.argv[1]
    dst_port = int(sys.argv[2])

    src_ip = sys.argv[3]
    src_port = int(sys.argv[4])

    seq_n = int(sys.argv[5])
    ack_n = int(sys.argv[6])

    ip_id = int(sys.argv[7])
    ip_flags = int(sys.argv[8])

    # build packet
    ip = IP(src=src_ip, dst=dst_ip, id=ip_id, flags=ip_flags)
    tcp = ip / TCP(sport=src_port, dport=dst_port, flags='PA',
                    seq=seq_n, ack=ack_n) / test_content
    tcp.display()
    print("length of packet {}".format(len(tcp)))
    
    send(tcp)

if __name__ == '__main__':
    main()