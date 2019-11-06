#ifndef BL_SHADOW_LAYER_H
#define BL_SHADOW_LAYER_H

#include <fcntl.h>
#include <iostream>
#include <string>

#include "shadow_interface.h"

#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"

namespace libBLEEP_BL {
    class ShadowLayer {
    private:
        class ShadowPipeWatcher {
        private:
            ev::io _watcher;
            int _shadowpipeFD[2];

            static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
            {
                size_t pos = txt.find( ch );
                size_t initialPos = 0;
                strs.clear();

                // Decompose statement
                while( pos != std::string::npos ) {
                    strs.push_back( txt.substr( initialPos, pos - initialPos ) );
                    initialPos = pos + 1;

                    pos = txt.find( ch, initialPos );
                }

                // Add the last one
                strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

                return strs.size();
            }


            void _shadowPipeIOCallback (ev::io &w, int revents) {
                std::cout << "shadowpipe iocallback called!" << "\n";

                char string_read[2000];  
                memset(string_read, 0, 2000);
                int n;

                int fd = _shadowpipeFD[0];

                // receive command length
                int length = 0;
                n = read(fd,&length,sizeof(int));
                if (n == -1 && errno != EAGAIN){
                    perror("read - non blocking \n");
                    std::cout << "errno=" << errno << "\n";
                    exit(-1);
                }
                else if (n == 0) {
                    std::cout << "error: pipe disconnected" << "\n";
                    exit(-1);
                }

                // receive command
                int total_recv_size = 0;
                int numbytes = 0;
                std::string recv_str;
                while(1) {
                    int recv_size = std::min(2000, length);
                    numbytes = read(fd, string_read, recv_size);
                    if (numbytes > 0) {
                        total_recv_size += numbytes;
                        recv_str.append(string_read, numbytes);
                    }
                    else if (numbytes == 0) {
                        perror("recv_cmd fail (pipe closed) \n");
                        exit(-1);
                    }
                    else if (numbytes < 0) {
                        perror("recv_cmd fail \n");
                        std::cout << "errno=" << errno << "\n";
                        exit(-1);
                    }

                    if (total_recv_size == length)
                        break;
                }

                if (length != total_recv_size) {
                    std::cout << "error: received only part of command" << "\n";
                    exit(-1);
                }

                // parse command 
                std::string delimiter = ":";
                size_t pos = 0;
                std::string token;
                if ( (pos = recv_str.find(delimiter)) == std::string::npos ) {
                    std::cout << "error: wrong format for shadow-generated command (" << recv_str << ")" << "\n";
                    exit(-1);
                }
                else {
                    std::cout << "Successfully received shadow-generated command (" << recv_str << ")" << "\n";
                }
            
                std::string cmd_id = recv_str.substr(0, pos);
                std::string cmd_args_str = recv_str.substr(pos+1);
                std::vector<std::string> cmd_args;
                split( cmd_args_str, cmd_args, ' ');

                // TODO: Execute received shadow command,
                if (cmd_id == "shutdown") {
                    g_PeerConnectivityLayer_API->Shutdown();
                }
            }
        public:
            ShadowPipeWatcher() {
                int result = shadow_pipe2(_shadowpipeFD, 0);
                if (result < 0) {
                    perror("pipe ");
                    exit(-1);
                }
                fcntl(_shadowpipeFD[0], F_SETFL, O_NONBLOCK);

                _watcher.set<ShadowPipeWatcher, &ShadowPipeWatcher::_shadowPipeIOCallback>(this);
                _watcher.start(_shadowpipeFD[0], ev::READ);
            }
        };

        std::unique_ptr<ShadowPipeWatcher> _watcher;
    public:
        ShadowLayer() {
            _watcher = std::unique_ptr<ShadowPipeWatcher>(new ShadowPipeWatcher());
        }
    };

    extern std::unique_ptr<ShadowLayer> g_ShadowLayer;

}





#endif
