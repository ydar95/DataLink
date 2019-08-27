#pragma once
#include"atomic.hpp"
#include"typedef.hpp"
#include"message/message.hpp"
#include"../utility/utility.hpp"
#include<memory>
#include<any>
namespace devs {
	class Ju
		:public AtomicAbstract
	{
	public :
		// J3 AT 的数据最后都到这里
		struct TrackInformation;
		static std::shared_ptr<Ju> make_shared(Digraph& _digraph, const std::string&_name, uint64_t _uid);
	public:
		Ju(Digraph& _digraph, const std::string&_name, uint64_t _uid);

		// used only c++17 or more than
		// usage:  auto [is_exist_at, is_exist_rt, is_exist_r2] = GetExist(track_name); 
		std::tuple<bool, bool, bool> GetExist(const std::string& track_name);

		void AddComponent(const std::string& name, std::any sptr_component);

	public:
		
		std::list<IO_Type>	buffer_list;			//从 hub 接收的消息 全部保存在这里. 立即通过 output 进行触发
		std::map<std::string, std::any> map_component;	//
		
		msg::TimeSlice		current_time_silce;		//保存 接收到的时间片信息
		

		//		
		util::sync::Map<std::string, TrackInformation>	dict_active_track;
		util::sync::Map<std::string, TrackInformation>	dict_recv_track;
		util::sync::Map<std::string, TimeType>			dict_r2;
	public:
		//与Component 传输使用
		const PortType		port_self_recv;			//内部使用的接收端口
		const PortType		port_self_send_cmd;		//CMD 转发
		const PortType		port_self_send_at;		//Active Track 消息转发
		const PortType		port_self_send_j3;		//J_3 消息转发
		const PortType		port_self_send_j7;		//J_7 消息转发
		const PortType		port_self_send_ts;		//TimeSlice 消息转发

		//主要传递 AT 消息
		const PortType		port_private_recv;		//与广播1v1接收端口
		//主要 传送 J 消息
		const PortType		port_broadcast_send;	//广播发送端口 
		const PortType		port_broadcast_recv;	//广播接收端口 

	public:
		// 通过 AtomicAbstract 继承
		virtual void delta_int() override;
		virtual void delta_ext(devs::TimeType e, const IO_Bag & xb) override;
		virtual void output_func(IO_Bag & yb) override;
		virtual devs::TimeType ta() override;
	};

	inline auto CreatSptrJu(Digraph& _digraph, const std::string&_name, uint64_t _uid) {
		return Ju::make_shared(_digraph, _name,_uid);
	}

	typedef std::shared_ptr<Ju> SptrJu;
}


struct devs::Ju::TrackInformation
{
	std::string track_number;
	std::string from_sut_name;

	msg::TrackPlatform track_platform;
	int32_t		track_quality;

	TimeType	time_msec;

	explicit TrackInformation()
	{
		track_number = "";
		from_sut_name = "";
		track_platform = msg::Platform_UNDEFINED;
		track_quality = -1;
		time_msec = -1;
	}

	explicit TrackInformation(const msg::ActiveTrack& at, const std::string& sut_name) {
		track_number = at.track_number;
		from_sut_name = sut_name;
		track_platform = at.track_platform;
		track_quality = at.track_quality;
		time_msec = at.time_msec;
	}

	explicit TrackInformation(const msg::JointMsg3_0I& j30i) {
		track_number = j30i.track_number;
		from_sut_name = j30i.from_sut_name;
		track_platform = j30i.track_platform;
		track_quality = j30i.track_quality;
		time_msec = j30i.time_msec;
	}

	template<class Ty> inline Ty get(TimeType _time = -1, bool is_new_info = true) {
		assert(false);//该类型不在生成范围内
		return Ty();
	}

	template <> inline msg::JointMsg3_0I get(TimeType _time, bool is_new_info) {

		std::string t_track_number = this->track_number;
		if (is_new_info)
			t_track_number = util::TrackNumberHandler::Create(
				util::TrackNumberHandler::GetName(t_track_number));

		auto ojb = msg::JointMsg3_0I(
			t_track_number.c_str(),
			this->from_sut_name.c_str(),
			this->track_platform,
			this->track_quality,
			this->time_msec
		);


		if (_time > 0)
			ojb.time_msec = _time;
		return ojb;
	}
};