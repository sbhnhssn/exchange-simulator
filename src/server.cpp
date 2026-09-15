#include "server.hpp"
#include "protocol.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <mutex>
#include <iostream>
namespace { std::mutex m; template<class T>T rd(const uint8_t*b){T x=0;for(size_t i=0;i<sizeof(T);++i)x|=T(b[i])<<(8*i);return x;} }
void ExchangeServer::run(){
 int s=socket(AF_INET,SOCK_STREAM,0); if(s<0)throw std::runtime_error("socket");
 int one=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));
 sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_ANY);a.sin_port=htons(port_);
 if(bind(s,(sockaddr*)&a,sizeof(a))<0||listen(s,64)<0)throw std::runtime_error("bind/listen");
 std::cout<<"listening on "<<port_<<"\n";
 while(true){int c=accept(s,nullptr,nullptr);if(c<0)continue;
  std::thread([this,c]{uint8_t h[5];while(true){if(recv(c,h,5,MSG_WAITALL)!=5)break;uint32_t n=ntohl(rd<uint32_t>(h));uint8_t cmd=h[4];if(n>1024)break;std::vector<uint8_t>b(n);if(recv(c,b.data(),n,MSG_WAITALL)!=int(n))break;
   std::lock_guard<std::mutex>g(m);
   if(cmd==uint8_t(Command::NewOrder)&&n==26){Order o;o.id=rd<uint64_t>(b.data());o.side=Side(b[8]);o.type=OrderType(b[9]);o.price_ticks=rd<int64_t>(b.data()+10);o.quantity=rd<uint64_t>(b.data()+18);if(book_.submit(o))log_.append_submit(o);}
   else if(cmd==uint8_t(Command::Cancel)&&n==8){uint64_t id=rd<uint64_t>(b.data());if(book_.cancel(id))log_.append_cancel(id);}
   auto out=snapshot_json(book_);uint32_t len=htonl(uint32_t(out.size()));send(c,&len,4,0);send(c,out.data(),out.size(),0);
  }close(c);}).detach();
 }
}
