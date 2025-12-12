#include "Utilty.hpp"

QString Utility::cameraIp1 = "1";
QString Utility::zmotionIp = "192.168.0.11";

std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> ClassId::classIdNameMap = {
	{ClassId::Bad,"缺陷"},
	{ClassId::FengKou,"封口"},
	{ClassId::JiaoDai,"胶带"}
};

std::vector<rw::imgPro::ClassId> ClassId::classids = {
	ClassId::Bad,
	ClassId::FengKou,
	ClassId::JiaoDai
};