
import zmq
import struct
import ipaddress
import binascii
import threading

def reqrep_server(context):
    socket = context.socket(zmq.REP)

    socket.bind("tcp://*:5556")

    while True:
        try:
            message = socket.recv()
            print("Received request:" + str(message))
            if message == b'ping':
                socket.send(b"pong")
        except zmq.ContextTerminated:
            return







topic_tcpcontrol = "shadow_tcp_control".encode('ascii')
topic_tcpdata = "shadow_tcp_datastream".encode('ascii')
#topic = "fancyhw_data".encode('ascii')

print(f"Reading messages with topic: {topic_tcpcontrol}, {topic_tcpdata}")

with zmq.Context() as context:
    # run req-rep server as separate thread
    rep_th = threading.Thread(target=reqrep_server, args=(context,))
    rep_th.start()


    socket = context.socket(zmq.SUB)

    socket.bind("tcp://*:5555")
    socket.setsockopt(zmq.SUBSCRIBE, topic_tcpcontrol)
    socket.setsockopt(zmq.SUBSCRIBE, topic_tcpdata)

    i = 0
    print("connect done")

    try:
        while True:
            binary_topic, received_data = socket.recv().split(b' ', 1)
            # topic_size = len("shadow_tcp_stream")
            fd_size = 4
            port_size = 2
            addr_size = 4
            time_size = 8

            topic = binary_topic.decode(encoding='ascii')

            if topic == "shadow_tcp_control":
                binary_time = received_data[:time_size]
                binary_fd = received_data[time_size:time_size+fd_size]
                binary_from_addr = received_data[time_size+fd_size:time_size+fd_size+addr_size]
                binary_port = received_data[time_size+fd_size+addr_size:time_size+fd_size+addr_size+port_size]
                binary_addr = received_data[time_size+fd_size+addr_size+port_size:]


                print("Message {:d}:".format(i))

                time = struct.unpack("Q", binary_time)[0]
                time = time / 1000000000.0
                fd = struct.unpack("i", binary_fd)[0]
                from_addr = struct.unpack("!I", binary_from_addr)[0]
                from_addr = str(ipaddress.ip_address(from_addr))
                port = struct.unpack("!H", binary_port)[0]
                addr = struct.unpack("!I", binary_addr)[0]
                addr = str(ipaddress.ip_address(addr))


                print(f'[{time}] topic:{topic.split("shadow_")[1]}, fd:{fd}, {from_addr}-->{addr}:{port} (TCP connection)')
            elif topic == "shadow_tcp_datastream_send":
                binary_time = received_data[:time_size]
                binary_fd = received_data[time_size:time_size+fd_size]
                binary_from_port = received_data[time_size+fd_size:time_size+fd_size+port_size]
                binary_from_addr = received_data[time_size+fd_size+port_size:time_size+fd_size+port_size+addr_size]
                binary_peer_port = received_data[time_size+fd_size+port_size+addr_size:time_size+fd_size+port_size+addr_size+port_size]
                binary_peer_addr = received_data[time_size+fd_size+port_size+addr_size+port_size:time_size+fd_size + port_size + addr_size + port_size+addr_size]
                binary_buf = received_data[time_size+fd_size + port_size + addr_size + port_size+addr_size:]

                time = struct.unpack("Q", binary_time)[0]
                time = time / 1000000000.0
                fd = struct.unpack("i", binary_fd)[0]
                buf = binascii.hexlify(binary_buf)
                from_port = struct.unpack("!H", binary_from_port)[0]
                from_addr = struct.unpack("!I", binary_from_addr)[0]
                from_addr = str(ipaddress.ip_address(from_addr))
                peer_port = struct.unpack("!H", binary_peer_port)[0]
                peer_addr = struct.unpack("!I", binary_peer_addr)[0]
                peer_addr = str(ipaddress.ip_address(peer_addr))

                print(f'[{time}] topic:{topic.split("shadow_")[1]}, fd:{fd}, {from_addr}:{from_port}-->{peer_addr}:{peer_port}, buf:[{buf}]\n\t\t\tbufstr:[{binary_buf}]')
            elif topic == "shadow_tcp_datastream_recv":
                binary_time = received_data[:time_size]
                binary_fd = received_data[time_size:time_size+fd_size]
                binary_myport = received_data[time_size+fd_size:time_size+fd_size+port_size]
                binary_myaddr = received_data[time_size+fd_size+port_size:time_size+fd_size+port_size+addr_size]
                binary_buf = received_data[time_size+fd_size+port_size+addr_size:]

                time = struct.unpack("Q", binary_time)[0]
                time = time / 1000000000.0
                fd = struct.unpack("i", binary_fd)[0]
                buf = binascii.hexlify(binary_buf)
                myport = struct.unpack("!H", binary_myport)[0]
                myaddr = struct.unpack("!I", binary_myaddr)[0]
                myaddr = str(ipaddress.ip_address(myaddr))

                print(f'[{time}] topic:{topic.split("shadow_")[1]}, fd:{fd}, -->{myaddr}:{myport}, buf:[{buf}]')

            i += 1

    except KeyboardInterrupt:
        socket.close()
    except Exception as error:
        print("ERROR: {}".format(error))
        socket.close()
    finally:
        context.term()
