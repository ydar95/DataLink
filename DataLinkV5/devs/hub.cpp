#include "hub.hpp"
#include "time.hpp"
devs::Hub::Hub(Digraph & _digraph, const std::string & _name, uint64_t _uid) 
  :AtomicAbstract(_digraph, _name, _uid)
  , port_broadcast_send(util::NextUid())
  , port_broadcast_recv(util::NextUid())
{
  this->_PushNextTimeSlice();
}

void devs::Hub::add_ju(SptrJu sptr_ju)
{
  ary_ju.push_back(sptr_ju);

  //绑定广播端口
  digraph.couple(this, this->port_broadcast_send, sptr_ju.get(), sptr_ju->portBroadcastRecv);  // HUB  -->  JU
  digraph.couple(sptr_ju.get(), sptr_ju->portBroadcastSend, this, this->port_broadcast_recv);  // JU  -->  HUB

  //绑定私有端口
  map_private_recv_port[sptr_ju->uid] = util::NextUid();  //生存 HUB 与JU 绑定的端口
  digraph.couple(this, map_private_recv_port[sptr_ju->uid], sptr_ju.get(), sptr_ju->portPrivateRecv);  //  HUB(private)->JU(private)
  

}

void devs::Hub::_PushNextTimeSlice()
{
  PortType port;
  if (ary_ju.empty())
    port = port_abandon;
  else {
    int idx = this->tdma_idx%ary_ju.size();
    port = map_private_recv_port[ary_ju[idx]->uid];
  }
  auto msg = msg::TimeSlice(tdma_idx*time_division_interval , (tdma_idx + 1)*time_division_interval);
  tdma_idx++;
  push_buffer(msg.begin_time, port, msg);
}

void devs::Hub::delta_int()
{
  priority_blob_queue.pop();
}

void devs::Hub::delta_ext(devs::TimeType e, const IO_Bag & xb)
{
  for (auto&iter : xb) {
    if (iter.port == this->port_broadcast_recv)
      push_buffer(Time::now(), this->port_broadcast_send, iter.value);
    else
      assert(false/*只能收到 用于全局转发的消息*/);
  }
}

void devs::Hub::output_func(IO_Bag & yb)
{
  auto&y = priority_blob_queue.top();
  yb.insert(y.io_buffer);

  if (y.io_buffer.value->blob_type<msg::MsgType>() == msg::MsgType::Msg_TimeSilce)
  {
    //判断是否发送的是时间片,如果是,就将新的时间片推入发送队列中
    this->_PushNextTimeSlice();
  }
}

devs::TimeType devs::Hub::ta()
{
  if (priority_blob_queue.empty())
    return TIME_MAX;
  return  Time::distance(priority_blob_queue.top().schedule_time);
}


