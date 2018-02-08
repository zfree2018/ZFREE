from scapy.all import *
import atexit


# 
packet_stats = {}

def print_stats():
    print("\npacket statistics")
    print(packet_stats)

atexit.register(print_stats)

def packet_process(packet):
    global packet_stats

    src_ip = packet[0][IP].src
    dst_ip = packet[0][IP].dst
    src_port = packet[0][TCP].sport
    dst_port = packet[0][TCP].dport

    seq_num = packet[0][TCP].seq
    ack_num = packet[0][TCP].ack
    print("{}:{}->{}:{} :: seq:{}, ack:{}, packet size:{}".format(
        src_ip, src_port, dst_ip, dst_port, seq_num, ack_num, len(packet[0])+2
    ))

    # accumalate the size of packets received.
    connection = "{}:{}->{}:{}".format(src_ip, src_port, dst_ip, dst_port)
    if connection in packet_stats:
        packet_stats[connection] += len(packet[0])
    else:
        packet_stats[connection] = len(packet[0])
    
    # import pdb; pdb.set_trace()
    try:
        print("TCP raw data (length: {}):\n {}".format(
            len(packet[0][Raw].load),
            packet[0][Raw].load))
    except Exception as e:
        print("")

sniff(filter="tcp and (port 5000 or port 5001)", prn=packet_process)
