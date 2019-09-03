#pragma once
#include"../ju_component.hpp"
#include"../../utility/utility.hpp"
namespace devs::component {

  //���� ���涨ʱ��㲥 J3 ��Ϣ
  class CoreJ3BroadcastHandler
    : JuComponent
  {

  public:
    CoreJ3BroadcastHandler(Ju& ju, Digraph& _digraph, const std::string&_name, PortType _uid);
    _FUNC_MakeShared_(CoreJ3BroadcastHandler);

    // ͨ�� JuComponent �̳�
    virtual void delta_int() override;
    virtual void delta_ext(devs::TimeType e, const IO_Bag & xb) override;
    virtual void output_func(IO_Bag & yb) override;
    virtual devs::TimeType ta() override;
    
  private:
    bool is_recv_ts=false;

  
  };
  _FUNC_CreatSptr_(CoreJ3BroadcastHandler);
}